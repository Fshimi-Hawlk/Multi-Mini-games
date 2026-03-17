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

    <!-- Existing language versions -->
    <p class="section-title">Available languages</p>
    <div class="lang-buttons" id="langButtons">
        <a class="btn-lang" href="html/index.html">English</a>
        <?php
        foreach (glob(__DIR__ . '/html-*', GLOB_ONLYDIR) as $dir) {
            $code  = substr(basename($dir), 5);
            $label = strtoupper($code);
            echo '<a class="btn-lang" href="html-' . htmlspecialchars($code) . '/index.html">' . htmlspecialchars($label) . '</a>' . "
";
        }
        ?>
    </div>


    <hr class="divider">

    <!-- Translation section -->
    <div class="translate-section" id="translateSection">
        <p class="section-title">Generate a new translation</p>

        <div class="form-row">
            <select id="langSelect">
                <option value="">Loading languages...</option>
            </select>
            <button id="translateBtn" disabled>Translate</button>
        </div>

        <p class="lt-hint">
            LibreTranslate starts automatically on first use.<br>
            Models are downloaded on first translation (~300 Mo per language).
        </p>

        <div id="ltStatus" style="display:none; margin-top:0.75rem; font-size:0.8rem; color:#8b949e; text-align:center;"></div>
        <div id="log"></div>
    </div>
</div>

<script>
const LT_API = 'http://localhost:5000';

// Fetch available languages from local LibreTranslate
async function loadLanguages() {
    const sel      = document.getElementById('langSelect');
    const btn      = document.getElementById('translateBtn');
    const section  = document.getElementById('translateSection');
    const ltStatus = document.getElementById('ltStatus');

    try {
        const res = await fetch(LT_API + '/languages', { signal: AbortSignal.timeout(3000) });
        if (!res.ok) throw new Error('HTTP ' + res.status);
        const langs = await res.json();

        sel.innerHTML = '<option value="">-- Select a language --</option>';
        langs.forEach(l => {
            if (l.code === 'en') return;
            const opt = document.createElement('option');
            opt.value = l.code;
            opt.textContent = l.name + ' (' + l.code + ')';
            sel.appendChild(opt);
        });
        btn.disabled = false;
        ltStatus.style.display = 'none';
    } catch (e) {
        // LibreTranslate not available — hide the form, show a soft notice
        sel.style.display = 'none';
        btn.style.display = 'none';
        ltStatus.textContent = 'LibreTranslate is not installed or not running. Install it with: pip install libretranslate';
        ltStatus.style.display = 'block';
    }
}

function appendLog(msg, type = 'info') {
    const log = document.getElementById('log');
    log.style.display = 'block';
    const span = document.createElement('span');
    span.className = 'status-' + type;
    span.textContent = msg + '\n';
    log.appendChild(span);
    log.scrollTop = log.scrollHeight;
}

document.getElementById('translateBtn').addEventListener('click', async () => {
    const code = document.getElementById('langSelect').value;
    if (!code) return;

    const btn = document.getElementById('translateBtn');
    btn.disabled = true;
    btn.textContent = 'Running...';

    const log = document.getElementById('log');
    log.innerHTML = '';
    appendLog('Starting translation to ' + code + '...', 'info');

    try {
        const res = await fetch('translate.php?lang=' + encodeURIComponent(code));
        const reader = res.body.getReader();
        const decoder = new TextDecoder();

        while (true) {
            const { done, value } = await reader.read();
            if (done) break;
            const text = decoder.decode(value, { stream: true });
            text.split('\n').forEach(line => {
                if (!line.trim()) return;
                const type = line.startsWith('ERROR') || line.startsWith('[FAIL]') ? 'error'
                           : line.startsWith('[OK]') ? 'ok'
                           : 'info';
                appendLog(line, type);
            });
        }

        appendLog('Done. Reload the page to see the new language button.', 'ok');

    } catch (e) {
        appendLog('ERROR: ' + e.message, 'error');
    }

    btn.disabled = false;
    btn.textContent = 'Translate';
});

loadLanguages();
</script>
</body>
</html>
