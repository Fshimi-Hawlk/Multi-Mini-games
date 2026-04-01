<?php
/**
 * translate.php — Doxygen documentation translation via LibreTranslate
 *
 * This script translates Markdown documentation from English to another language
 * using LibreTranslate, preserving Doxygen commands and Markdown links.
 *
 * Usage: php translate.php <lang>
 *   lang: target language code (e.g., fr, de, es)
 *
 * The script:
 *   1. Reads Markdown files from docs/doxygen/en/
 *   2. Translates them to the target language
 *   3. Writes translated files to docs/doxygen/src-<lang>/
 *   4. Generates a Doxyfile.<lang> for building translated HTML
 */

// ----------------------------------------------------------------
// Configuration
// ----------------------------------------------------------------
// Support both CLI and HTTP (GET parameter) invocation
if (php_sapi_name() === 'cli' && isset($argv[1])) {
    $lang = $argv[1];
} elseif (isset($_GET['lang'])) {
    $lang = $_GET['lang'];
} else {
    $lang = null;
}

$enDir = __DIR__ . '/en';
$frDir = __DIR__ . '/src-' . ($lang ?? 'fr');
$doxyfileSrc = __DIR__ . '/Doxyfile';
$LT_URL = 'http://localhost:5000';

if (!isset($lang) || $lang === 'en') {
    echo "[INFO] Language \"en\" is the source language — no translation needed.\n";
    exit(0);
}

// Translate each file via LibreTranslate REST API
// ----------------------------------------------------------------
if (!is_dir($frDir)) mkdir($frDir, 0755, true);

function ltTranslate(string $text, string $targetLang, string $ltUrl): string {
    if (trim($text) === '') return $text;

    $payload = json_encode([
        'q'      => $text,
        'source' => 'en',
        'target' => $targetLang,
        'format' => 'text',
    ]);

    $ctx = stream_context_create([
        'http' => [
            'method'  => 'POST',
            'header'  => "Content-Type: application/json\r\n",
            'content' => $payload,
            'timeout' => 60,
        ]
    ]);

    $resp = @file_get_contents($ltUrl . '/translate', false, $ctx);
    if ($resp === false) throw new RuntimeException('LibreTranslate request failed');

    $data = json_decode($resp, true);
    if (!isset($data['translatedText'])) throw new RuntimeException('Bad response: ' . $resp);
    return $data['translatedText'];
}

/**
 * Batch translate multiple texts at once for better performance.
 * LibreTranslate supports translating an array of texts in one request.
 */
function ltTranslateBatch(array $texts, string $targetLang, string $ltUrl): array {
    // Filter out empty texts but keep their positions
    $filtered = [];
    $map = [];
    foreach ($texts as $i => $text) {
        if (trim($text) !== '') {
            $filtered[] = $text;
            $map[] = $i;
        }
    }

    if (empty($filtered)) return $texts;

    $payload = json_encode([
        'q'      => $filtered,
        'source' => 'en',
        'target' => $targetLang,
        'format' => 'text',
    ]);

    $ctx = stream_context_create([
        'http' => [
            'method'  => 'POST',
            'header'  => "Content-Type: application/json\r\n",
            'content' => $payload,
            'timeout' => 120,
        ]
    ]);

    $resp = @file_get_contents($ltUrl . '/translate', false, $ctx);
    if ($resp === false) throw new RuntimeException('LibreTranslate batch request failed');

    $data = json_decode($resp, true);
    if (!isset($data['translatedText']) || !is_array($data['translatedText'])) {
        throw new RuntimeException('Bad batch response: ' . $resp);
    }

    // Map results back to original positions
    $results = array_fill(0, count($texts), '');
    foreach ($data['translatedText'] as $i => $translated) {
        $results[$map[$i]] = $translated;
    }

    return $results;
}

/**
 * Splits content into segments [text, isCode].
 * isCode=true -> do not translate.
 *
 * FIX: protection of inline backticks (`...`) in addition to triple-backtick blocks.
 * The old version sent inline spans to LibreTranslate, which could break
 * command or symbol names (spaces inserted, characters altered).
 */
