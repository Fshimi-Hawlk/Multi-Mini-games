<?php
/**
 * translate.php
 * Triggered by index.php via fetch().
 * Translates all EN .md docs to the requested language using local LibreTranslate.
 * Streams progress line by line.
 *
 * GET param: lang  (ISO 639-1 code, e.g. "de", "es", "ja")
 */

header('Content-Type: text/plain; charset=utf-8');
header('X-Accel-Buffering: no'); // disable nginx buffering if any
if (ob_get_level()) ob_end_clean();

// Flush helper
function out(string $msg): void {
    echo $msg . "\n";
    flush();
}

// ----------------------------------------------------------------
// Validate input
// ----------------------------------------------------------------
$lang = $_GET['lang'] ?? '';
if (!preg_match('/^[a-z]{2,5}$/', $lang)) {
    out('ERROR: Invalid language code.');
    exit;
}
if ($lang === 'en') {
    out('ERROR: Cannot translate to source language (en).');
    exit;
}

// ----------------------------------------------------------------
// Paths
// ----------------------------------------------------------------
$doxygenDir = __DIR__;
$enDir      = $doxygenDir . '/en';
$targetDir  = $doxygenDir . '/html-' . $lang;
$frDir      = $doxygenDir . '/src-' . $lang; // translated .md sources
$doxyfileSrc = $doxygenDir . '/Doxyfile'; // use EN Doxyfile as template
$doxyfileTgt = $doxygenDir . '/Doxyfile.' . $lang;
$scriptPath  = __DIR__ . '/translate_md.py';
$LT_URL      = 'http://localhost:5000';

// ----------------------------------------------------------------
// Auto-start LibreTranslate if not running
// ----------------------------------------------------------------
out('[INFO] Checking LibreTranslate...');

function ltIsUp(string $url): bool {
    $ctx = stream_context_create(['http' => ['timeout' => 3]]);
    return @file_get_contents($url . '/languages', false, $ctx) !== false;
}

function waitForLt(string $url, string $lang, string $logFile, int $maxWait): bool {
    $waited   = 0;
    $lastSize = 0;
    while ($waited < $maxWait) {
        sleep(2);
        $waited += 2;
        if (file_exists($logFile)) {
            $c = file_get_contents($logFile);
            if (strlen($c) > $lastSize) {
                foreach (explode("\n", substr($c, $lastSize)) as $l) {
                    if (trim($l)) out('[LT]   ' . trim($l));
                }
                $lastSize = strlen($c);
            }
        }
        if (ltIsUp($url)) {
            $lj = @file_get_contents($url . '/languages');
            $supported = array_column(json_decode($lj, true) ?? [], 'code');
            if (in_array($lang, $supported)) return true;
        }
    }
    return false;
}

$logFile = sys_get_temp_dir() . '/libretranslate.log';

if (!ltIsUp($LT_URL)) {
    out('[INFO] LibreTranslate not running — starting it (may download models)...');
    @file_put_contents($logFile, '');
    exec('libretranslate --load-only en,' . escapeshellarg($lang) . ' > ' . escapeshellarg($logFile) . ' 2>&1 &');
    if (!waitForLt($LT_URL, $lang, $logFile, 180)) {
        out('ERROR: LibreTranslate did not start within 3 minutes. Check: ' . $logFile);
        exit;
    }
    out('[OK]   LibreTranslate is up.');
} else {
    // Already running — check if requested lang is loaded
    $lj = @file_get_contents($LT_URL . '/languages');
    $supported = array_column(json_decode($lj, true) ?? [], 'code');
    if (!in_array($lang, $supported)) {
        out('[INFO] Language "' . $lang . '" not loaded — restarting LibreTranslate...');
        exec('pkill -f libretranslate 2>/dev/null');
        sleep(2);
        @file_put_contents($logFile, '');
        exec('libretranslate --load-only en,' . escapeshellarg($lang) . ' > ' . escapeshellarg($logFile) . ' 2>&1 &');
        if (!waitForLt($LT_URL, $lang, $logFile, 180)) {
            out('ERROR: Language "' . $lang . '" still not available after restart.');
            exit;
        }
    }
}

out('[OK]   LibreTranslate ready. Language "' . $lang . '" available.');

// ----------------------------------------------------------------
// Collect all .md files to translate
// ----------------------------------------------------------------
function collectMd(string $dir, string $base): array {
    $files = [];
    $it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($dir));
    foreach ($it as $f) {
        if ($f->isFile() && $f->getExtension() === 'md') {
            $files[] = $f->getPathname();
        }
    }
    sort($files);
    return $files;
}

$files = collectMd($enDir, $enDir);
out('[INFO] ' . count($files) . ' files to translate.');

// ----------------------------------------------------------------
// Translate each file via LibreTranslate REST API
// ----------------------------------------------------------------
if (!is_dir($frDir)) mkdir($frDir, 0755, true);

/**
 * Send a single string to LibreTranslate, return translated text.
 */
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
            'timeout' => 30,
        ]
    ]);

    $resp = @file_get_contents($ltUrl . '/translate', false, $ctx);
    if ($resp === false) throw new RuntimeException('LibreTranslate request failed');

    $data = json_decode($resp, true);
    if (!isset($data['translatedText'])) throw new RuntimeException('Bad response: ' . $resp);
    return $data['translatedText'];
}

