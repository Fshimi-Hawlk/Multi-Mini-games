@page sub_project_makefile Unterprojekt Makefile Dokumentation

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

Diese Seite beschreibt das Makefile, das in jedem Teilprojektordner verwendet wird (tetris/, Lobby/, neue Spiele, die von Subprojekt-Beispiel/, etc. kopiert werden).

Die Makefile übernimmt den Aufbau des Spiels als eigenständiges, ausführbares Testen, die Erstellung einer statischen Bibliothek für die Integration der Lobby, die Durchführung von Unit-Tests und die Unterstützung mehrerer Debug/Optimierung-Modi.

## Überblick

Hauptverantwortlichkeiten:
- Bauen Sie Standalone aus`src/main.c`(für schnelle tests)
- statische Bibliothek bauen`lib<gameName>.a`(von root Makefile verwendet, um in die Lobby zu verlinken)
- Bauen und Ausführen von Einheiten`tests/`
- Unterstützung mehrerer Build-Modi (Release / Debug / Sanitizer / Valgrind)
- Automatisches Kopfabhängigkeitstracking (-MMD -MP)
- Ruhe standardmäßig, Verbose mit VERBOSE=1
- Benutzerdefinierte Flaggen über EXTRA CFLAGS / EXTRA LDFLAGS
- Log-Test und Valgrind-Ausgang in zeitgenähten Ordnern unter`logs/`

Typische Ausgabepfade (bezogen auf Teilprojektordner):
- Objekte:`build/obj/`
- Standalone binär:`build/bin/main`
- Static lib:`build/lib/lib<gameName>.a`
- Test binäre:`build/bin/tests/<test-name>`

## Unterstützte Build-Modi

| Modus | Compiler | Hauptfahnen | Anforderungen | Typischer Anwendungsfall |
|-----------------|----------|------------------------------------------|--------------------|----------------------------------------|
| release | gcc | -O2 | - | Schluss / optimierte Standalone-Läufe |
| debug | gcc | -Wall -Wextra -g -O0 | - Normale Entwicklung |
| streng-debug | gcc | -Werror -Wall -Wextra -pedantic -g -O0 | - | Fangwarnungen vor der Zusammenführung |
| clang-debug | clang | same + -fsanitize=address,undefiniert | clang installiert | Memory / UB Bug Jagd |
| valgrind-debug | -Werror -Wall -Wextra -pedantic -g -O0 | valgrind installiert | Laufzeitleck / ungültige Leseprüfungen |

Standardmodus ist`release`.

## Nützliche Ziele

- Ja`all`/ standardziel : aufbau von standalone ausführbar
- Ja`static-lib`: erstellung einer statischen bibliothek für die integration der lobby
- Ja`tests`: alle test-binaries bauen
- Ja`run-main`: bauen + laufen standalone ausführbar
- Ja`run-tests`: alle tests erstellen + ausführen (mit live-ausgang + protokolle)
- Ja`rebuild`: sauber + alle
- Ja`clean`: build/ ordner entfernen
- Ja`run-gdb`: laufen standalone binär unter gdb

## Schnelle Beispiele

Basic Standalone build + run:

```bash
make
make run-main
```

Debug / strenger Modus:

```bash
make MODE=debug
make MODE=strict-debug rebuild run-main
```

Mit Sanitizern oder Valgrind:

```bash
make MODE=clang-debug run-main
make MODE=valgrind-debug run-tests
```

Bauen Sie nur die statische lib (für Lobby):

```bash
make static-lib
```

Verbose + zusätzliche Warnungen:

```bash
make VERBOSE=1 EXTRA_CFLAGS="-Wshadow -Wconversion" MODE=strict-debug
```

## Logging & Ausgabehinweise

- Ja`run-tests`erstellt`logs/tests-<timestamp>/`mit einer .log-datei pro test (stdout + stderr)
- In`valgrind-debug`modus: extra`logs/valgrind-<timestamp>/`ordner mit Valgrind-Berichten
- Logs nur nicht leere Ausgabe speichern
- Crash-Ausgangssicherheit: Verwendung`stdbuf --output=L --error=L`(Linienpufferung) wenn verfügbar, so dass die letzten Printf()-Zeilen auch auf SIGABRT oder ASan Crash erscheinen
- Wenn`stdbuf`fehlt -> warnung einmal gedruckt, ausgabe könnte auf crash ablaufen -> add explizit`fflush(stdout); fflush(stderr);`in prüfcode, falls erforderlich

## Anforderungen und Tragfähigkeit

- Ja`clang-debug`bedarf clang
- Ja`valgrind-debug`braucht valgrind
- Ja`stdbuf`(von coreutils) ist optional, aber für saubere Testausgabe auf Linux empfohlen

## Verwandte Seiten

@ref sub_project_internal_makefile_structure "Makefile Internal Structure": Wie die Makefile ist **intern strukturiert*

**Erstellt:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"