function splitSegments(string $content): array {
    $segments = [];
    // Ordre important : triple backtick avant simple backtick pour éviter les faux positifs
    preg_match_all(
        '/```[\s\S]*?```|`[^`\n]+`|@verbatim[\s\S]*?@endverbatim/m',
        $content,
        $matches,
        PREG_OFFSET_CAPTURE
    );
    $last = 0;
    foreach ($matches[0] as [$match, $offset]) {
        if ($offset > $last) $segments[] = [substr($content, $last, $offset - $last), false];
        $segments[] = [$match, true];
        $last = $offset + strlen($match);
    }
    if ($last < strlen($content)) $segments[] = [substr($content, $last, strlen($content) - $last), false];
    return $segments;
}


/**
 * Replaces all tokens to protect (Doxygen and Markdown hyperlinks) with
 * opaque placeholders before translation, then restores them after.
 *
 * Protected tokens:
 *   @ref    ID ["label"]   — internal Doxygen link
 *   @subpage ID ["label"]  — Doxygen subpage link
 *   [text](url)            — Markdown link (URL preserved, visible text translated)
 *
 * @param string $line   Line to process
 * @param array  $tokens  Reference-passed array, accumulated between calls
 * @param string $lang   Target language (to translate Markdown link text)
 * @param string $ltUrl  URL of the LibreTranslate instance
 */
function protectTokens(string $line, array &$tokens, string $lang, string $ltUrl): string {
    // @ref and @subpage: keep both the identifier AND label as-is.
    // The ID is the link resolution key for Doxygen — it must never be translated.
    // The label is the displayed text: we preserve it too for consistency between versions.
    // Use self-closing XML tags that won't be modified by translation
    $line = preg_replace_callback(
        '/@(?:ref|subpage)\s+\w+(?:\s+"[^"]*")?/',
        function ($m) use (&$tokens) {
            $token          = '<TOK' . count($tokens) . '/>';
            $tokens[$token] = $m[0];
            return $token;
        },
        $line
    );

    // Markdown links [text](url): translate display text, preserve URL intact.
    // Use self-closing XML tags that won't be modified by translation
    $line = preg_replace_callback(
        '/\[([^\]]*)\]\(([^)]+)\)/',
        function ($m) use (&$tokens, $lang, $ltUrl) {
            $displayText = $m[1];
            $url         = $m[2];
            try {
                $translated = $displayText ? ltTranslate($displayText, $lang, $ltUrl) : $displayText;
            } catch (Exception $e) {
                $translated = $displayText;
            }
            $token          = '<TOK' . count($tokens) . '/>';
            $tokens[$token] = '[' . $translated . '](' . $url . ')';
            return $token;
        },
        $line
    );

    return $line;
}

function restoreTokens(string $line, array $tokens): string {
    foreach ($tokens as $token => $original) {
        $line = str_replace($token, $original, $line);
    }
    return $line;
}

/**
 * Translate prose (non-code) content with batch optimization.
 * Groups simple lines together for bulk translation to improve performance.
 */
