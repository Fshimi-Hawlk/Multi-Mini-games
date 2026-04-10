<?php
/**
 * lang-buttons.php
 * Retourne le HTML des boutons de langue disponibles (fragment, pas une page complète).
 *
 * Appelé par index.php via fetch() après une traduction pour mettre à jour
 * les boutons dynamiquement sans recharger toute la page.
 *
 * Retourne du HTML pur (text/html) — injecté directement dans #langButtons.
 */

header('Content-Type: text/html; charset=utf-8');

// Bouton anglais toujours présent
echo '<a class="btn-lang" href="html/index.html">English</a>' . "\n";

// Un bouton par langue traduite (dossiers html-XX/)
foreach (glob(__DIR__ . '/html-*', GLOB_ONLYDIR) as $dir) {
    $code  = substr(basename($dir), 5);
    $label = strtoupper($code);
    echo '<a class="btn-lang" href="html-' . htmlspecialchars($code) . '/index.html">'
       . htmlspecialchars($label) . '</a>' . "\n";
}
