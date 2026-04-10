# Documentation — Viewing and Translation

## Prerequisites

- **PHP** — `sudo apt install php` (Linux) or `brew install php` (Mac)
- **Doxygen** — `sudo apt install doxygen` (Linux) or `brew install doxygen` (Mac)
- **pipx** — `sudo apt install pipx && pipx ensurepath` (then reopen terminal)

---

## Generate Documentation

From the project root:

```bash
make docs
```

Generates the English HTML site in `docs/doxygen/html/`.

---

## View Documentation

### 1. Free the port if necessary

If port 8080 is already in use:

```bash
fuser -k 8080/tcp
```

### 2. Start the PHP server

```bash
cd docs/doxygen
php -S localhost:8080
```

### 3. Open in browser

```
http://localhost:8080
```

### 4. Close the server

`Ctrl+C` in the terminal.

---

## Translate Documentation

Translation uses [LibreTranslate](https://libretranslate.com) locally.
It is triggered from the **Translate** button on the homepage or from
the **Translate this documentation** button at the top of each Doxygen page.

### Installing LibreTranslate (once)

```bash
pipx install libretranslate
```

### Usage

The PHP server and LibreTranslate must run simultaneously in two separate terminals.

**Terminal 1 — PHP server:**

```bash
cd docs/doxygen
php -S localhost:8080
```

**Terminal 2 — LibreTranslate:**

```bash
libretranslate --load-only en,<language_code>
```

Replace `<language_code>` with the ISO code of the desired language (e.g., `fr`, `de`, `es`, `ja`).

Wait for terminal 2 to display:

```
Running on http://0.0.0.0:5000
```

Then open `http://localhost:8080`, select the language, and click **Translate**.

### Notes

- The first launch downloads models (~300 MB per language), which may take a few minutes.
- Subsequent launches are instant (cached models).
- Once translation is complete, reload the page to see the new language button.

---

## File Structure

```
docs/doxygen/
├-- Doxyfile              # Doxygen configuration
├-- header.html           # Custom header (Translate button)
├-- translate-bar.css     # Button styles in Doxygen pages
├-- index.php            # Homepage (language selector)
├-- index.css            # Homepage styles
├-- translate.php        # PHP backend — launches LibreTranslate and translates
├-- en/                  # Markdown sources for EN documentation
└-- html/                # Generated documentation (English)
```

The `html-<lang>/` folders are automatically created during translation.

---

**Last updated: March 2026**
