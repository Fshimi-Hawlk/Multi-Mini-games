#!/bin/bash
# translate-root-docs.sh
# Translates documentation via LibreTranslate
# Usage: ./translate-root-docs.sh <lang1,lang2,...>

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

LT_URL="http://localhost:5000"
PHP_PORT=8080
PHP_DIR="docs/doxygen"

echo -e "${YELLOW}=== Documentation Translation Script ===${NC}"
echo ""

# ----------------------------------------------------------------
# FIX: check dependencies before starting any service.
# If libretranslate or php is missing, the script was waiting 300s in vain.
# ----------------------------------------------------------------
for cmd in libretranslate php curl; do
    if ! command -v "$cmd" &>/dev/null; then
        echo -e "${RED}ERROR: '$cmd' not found in PATH.${NC}"
        case "$cmd" in
            libretranslate) echo "  -> Install with: pipx install libretranslate" ;;
            php)            echo "  -> Install with: sudo apt install php" ;;
            curl)           echo "  -> Install with: sudo apt install curl" ;;
        esac
        exit 1
    fi
done

get_available_langs() {
    curl -s "${LT_URL}/languages" 2>/dev/null | grep -o '"code":"[^"]*"' | cut -d'"' -f4
}

lang_is_available() {
    local lang=$1
    get_available_langs | grep -q "^${lang}$"
}

# FIX: php_is_ready checks for HTTP 200 rather than simple curl success.
# The old version returned 0 even on a 500 or 404 response.
php_is_ready() {
    local code
    code=$(curl -s -o /dev/null -w "%{http_code}" "http://localhost:${PHP_PORT}/index.php" 2>/dev/null)
    [ "$code" = "200" ]
}

stop_libretranslate() {
    echo -e "${YELLOW}Stopping existing LibreTranslate...${NC}"
    pkill -f "libretranslate" 2>/dev/null || true
    sleep 2
}

start_libretranslate() {
    local lang=$1
    echo -e "${YELLOW}Starting LibreTranslate (lang: ${lang})...${NC}"

    stop_libretranslate

    libretranslate --load-only en,"${lang}" --no-api-keys > /tmp/libretranslate.log 2>&1 &
    local lt_pid=$!

    local waited=0
    set +e
    while [ $waited -lt 300 ]; do
        sleep 2
        waited=$((waited + 2))
        echo -n "."

        # Display filtered logs every 30s
        if [ $((waited % 30)) -eq 0 ]; then
            echo ""
            echo -e "${YELLOW}Logs:${NC}"
            grep -v "RequestsDependencyWarning\|warnings\.warn\|DeprecationWarning\|UserWarning\|InsecureRequestWarning" \
                /tmp/libretranslate.log 2>/dev/null | tail -5 || true
        fi

        # Check if language is available
        if lang_is_available "${lang}" 2>/dev/null; then
            break
        fi

        # If LT responds but language is not yet there, it's loading
        # (downloading model) — continue waiting without restarting
        if curl -s --max-time 2 "http://localhost:5000/languages" > /dev/null 2>&1; then
            : # LT is active, wait for model to finish loading
        else
            # LT not responding at all — check if it's still running
            if ! kill -0 "$lt_pid" 2>/dev/null; then
                echo ""
                echo -e "${RED}ERROR: LibreTranslate stopped unexpectedly${NC}"
                echo "Logs:"
                grep -v "RequestsDependencyWarning\|warnings\.warn\|DeprecationWarning" \
                    /tmp/libretranslate.log 2>/dev/null || true
                set -e
                exit 1
            fi
        fi
    done
    set -e
    echo ""

    if ! lang_is_available "${lang}" 2>/dev/null; then
        echo -e "${RED}ERROR: LibreTranslate did not load '${lang}' (timeout 300s)${NC}"
        echo "Logs:"
        grep -v "RequestsDependencyWarning\|warnings\.warn\|DeprecationWarning" \
            /tmp/libretranslate.log 2>/dev/null || true
        echo ""
        echo "Tip: First download can take several minutes (~300 MB per language)."
        exit 1
    fi
    echo -e "${GREEN}LibreTranslate ready (${lang} available)${NC}"
}