function translateProse(string $text, string $lang, string $ltUrl): string {
    $lines  = explode("\n", $text);
    $result = [];

    // Batch translation buffer: group lines without complex tokens for faster translation
    $batchTexts = [];
    $batchIndices = [];

    foreach ($lines as $idx => $line) {
        $stripped = trim($line);

        // Ligne vide - flush batch first
        if ($stripped === '') {
            // Translate any pending batch
            if (!empty($batchTexts)) {
                try {
                    $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $translatedBatch[$i];
                    }
                } catch (Exception $e) {
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $batchTexts[$i];
                    }
                }
                $batchTexts = [];
                $batchIndices = [];
            }
            $result[$idx] = $line;
            continue;
        }

        // @page / @section / etc. : conserver l'ID, traduire le titre uniquement
        if (preg_match('/^(\s*@(?:page|section|subsection|subsubsection|anchor)\s+\w+\s*)(.*)/u', $line, $m)) {
            // Flush batch first
            if (!empty($batchTexts)) {
                try {
                    $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $translatedBatch[$i];
                    }
                } catch (Exception $e) {
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $batchTexts[$i];
                    }
                }
                $batchTexts = [];
                $batchIndices = [];
            }
            $title    = trim($m[2]);
            $result[$idx] = $m[1] . ($title ? ltTranslate($title, $lang, $ltUrl) : '');
            continue;
        }

        // Directive seule sans texte (@code, @endcode, @mainpage, etc.)
        if (preg_match('/^\s*@\w[\w_]*\s*$/', $line)) {
            // Flush batch first
            if (!empty($batchTexts)) {
                try {
                    $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $translatedBatch[$i];
                    }
                } catch (Exception $e) {
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $batchTexts[$i];
                    }
                }
                $batchTexts = [];
                $batchIndices = [];
            }
            $result[$idx] = $line;
            continue;
        }

        // @ref ou @subpage seuls sur leur ligne (éventuellement suivi de texte)
        // Conserver intact : l'ID et le label sont des hyperliens Doxygen.
        if (preg_match('/^\s*@(?:ref|subpage)\s+\w+(?:\s+"[^"]*")?(.*)$/u', $line, $m)) {
            // Flush batch first
            if (!empty($batchTexts)) {
                try {
                    $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $translatedBatch[$i];
                    }
                } catch (Exception $e) {
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $batchTexts[$i];
                    }
                }
                $batchTexts = [];
                $batchIndices = [];
            }
            $after = trim($m[1]);
            if ($after !== '') {
                // Texte ordinaire après le lien (ponctuation, phrase) : on le traduit
                $tokens    = [];
                $protected = protectTokens($line, $tokens, $lang, $ltUrl);
                try {
                    $translated = ltTranslate($protected, $lang, $ltUrl);
                    $result[$idx]   = restoreTokens($translated, $tokens);
                } catch (Exception $e) {
                    $result[$idx] = $line;
                }
            } else {
                $result[$idx] = $line;
            }
            continue;
        }

        // Directive suivie de prose (@brief, @note, @param[in] name ...)
        // Protéger les tokens inline (@ref, @subpage, liens MD) avant de traduire.
        if (preg_match('/^(\s*@\w[\w_]*\s+)(.*)/u', $line, $m)) {
            // Flush batch first
            if (!empty($batchTexts)) {
                try {
                    $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $translatedBatch[$i];
                    }
                } catch (Exception $e) {
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $batchTexts[$i];
                    }
                }
                $batchTexts = [];
                $batchIndices = [];
            }
            $tokens    = [];
            $prose     = protectTokens($m[2], $tokens, $lang, $ltUrl);
            try {
                $translated = ltTranslate(trim($prose), $lang, $ltUrl);
                $result[$idx]   = $m[1] . restoreTokens($translated, $tokens);
            } catch (Exception $e) {
                $result[$idx] = $line;
            }
            continue;
        }

        // Titre Markdown : protéger les tokens inline avant traduction
        if (preg_match('/^(\s*#{1,6}\s+)(.*)/u', $line, $m)) {
            // Flush batch first
            if (!empty($batchTexts)) {
                try {
                    $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $translatedBatch[$i];
                    }
                } catch (Exception $e) {
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $batchTexts[$i];
                    }
                }
                $batchTexts = [];
                $batchIndices = [];
            }
            $tokens   = [];
            $prose    = protectTokens($m[2], $tokens, $lang, $ltUrl);
            try {
                $translated = ltTranslate(trim($prose), $lang, $ltUrl);
                $result[$idx]   = $m[1] . restoreTokens($translated, $tokens);
            } catch (Exception $e) {
                $result[$idx] = $line;
            }
            continue;
        }

        // Séparateur de tableau
        if (preg_match('/^\s*\|[-| :]+\|\s*$/', $line)) {
            // Flush batch first
            if (!empty($batchTexts)) {
                try {
                    $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $translatedBatch[$i];
                    }
                } catch (Exception $e) {
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $batchTexts[$i];
                    }
                }
                $batchTexts = [];
                $batchIndices = [];
            }
            $result[$idx] = $line;
            continue;
        }

        // URL ou chemin pur (pas d'espace = rien à traduire)
        if (preg_match('/^\s*(https?:\/\/|\.\.\/?\/|\w+\/)/', $stripped) && strpos($stripped, ' ') === false) {
            // Flush batch first
            if (!empty($batchTexts)) {
                try {
                    $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $translatedBatch[$i];
                    }
                } catch (Exception $e) {
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $batchTexts[$i];
                    }
                }
                $batchTexts = [];
                $batchIndices = [];
            }
            $result[$idx] = $line;
            continue;
        }

        // Cas général avec tokens protégés - translate individually
        if (strpos($line, '@ref') !== false || strpos($line, '@subpage') !== false || strpos($line, '](') !== false) {
            // Flush batch first
            if (!empty($batchTexts)) {
                try {
                    $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $translatedBatch[$i];
                    }
                } catch (Exception $e) {
                    foreach ($batchIndices as $i => $origIdx) {
                        $result[$origIdx] = $batchTexts[$i];
                    }
                }
                $batchTexts = [];
                $batchIndices = [];
            }
            // Translate individually with token protection
            try {
                $tokens    = [];
                $protected = protectTokens($line, $tokens, $lang, $ltUrl);
                $translated = ltTranslate($protected, $lang, $ltUrl);
                $result[$idx]   = restoreTokens($translated, $tokens);
            } catch (Exception $e) {
                $result[$idx] = $line;
            }
            continue;
        }

        // Simple prose line - add to batch for bulk translation
        $batchTexts[] = $line;
        $batchIndices[] = $idx;
    }

    // Translate any remaining batch
    if (!empty($batchTexts)) {
        try {
            $translatedBatch = ltTranslateBatch($batchTexts, $lang, $ltUrl);
            foreach ($batchIndices as $i => $origIdx) {
                $result[$origIdx] = $translatedBatch[$i];
            }
        } catch (Exception $e) {
            foreach ($batchIndices as $i => $origIdx) {
                $result[$origIdx] = $batchTexts[$i];
            }
        }
    }

    // Ensure results are in correct order and rebuild the text
    ksort($result);
    return implode("\n", $result);
}


