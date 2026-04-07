#!/bin/bash
# generate-root-docs.sh
# Generates English documentation. Other languages via index.php + LibreTranslate.

set -e

PROJNAME=$(basename "$PWD")
echo "Generating documentation for ${PROJNAME}..."

rm -rf docs/doxygen/html 2>/dev/null || true

cd docs/doxygen
doxygen Doxyfile

# FIX: index.html — the title contained $(basename "$PWD") literally because the heredoc
# used single quotes ('REDIRECT'). Replaced with an interpolated variable.
cat > index.html << REDIRECT
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="0; url=html/index.html">
    <title>Documentation - ${PROJNAME}</title>
    <style>
        body { font-family: system-ui, sans-serif; text-align: center; padding: 4rem; }
        a { color: #0066cc; }
    </style>
</head>
<body>
    <h1>Documentation generated</h1>
    <p>Redirecting to the full documentation...</p>
    <p>If nothing happens, <a href="html/index.html">click here</a>.</p>
</body>
</html>
REDIRECT

echo "Done. Open docs/doxygen/index.php (via php -S localhost:8080) or docs/doxygen/index.html."