start_php_server() {
    echo -e "${YELLOW}Starting PHP server on port ${PHP_PORT}...${NC}"

    pkill -f "php -S localhost:${PHP_PORT}" 2>/dev/null || true
    sleep 1

    cd "${PHP_DIR}"
    php -S "localhost:${PHP_PORT}" > /tmp/php-server.log 2>&1 &
    cd - > /dev/null
    sleep 3

    if ! php_is_ready; then
        echo -e "${RED}ERROR: PHP server did not start${NC}"
        cat /tmp/php-server.log
        exit 1
    fi
    echo -e "${GREEN}PHP server ready${NC}"
}

# FIX: translate_lang — detects ERROR: lines in PHP stream.
# The old version always returned 0 because curl succeeded even if PHP wrote an error.
translate_lang() {
    local lang=$1
    local has_error=0

    echo ""
    echo -e "${GREEN}Translating to ${lang}...${NC}"

    # set +e : the curl process substitution can return non-zero (timeout, etc.)
    # without being a fatal error — we handle this manually.
    set +e
    while IFS= read -r line; do
        echo "$line"
        if [[ "$line" == ERROR:* ]]; then
            has_error=1
        fi
    done < <(curl -sN "http://localhost:${PHP_PORT}/translate.php?lang=${lang}" 2>&1)
    local curl_exit=$?
    set -e

    if [ $curl_exit -ne 0 ]; then
        echo -e "${RED}Connection failed for translation ${lang} (curl exit ${curl_exit})${NC}"
        return 1
    fi

    if [ $has_error -eq 1 ]; then
        echo -e "${RED}Translation ${lang} encountered errors (see above)${NC}"
        return 1
    fi

    echo -e "${GREEN}Translation ${lang} complete${NC}"
}

# ----------------------------------------------------------------
# MAIN
# ----------------------------------------------------------------

if [ -z "$1" ]; then
    echo "Usage: $0 <lang1,lang2,...>"
    echo "Example: $0 fr,de,es"
    echo ""
    echo "Prerequisites:"
    echo "  - LibreTranslate : pipx install libretranslate"
    echo "  - PHP            : sudo apt install php"
    exit 1
fi

IFS=',' read -ra LANGS <<< "$1"

# FIX bug "en" : intercept before any service start.
# translate.php would reject "en" with ERROR:, which would crash the script.
# We filter here with a clear message, without touching LT or PHP.
FILTERED_LANGS=()
for lang in "${LANGS[@]}"; do
    if [ "$lang" = "en" ]; then
        echo -e "${YELLOW}[SKIP] 'en' — English documentation is already generated by 'make docs'.${NC}"
        echo "       Open docs/doxygen/html/index.html or http://localhost:${PHP_PORT}/html/index.html"
    else
        FILTERED_LANGS+=("$lang")
    fi
done

# If all requested languages were "en", we stop cleanly
if [ ${#FILTERED_LANGS[@]} -eq 0 ]; then
    echo ""
    echo -e "${GREEN}Nothing to translate.${NC}"
    exit 0
fi

FIRST_LANG="${FILTERED_LANGS[0]}"

echo -e "${YELLOW}Checking services...${NC}"
echo ""

# FIX bug "de"/"es" : lang_is_available returns 1 if LT is not started,
# which with set -e crashed the script before calling start_libretranslate.
# We disable set -e only for availability tests (|| true).
if lang_is_available "${FIRST_LANG}" 2>/dev/null; then
    echo -e "${GREEN}LibreTranslate already active with ${FIRST_LANG}${NC}"
else
    start_libretranslate "${FIRST_LANG}"
fi

if php_is_ready; then
    echo -e "${GREEN}PHP server already active${NC}"
else
    start_php_server
fi

for lang in "${FILTERED_LANGS[@]}"; do
    if ! lang_is_available "${lang}" 2>/dev/null; then
        echo -e "${YELLOW}Switching language to ${lang}...${NC}"
        start_libretranslate "${lang}"
    fi
    translate_lang "${lang}"
done

echo ""
echo -e "${GREEN}=== All translations completed! ===${NC}"
echo ""
echo "Documentation available at:"
for lang in "${FILTERED_LANGS[@]}"; do
    echo "  http://localhost:${PHP_PORT}/html-${lang}/index.html"
done
