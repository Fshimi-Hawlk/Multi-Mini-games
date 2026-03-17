# Documentation — Consultation et traduction

## Prérequis

- **PHP** — `sudo apt install php` (Linux) ou `brew install php` (Mac)
- **Doxygen** — `sudo apt install doxygen` (Linux) ou `brew install doxygen` (Mac)
- **pipx** — `sudo apt install pipx && pipx ensurepath` (puis rouvrir le terminal)

---

## Générer la documentation

Depuis la racine du projet :

```bash
make docs
```

Génère le site HTML anglais dans `docs/doxygen/html/`.

---

## Consulter la documentation

### 1. Libérer le port si nécessaire

Si le port 8080 est déjà utilisé :

```bash
fuser -k 8080/tcp
```

### 2. Lancer le serveur PHP

```bash
cd docs/doxygen
php -S localhost:8080
```

### 3. Ouvrir dans le navigateur

```
http://localhost:8080
```

### 4. Fermer le serveur

`Ctrl+C` dans le terminal.

---

## Traduire la documentation

La traduction utilise [LibreTranslate](https://libretranslate.com) en local.
Elle se déclenche depuis le bouton **Translate** de la page d'accueil ou depuis
le bouton **Translate this documentation** en haut de chaque page Doxygen.

### Installation de LibreTranslate (une seule fois)

```bash
pipx install libretranslate
```

### Utilisation

Le serveur PHP et LibreTranslate doivent tourner en même temps dans deux terminaux séparés.

**Terminal 1 — serveur PHP :**

```bash
cd docs/doxygen
php -S localhost:8080
```

**Terminal 2 — LibreTranslate :**

```bash
libretranslate --load-only en,<code_langue>
```

Remplace `<code_langue>` par le code ISO de la langue souhaitée (ex: `fr`, `de`, `es`, `ja`).

Attendre que le terminal 2 affiche :

```
Running on http://0.0.0.0:5000
```

Puis ouvrir `http://localhost:8080`, sélectionner la langue et cliquer **Translate**.

### Notes

- Le premier lancement télécharge les modèles (~300 Mo par langue), ce qui peut prendre quelques minutes.
- Les téléchargements suivants sont instantanés (modèles mis en cache).
- Une fois la traduction terminée, recharger la page pour voir le nouveau bouton de langue.

---

## Structure des fichiers

```
docs/doxygen/
├── Doxyfile              # Configuration Doxygen
├── header.html           # Header custom (bouton Translate)
├── translate-bar.css     # Styles du bouton dans les pages Doxygen
├── index.php             # Page d'accueil (sélecteur de langue)
├── index.css             # Styles de la page d'accueil
├── translate.php         # Backend PHP — lance LibreTranslate et traduit
├── en/                   # Sources Markdown de la doc EN
└── html/                 # Doc générée (anglais)
```

Les dossiers `html-<lang>/` sont créés automatiquement lors d'une traduction.

---

**Dernière mise à jour : Mars 2026**
