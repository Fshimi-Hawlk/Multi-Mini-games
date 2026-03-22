@page sub_project_internal_makefile_structure Unterprojekt Interne Makefile Struktur

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

Diese Seite erklärt die **interne Struktur* des in jedem Teilprojekt verwendeten Makefile-Systems (Spiele, Lobby usw.).

Wir verwenden ein **modular Split-Makefile**-Design, um die Dinge aufrechtzuerhalten, lesbar und leichter zu erweitern (insbesondere für verschiedene Plattformen).

Alle realen Makefile Logik lebt in der`make/`ordner.
Die Wurzel`Makefile`in jedem teilprojekt wird sehr kurz gehalten — es enthält nur die modularen dateien in fester reihenfolge und erklärt`.PHONY`ziele.

## Verzeichnis und Verzeichnis

```text
sub-project-root/
├-- Makefile                  # very short: includes + .PHONY
├-- make/
│   ├-- 00-config.mk          # compiler, modes, flags, paths, OS detection
│   ├-- 10-sources.mk         # finds .c files in src/ and tests/, builds object lists
│   ├-- 20-build-rules.mk     # pattern rules (.o from .c, linking, static-lib)
│   ├-- 30-targets-main.mk    # main program targets (all, rebuild, run-main, etc.)
│   ├-- 40-targets-tests.mk   # test targets (tests, run-tests with logs)
│   ├-- 50-tools.mk           # verbosity control, clean, help, stdbuf handling
│   ├-- 99-overrides.mk       # optional local overrides (git-ignored)
│   └-- platform/             # OS-specific settings (included from 00-config.mk)
│       ├-- linux.mk
│       ├-- darwin.mk
│       └-- mingw.mk
├-- makefile.md               # user-facing guide (make help, modes, examples)
├-- src/
├-- tests/
├-- build/                    # generated files
└-- ...
```

## Inclusion Order (sehr wichtig)

Die root Makefile enthält diese Dateien **exakt in dieser Sequenz*:

ANHANG.`00-config.mk`- setzt Compiler, Fahnen, MODE, Verzeichnisse, erkennt OS, beinhaltet Plattform/*.mk
2.`10-sources.mk`- scannt src/ und testet/ für .c-dateien, erstellt objekt- & abhängigkeitslisten
3.`20-build-rules.mk`- definiert, wie man .o dateien kompiliert und executables/static libs verlinkt
4.`30-targets-main.mk`- benutzerziele für das hauptprogramm (alle, wieder aufbauen, run-main, run-gdb, statische-lib)
5.`40-targets-tests.mk`- testbezogene ziele (tests, run-tests mit zeitgestempelten protokollen)
6.`50-tools.mk`- helfer-stopfen: ruhe-/waffe-modus, sauber, hilfeleistung
7.`99-overrides.mk`- letzte (optionale) datei für lokale maschinentweaks, nie begangen

Frühe Dateien definieren meist Variablen.
Spätere Dateien verwenden diese Variablen, um tatsächliche Regeln und Ziele zu erstellen.

## Warum teilen und nummerieren?

- Bewertet Dateien in der Reihenfolge, in der sie enthalten sind — nummerierende Kräfte vorhersehbares Verhalten
- Klare Trennung der Bedenken erleichtert das Finden/Bearbeiten eines Teils, ohne andere zu brechen
- 00 = einrichtung und umgebung
- 10 = entdeckung der quelle
- 20 = core build mechanik
- 30/40 = was die menschen tatsächlich laufen (make run-main, machen tests, etc.)
- 50 = entwickler
- 99 = persönliche überschreitungen ohne verschmutzung
- Plattformspezifisches Material bleibt isoliert in`platform/`so können wir später Windows/macOS/Linux-Unterschiede sauber hinzufügen

## Schnelle Referenztabelle

| Datei | Hauptauftrag | Wichtige Dinge, die es definiert / erstellt |
|-----------------------|--------------------------------------------------------|----------------------------------------------------|
| 00-config.mk | Compiler, Flaggen, Modi, Pfade, OS-Erkennung | CC, CFLAGS, LDFLAGS, MODE, OBJ DIR, BIN, Plattform umfassen |
| 10-sources.mk | Scans nach Quelldateien, erstellt Listen | LIB SOURCES, LIB OBJECTS, TEST BINS, DEPS |
| 20-build-rules.mk | Zusammenstellung & Verknüpfungsregeln | %.o Muster, $(BIN), $(STATIC LIB), Test Verknüpfung |
| 30-targets-main.mk | Hauptprogrammziele | all, rebuild, run-main, run-gdb, statische-lib |
| 40-Ziele-tests.mk | Test-Gebäude & Laufen mit Protokollen | Tests, Run-Tests |
| 50-tools.mk | Verbose Kontrolle, sauber, Hilfe, Stdbuf-Wrapper | SILENT PREFIX, sauber, Hilfe |
| 99-overrides.mk | Lokale Maschine tweaks (git ignoriert) | jede Überschreitung, die Sie brauchen |
| Plattform/*.mk | OS-spezifische Fahnen & libs | BASE CFLAGS, BASE LDFLAGS, Raylib Pfade, etc

## Weitere Seiten

- @ref sub_project_makefile "User-facing Makefile guide" — wie man tatsächlich die Ziele und Modi verwendet

**Created:* 23. Februar 2026
**Letzte Aktualisierung:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"