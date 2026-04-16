<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Multi Mini-Games - Documentation</title>
    <link rel="stylesheet" href="index.css">
</head>
<body>
<div class="card">
    <h1>Multi Mini-Games</h1>
    <p class="subtitle">Documentation</p>

    <!-- Boutons des langues disponibles -->
    <!-- FIX: générés côté PHP (serveur) — visibles immédiatement sans JS.
         refreshLangButtons() les met aussi à jour après une traduction
         sans forcer un rechargement complet de la page. -->
    <p class="section-title">Available languages</p>
    <div class="lang-buttons" id="langButtons">
        <a class="btn-lang" href="html/index.html">English</a>
        <?php
        foreach (glob(__DIR__ . '/html-*', GLOB_ONLYDIR) as $dir) {
            $code  = substr(basename($dir), 5);
            $label = strtoupper($code);
            echo '<a class="btn-lang" href="html-' . htmlspecialchars($code) . '/index.html">'
               . htmlspecialchars($label) . '</a>' . "\n";
        }
        ?>
    </div>

    <hr class="divider">

    <!-- Section traduction -->
    <div class="translate-section" id="translateSection">
        <p class="section-title">Generate a new translation</p>

        <div class="form-row">
            <select id="langSelect">
                <option value="">Loading languages...</option>
            </select>
            <button id="translateBtn" disabled>Translate</button>
        </div>

        <p class="lt-hint" id="ltHint">
            LibreTranslate starts automatically on first use.<br>
            Models are downloaded on first translation (~300 Mo per language).
        </p>

        <div id="log"></div>
    </div>
</div>

<script>
// ----------------------------------------------------------------
// FIX: la liste des langues passe maintenant par languages.php (proxy
// côté serveur PHP). L'ancienne version faisait un fetch direct vers
// localhost:5000 depuis le navigateur — ce qui échouait si LT tournait
// sur le serveur mais pas sur la machine du visiteur.
// ----------------------------------------------------------------
async function loadLanguages() {
    const sel  = document.getElementById('langSelect');
    const btn  = document.getElementById('translateBtn');
    const hint = document.getElementById('ltHint');

    try {
        const res  = await fetch('languages.php');
        const data = await res.json();

        if (data.langs.length === 0) {
            sel.innerHTML = '<option value="">-- No languages available --</option>';
            btn.disabled  = true;
            hint.innerHTML = 'Could not load language list.';
            return;
        }

        sel.innerHTML = '<option value="">-- Select a language --</option>';
        data.langs.forEach(l => {
            const opt       = document.createElement('option');
            opt.value       = l.code;
            opt.textContent = l.name + ' (' + l.code + ')';
            sel.appendChild(opt);
        });
        btn.disabled = false;

        // Indiquer si LT tourne déjà ou non
        if (data.status === 'static') {
            hint.innerHTML =
                'LibreTranslate is not running — it will start automatically on first use.<br>' +
                'First translation may take a few minutes (model download ~300 Mo per language).';
        } else {
            hint.innerHTML =
                'LibreTranslate is running. Select a language and click Translate.';
        }

    } catch (e) {
        sel.innerHTML    = '<option value="">-- Error loading languages --</option>';
        btn.disabled     = true;
        hint.textContent = 'Could not reach languages.php: ' + e.message;
    }
}

// ----------------------------------------------------------------
// FIX: recharge les boutons de langue dynamiquement après une traduction
// sans recharger toute la page, via lang-buttons.php.
// ----------------------------------------------------------------
async function refreshLangButtons() {
    try {
        const res  = await fetch('lang-buttons.php');
        const html = await res.text();
        document.getElementById('langButtons').innerHTML = html;
    } catch (e) {
        // Non bloquant — les boutons seront corrects au prochain rechargement
    }
}

function appendLog(msg, type = 'info') {
    const log        = document.getElementById('log');
    log.style.display = 'block';
    const span       = document.createElement('span');
    span.className   = 'status-' + type;
    span.textContent = msg + '\n';
    log.appendChild(span);
    log.scrollTop    = log.scrollHeight;
}

document.getElementById('translateBtn').addEventListener('click', async () => {
    const code = document.getElementById('langSelect').value;
    if (!code) return;

    const btn        = document.getElementById('translateBtn');
    btn.disabled     = true;
    btn.textContent  = 'Running...';

    const log        = document.getElementById('log');
    log.innerHTML    = '';
    log.style.display = 'block';
    appendLog('Starting translation to ' + code + '...', 'info');

    try {
        const res    = await fetch('translate.php?lang=' + encodeURIComponent(code));
        const reader = res.body.getReader();
        const dec    = new TextDecoder();

        while (true) {
            const { done, value } = await reader.read();
            if (done) break;
            dec.decode(value, { stream: true }).split('\n').forEach(line => {
                if (!line.trim()) return;
                const type = line.startsWith('ERROR') || line.startsWith('[FAIL]') ? 'error'
                           : line.startsWith('[OK]')  ? 'ok'
                           : line.startsWith('[INFO]') && line.includes('make docs-translate') ? 'warn'
                           : 'info';
                appendLog(line, type);
            });
        }

        appendLog('Done!', 'ok');

        // Mettre à jour les boutons sans recharger la page
        await refreshLangButtons();

    } catch (e) {
        appendLog('ERROR: ' + e.message, 'error');
    }

    btn.disabled    = false;
    btn.textContent = 'Translate';
});

loadLanguages();
</script>
</body>
</html>