// ----------------------------------------------------------------
// Main translation loop
// ----------------------------------------------------------------

// FIX: glob() does not support recursive ** patterns in PHP.
//      'en/**/*.md' would only match one level deep, missing top-level
//      files like mainpage.md, build-system-details.md, etc.
//      Use RecursiveDirectoryIterator for proper recursive discovery.
function collectMdFiles(string $dir): array {
    $files = [];
    if (!is_dir($dir)) return $files;
    $it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($dir));
    foreach ($it as $f) {
        if ($f->isFile() && $f->getExtension() === 'md') {
            $files[] = $f->getPathname();
        }
    }
    sort($files);
    return $files;
}

$files = collectMdFiles($enDir);
if (empty($files)) {
    echo "ERROR: No source files found in $enDir\n";
    exit(1);
}

// OPT: LT_MAX_CHARS limits payload per batch request to stay under LT's limit.
// Simple prose lines across the whole file are collected and sent in chunks,
// reducing HTTP round-trips from ~N_paragraphs to ceil(file_chars / LT_MAX_CHARS).
define('LT_MAX_CHARS', 4000);

/**
 * Translate a list of simple prose lines as chunked batch requests.
 * Lines with @ref/@subpage tokens are handled individually by translateProse().
 *
 * @param string[] $lines      Lines to translate
 * @param string   $lang       Target language code
 * @param string   $ltUrl      LibreTranslate base URL
 * @return string[]            Translated lines (same indices)
 */