/**
 * Split content into segments: [text, isCode].
 * isCode=true -> do not translate.
 */
function splitSegments(string $content): array {
    $segments = [];
    // Match code fences and @verbatim blocks
    preg_match_all('/```[\s\S]*?```|@verbatim[\s\S]*?@endverbatim/m', $content, $matches, PREG_OFFSET_CAPTURE);
    $last = 0;
    foreach ($matches[0] as [$match, $offset]) {
        if ($offset > $last) $segments[] = [substr($content, $last, $offset - $last), false];
        $segments[] = [$match, true];
        $last = $offset + strlen($match);
    }
    if ($last < strlen($content)) $segments[] = [substr($content, $last), false];
    return $segments;
}

/**
 * Translate a prose block line by line, preserving Doxygen directives.
 */
function translateProse(string $text, string $lang, string $ltUrl): string {
    $lines = explode("\n", $text);
    $result = [];

    foreach ($lines as $line) {
        $stripped = trim($line);

        // Blank line
        if ($stripped === '') { $result[] = $line; continue; }

        // @page / @section / @subsection etc: preserve ID, translate title only
        if (preg_match('/^(\s*@(?:page|section|subsection|subsubsection|anchor)\s+\w+\s*)(.*)/u', $line, $m)) {
            $title = trim($m[2]);
            $result[] = $m[1] . ($title ? ltTranslate($title, $lang, $ltUrl) : '');
            continue;
        }

        // Pure directive line with no trailing prose (@code, @endcode, @ref xxx, etc.)
        if (preg_match('/^\s*@\w[\w_]*\s*$/', $line)) { $result[] = $line; continue; }

        // Directive followed by prose (@brief ..., @note ..., @param[in] name ...)
        if (preg_match('/^(\s*@\w[\w_]*\s+)(.*)/u', $line, $m)) {
            $result[] = $m[1] . ltTranslate(trim($m[2]), $lang, $ltUrl);
            continue;
        }

        // Markdown heading
        if (preg_match('/^(\s*#{1,6}\s+)(.*)/u', $line, $m)) {
            $result[] = $m[1] . ltTranslate(trim($m[2]), $lang, $ltUrl);
            continue;
        }

        // Table separator row
        if (preg_match('/^\s*\|[-| :]+\|\s*$/', $line)) { $result[] = $line; continue; }

        // Pure path / URL
        if (preg_match('/^\s*(https?:\/\/|\.\.\/?\/|\w+\/)/', $stripped) && strpos($stripped, ' ') === false) {
            $result[] = $line; continue;
        }

        // Default: translate whole line
        try {
            $result[] = ltTranslate($line, $lang, $ltUrl);
        } catch (Exception $e) {
            $result[] = $line; // keep original on error
        }
    }

    return implode("\n", $result);
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

    foreach ($segments as [$seg, $isCode]) {
        $output .= $isCode ? $seg : translateProse($seg, $lang, $LT_URL);
    }

    file_put_contents($dest, $output);
    out('[OK]   ' . $rel);
}

// ----------------------------------------------------------------
// Write a Doxyfile for the new language (copy from Doxyfile.fr)
// ----------------------------------------------------------------
out('[INFO] Writing Doxyfile.' . $lang . '...');
$doxyContent = file_get_contents($doxyfileSrc);
// Set output dir to html-{lang}
$doxyContent = str_replace('HTML_OUTPUT             = html', 'HTML_OUTPUT             = html-' . $lang, $doxyContent);
// Set output language
$doxyContent = str_replace('GENERATE_HTML           = YES', 'GENERATE_HTML           = YES' . "\nOUTPUT_LANGUAGE         = " . ucfirst($lang), $doxyContent);
// Replace EN input dirs with src-{lang}
$doxyContent = preg_replace(
    '/^INPUT\s*=.*?(\\\n.*?)*$/m',
    'INPUT                   = src-' . $lang,
    $doxyContent
);
// Remove HTML_HEADER and HTML_EXTRA_FILES (not needed for translated versions)
$doxyContent = preg_replace('/^HTML_HEADER.*$\n?/m', '', $doxyContent);
$doxyContent = preg_replace('/^HTML_EXTRA_FILES.*$\n?/m', '', $doxyContent);
// Point mainpage to translated version
$doxyContent = preg_replace('/^USE_MDFILE_AS_MAINPAGE.*$/m', 'USE_MDFILE_AS_MAINPAGE  = src-' . $lang . '/mainpage.md', $doxyContent);
file_put_contents($doxyfileTgt, $doxyContent);
out('[OK]   Doxyfile.' . $lang . ' written.');

// ----------------------------------------------------------------
// Run Doxygen
// ----------------------------------------------------------------
out('[INFO] Running Doxygen for language "' . $lang . '"...');
$cmd = 'cd ' . escapeshellarg($doxygenDir) . ' && doxygen ' . escapeshellarg('Doxyfile.' . $lang) . ' 2>&1';
$output = [];
exec($cmd, $output, $code);
foreach ($output as $line) {
    if (trim($line)) out('       ' . $line);
}
if ($code !== 0) {
    out('ERROR: Doxygen exited with code ' . $code);
    exit;
}
out('[OK]   Doxygen done. Output: html-' . $lang . '/');

// ----------------------------------------------------------------
// Done
// ----------------------------------------------------------------
out('[OK]   Translation complete. Reload the page to see the new language.');
