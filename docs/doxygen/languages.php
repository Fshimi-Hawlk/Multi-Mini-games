<?php
/**
 * languages.php
 * Returns the list of available languages for translation.
 *
 * Always returns the complete static list of languages supported by LT —
 * regardless of what is loaded in memory (--load-only).
 * The status indicates whether LT is currently reachable or not.
 *
 * Returns JSON:
 *   { "status": "ok",      "langs": [...] }   LT reachable
 *   { "status": "static",  "langs": [...] }   LT offline (script will start it automatically)
 */

header('Content-Type: application/json; charset=utf-8');

// Complete static list of languages supported by LibreTranslate (v1.6+).
// We use this list instead of LT /languages because with --load-only,
// LT only returns languages loaded in memory, not all installed languages.
$staticLangs = [
    ['code' => 'ar', 'name' => 'Arabic'],
    ['code' => 'az', 'name' => 'Azerbaijani'],
    ['code' => 'zh', 'name' => 'Chinese'],
    ['code' => 'cs', 'name' => 'Czech'],
    ['code' => 'da', 'name' => 'Danish'],
    ['code' => 'nl', 'name' => 'Dutch'],
    ['code' => 'eo', 'name' => 'Esperanto'],
    ['code' => 'fi', 'name' => 'Finnish'],
    ['code' => 'fr', 'name' => 'French'],
    ['code' => 'de', 'name' => 'German'],
    ['code' => 'el', 'name' => 'Greek'],
    ['code' => 'he', 'name' => 'Hebrew'],
    ['code' => 'hi', 'name' => 'Hindi'],
    ['code' => 'hu', 'name' => 'Hungarian'],
    ['code' => 'id', 'name' => 'Indonesian'],
    ['code' => 'ga', 'name' => 'Irish'],
    ['code' => 'it', 'name' => 'Italian'],
    ['code' => 'ja', 'name' => 'Japanese'],
    ['code' => 'ko', 'name' => 'Korean'],
    ['code' => 'fa', 'name' => 'Persian'],
    ['code' => 'pl', 'name' => 'Polish'],
    ['code' => 'pt', 'name' => 'Portuguese'],
    ['code' => 'ru', 'name' => 'Russian'],
    ['code' => 'sk', 'name' => 'Slovak'],
    ['code' => 'es', 'name' => 'Spanish'],
    ['code' => 'sv', 'name' => 'Swedish'],
    ['code' => 'tl', 'name' => 'Tagalog'],
    ['code' => 'tr', 'name' => 'Turkish'],
    ['code' => 'uk', 'name' => 'Ukrainian'],
];

// Check if LT is reachable to indicate its status in the response.
// We do NOT use its list — it is incomplete with --load-only.
$LT_URL = 'http://localhost:5000';
$ctx    = stream_context_create(['http' => ['timeout' => 3]]);
$raw    = @file_get_contents($LT_URL . '/languages', false, $ctx);
$status = ($raw !== false) ? 'ok' : 'static';

echo json_encode(['status' => $status, 'langs' => $staticLangs]);