function translateLinesBatched(array $lines, string $lang, string $ltUrl): array {
    $result = $lines; // start with originals as fallback
    $chunk  = [];
    $idxMap = [];
    $charCount = 0;

    $flush = function() use (&$chunk, &$idxMap, &$charCount, &$result, $lang, $ltUrl) {
        if (empty($chunk)) return;
        try {
            $translated = ltTranslateBatch($chunk, $lang, $ltUrl);
            foreach ($idxMap as $bi => $origIdx) {
                $result[$origIdx] = $translated[$bi] ?? $chunk[$bi];
            }
        } catch (Exception $e) {
            // keep originals on failure
        }
        $chunk = []; $idxMap = []; $charCount = 0;
    };

    foreach ($lines as $i => $line) {
        $len = strlen($line);
        if ($charCount + $len > LT_MAX_CHARS) $flush();
        $idxMap[] = $i;
        $chunk[]  = $line;
        $charCount += $len + 1; // +1 for newline
    }
    $flush();
    return $result;
}

foreach ($files as $src) {
    $rel  = substr($src, strlen($enDir) + 1);
    $dest = $frDir . '/' . $rel;

    if (file_exists($dest)) {
        out('[SKIP] ' . $rel);
        continue;
    }

    @mkdir(dirname($dest), 0755, true);
    out('[....] ' . $rel);

    $content  = file_get_contents($src);
    $segments = splitSegments($content);
    $output   = '';

    // OPT: two-pass translation per file
    // Pass 1: collect all simple prose lines across all segments
    // Pass 2: batch-translate them, then reintegrate into segments
    $allSimpleLines = [];   // [segIdx][lineIdx] => text
    $allSegParsed   = [];   // pre-parsed structure per segment

    foreach ($segments as $si => [$seg, $isCode]) {
        if ($isCode) {
            $allSegParsed[$si] = ['code' => $seg];
            continue;
        }
        $parsed = ['lines' => [], 'simple' => [], 'special' => []];
        $lines  = explode("
", $seg);
        foreach ($lines as $li => $line) {
            $stripped = trim($line);
            // Classify: simple (batchable) vs special (needs protectTokens or passthrough)
            $isSpecial = (
                $stripped === '' ||
                preg_match('/^\s*@\w/', $line) ||
                preg_match('/^\s*#{1,6}\s/', $line) ||
                preg_match('/^\s*\|[-| :]+\|\s*$/', $line) ||
                (preg_match('/^\s*(https?:\/\/|\.\.\/?\/)/', $stripped) && strpos($stripped, ' ') === false) ||
                strpos($line, '@ref') !== false ||
                strpos($line, '@subpage') !== false ||
                strpos($line, '](') !== false
            );
            $parsed['lines'][$li]   = $line;
            $parsed['simple'][$li]  = !$isSpecial;
            if (!$isSpecial && $stripped !== '') {
                $allSimpleLines[] = ['si' => $si, 'li' => $li, 'text' => $line];
            }
        }
        $allSegParsed[$si] = ['prose' => $parsed];
    }

    // Batch translate all simple lines
    $simpleTexts = array_column($allSimpleLines, 'text');
    $simpleTranslated = translateLinesBatched($simpleTexts, $lang, $LT_URL);

    // Distribute results back
    foreach ($allSimpleLines as $idx => $entry) {
        $allSegParsed[$entry['si']]['prose']['translated'][$entry['li']] =
            $simpleTranslated[$idx];
    }

    // Reconstruct output
    foreach ($segments as $si => [$seg, $isCode]) {
        if ($isCode) { $output .= $seg; continue; }

        $parsed  = $allSegParsed[$si]['prose'];
        $result  = [];
        $lines   = explode("\n", $seg);

        foreach ($lines as $li => $line) {
            if (isset($parsed['translated'][$li])) {
                // Already batch-translated
                $result[$li] = $parsed['translated'][$li];
            } else {
                // Special line: use per-line translation (protectTokens etc.)
                $singleLine = translateProse($line, $lang, $LT_URL);
                $result[$li] = $singleLine;
            }
        }
        ksort($result);
        $output .= implode("\n", $result);
    }

    file_put_contents($dest, $output);
    out('[OK]   ' . $rel);
}

// ----------------------------------------------------------------
// Generate Doxyfile for the new language
// FIX: the multiline INPUT regex with /m alone did not traverse newlines
// in PHP PCRE. We now replace INPUT line by line reliably.
// ----------------------------------------------------------------
out('[INFO] Writing Doxyfile.' . $lang . '...');
$doxyLines   = file($doxyfileSrc);
$doxyOutput  = [];
$inInput     = false;

foreach ($doxyLines as $line) {
    // Detect start of an INPUT block (potentially multiline with \)
    if (preg_match('/^\s*INPUT\s*=/', $line)) {
        // Emit our replacement
        $doxyOutput[] = 'INPUT                   = src-' . $lang . "\n";
        $inInput = (substr(rtrim($line), -1) === '\\'); // continuation ?
        continue;
    }
    // Skip continuation lines of the original INPUT block
    if ($inInput) {
        $inInput = (substr(rtrim($line), -1) === '\\');
        continue;
    }

    // Change HTML output folder
    if (preg_match('/^\s*HTML_OUTPUT\s*=\s*html\s*$/', $line)) {
        $doxyOutput[] = 'HTML_OUTPUT             = html-' . $lang . "\n";
        continue;
    }

    // Injecter OUTPUT_LANGUAGE juste après GENERATE_HTML = YES
    if (preg_match('/^\s*GENERATE_HTML\s*=\s*YES/', $line)) {
        $doxyOutput[] = $line;
        $doxyOutput[] = 'OUTPUT_LANGUAGE         = ' . ucfirst($lang) . "\n";
        continue;
    }

<<<<<<< HEAD
=======
    // Change RECURSIVE to YES for translations (scan subdirectories like project-details/, build-system-details/, team-guidelines/)
    if (preg_match('/^\s*RECURSIVE\s*=\s*NO/', $line)) {
        $doxyOutput[] = 'RECURSIVE               = YES' . "\n";
        continue;
    }

>>>>>>> 3777fd6 (- add : new 3D golf game)
    // Point mainpage to the copied version (not translated but with directives intact)
    if (preg_match('/^\s*USE_MDFILE_AS_MAINPAGE\s*=/', $line)) {
        $doxyOutput[] = 'USE_MDFILE_AS_MAINPAGE  = src-' . $lang . '/mainpage.md' . "\n";
        continue;
    }

    // HTML_HEADER is intentionally kept: header.html contains the link
    // "../index.php" which works from html-XX/ (same level as html/).
    // Without it, the user no longer has access to the English button from a translated page.
    // HTML_EXTRA_FILES is removed because it references EN doc-specific files
    // that are not needed for translations.
    if (preg_match('/^\s*HTML_EXTRA_FILES\s*=/', $line))  continue;

    $doxyOutput[] = $line;
}

file_put_contents($doxyfileSrc . '.' . $lang, implode('', $doxyOutput));
out('[OK]   Doxyfile.' . $lang . ' written.');

out('[INFO] Running Doxygen for language "' . $lang . '"...');
$doxyOut = [];
$doxyExit = 0;
exec('doxygen ' . $doxyfileSrc . '.' . $lang . ' 2>&1', $doxyOut, $doxyExit);

if ($doxyExit === 0) {
    out('[OK]   Doxygen done. Output: html-' . $lang . '/');
} else {
    out('[ERR]  Doxygen failed with exit code ' . $doxyExit);
    foreach ($doxyOut as $l) {
        if (preg_match('/^warning:/', $l)) out('       ' . $l);
    }
}

out('[OK]   Translation complete. Reload the page to see the new language.');


// ----------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------
function out(string $msg): void {
    echo $msg . "\n";
}
