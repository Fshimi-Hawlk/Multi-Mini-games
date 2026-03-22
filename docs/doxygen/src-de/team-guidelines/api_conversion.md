@page api_conversion API Umwandlung und Integration

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

**Ein eigenständiges Minispiel in eine Lobby-kompatible API* konvertieren

Diese Seite erklärt die Schritte, um ein unabhängiges Mini-Spiel (zum Beispiel die`tetris/`ordner) und schalten Sie es in etwas, das die Lobby laden und laufen direkt im selben Fenster. Keine neuen Prozesse, keine zusätzlichen Fenster – nur saubere Szenenwechsel.

Der Ansatz basiert auf der Integration von Tetris und folgt den Mustern in`generalAPI.h`, das Beispiel API-Dateien, und`lobby/src/main.c`.

**Kernprinzipien folgen wir**
- Verwenden Sie opake structs, so dass die Lobby nie Zugriff auf Spiel-Innerheiten
- Geben Sie genau drei Hauptfunktionen: init, Schleife, kostenlos
- Rückgabefehler mit Hilfe der geteilten`Error_Et`enum
- Behandeln Sie jedes Mini-Spiel als temporäre "Szene" kann die Lobby starten und stoppen

## Voraussetzungen

Vor Beginn der Konvertierung stellen Sie sicher:
- Ihr Spielordner entspricht dem`sub-project-example/`layout (`src/`,`include/`,`tests/`,`Makefile`)
- Standalone-Modus funktioniert:`make rebuild run-main`läuft das spiel ohne fehler
- Raylib ist korrekt über die Makefile verknüpft

Wenn eine davon fehlt, kopieren Sie den Template-Ordner und erhalten Standalone zuerst laufen.

## Schritt 1 – Erstellen Sie die öffentliche API-Header

Datei:`include/<gameName>API.h`(beispiel:`tetrisAPI.h`)

Dieser Header ist das Einzige, was die Lobby sehen wird. Halten Sie es minimal: opaker Typ, Funktionserklärungen, optionale config struct.

Typischer Inhalt:

```c
#ifndef TETRIS_API_H
#define TETRIS_API_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

typedef struct TetrisGame_St TetrisGame_St;

typedef struct {
    int fps;            // target FPS, 0 = use default
    // add other options later if needed
} TetrisConfigs_St;

Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs);

#define tetris_initGame(game, ...) \
    tetris_initGame__full(game, (TetrisConfigs_St){ __VA_ARGS__ })

void tetris_gameLoop(TetrisGame_St* const game);

void tetris_freeGame(TetrisGame_St** game);

#endif
```

**Importierte Anmerkung*
In der Implementierung (.c-Datei) ist das erste Mitglied Ihrer Anleitung **must**`Game_St base;`so kann die lobby sicher auf die geteilte`Game_St*`typ.

Sehen Sie die richtigen Dateien in`firstparty/APIs/`oder`tetris/include/tetrisAPI.h`für referenz.

## Schritt 2 – Implementierung der API-Funktionen

Datei:`src/<gameName>API.c`

Hier definieren Sie die komplette Anleitung und schreiben Sie die drei Funktionen.

Grundgerüst:

```c
struct TetrisGame_St {
    Game_St base;       // MUST be first member
    // your game-specific fields
    // Board board;
    // int score;
    // ...
};

Error_Et tetris_initGame__full(TetrisGame_St** game_out, TetrisConfigs_St configs) {
    TetrisGame_St* game = calloc(1, sizeof(TetrisGame_St));
    if (!game) return ERROR_ALLOC;

    game->base.running = true;
    // initialize your data, load assets, etc.
    // if (configs.fps > 0) SetTargetFPS(configs.fps);

    *game_out = game;
    return OK;
}

void tetris_gameLoop(TetrisGame_St* const game) {
    if (!game->base.running) return;

    // handle input
    // update logic
    BeginDrawing();
        // render everything
    EndDrawing();

    // set running = false when game should end
}

void tetris_freeGame(TetrisGame_St** game) {
    if (!game || !*game) return;
    // free textures, unload sounds, release memory
    free(*game);
    *game = NULL;
}
```

Niemals anrufen`InitWindow`,`CloseWindow`oder das fenster innerhalb dieser funktionen verwalten — die lobby hat das bereits getan.

## Schritt 3 – Refactor your game logic

Verschieben Sie Ihren Spielcode in die drei API-Funktionen:
- Initialisierung geht ein`init`
- Per-frame-Arbeit geht in`loop`
- Sauberkeit geht rein`free`

Entfernen Sie alle Fenster Erstellung / schließen Anrufe von Ihrem ursprünglichen Code.

## Schritt 4 – Integrieren in die Lobby

In`lobby/src/main.c`:
- Hinzufügen`#include "APIs/<gameName>API.h"`
- Fügen Sie Ihr Spiel zum`GameScene_Et`enum
- Wenn der Spieler mit der Auslösezone kollidiert:
- Hol den Zeiger:`Game_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_YOURGAME];`
- Einmal gegossen:`<GameName>Game_St** ref = (<GameName>Game_St**) miniRef;`
- Wenn das erste Mal: rufen Sie an und überprüfen Sie Fehler
- Rufen Sie jeden Rahmen an
- Wann`!(*miniRef)->running`: rufen sie frei und wechseln sie zurück in die lobby-szene

Wir verwenden`miniRef`zu vermeiden gießen jedes mal, wenn wir überprüfen`running`.

Sehen Sie, wie es für Tetris getan ist in`lobby/src/main.c`.

## Schritt 5 – Bauen und überprüfen

Aus der Repository Wurzel:

```bash
# Normal full rebuild + run
make MODE=strict-debug rebuild run-exe

# With sanitizers (good for catching bugs)
make MODE=clang-debug rebuild run-exe

# Or valgrind if you don't have clang
make MODE=valgrind-debug rebuild run-exe
```

Von innen in Ihrem Spielordner (für Standalone-Checks):

```bash
make MODE=strict-debug rebuild run-main
```

Beobachten Sie die Konsolenprotokolle, wenn etwas ausfällt.

## Gemeinsame Probleme, die wir in

- Ja`Game_St base`nicht erst in struct -> casting fehlschlägt
- Spielanrufe`InitWindow`/`CloseWindow`-> doppel-init crash
- Texturen/Sounds nicht freigegeben -> Speicherlecks
- Keine vorzeitige Rückkehr in der Schleife, wenn`!running`-> spiel läuft nach beendet
- Instant Re-Trigger beim Ausstieg -> Kühlen auf hitbox hinzufügen
- Vergessen, API-Header zu kopieren -> Lobby gehören Fehler

Wenn etwas bricht, überprüfen @ref sub_project_makefile "Sub Project Makefile" Seite, vergleichen Sie mit`tetrisAPI.c`, oder fragen sie im gruppenchat.

**Created:* 10. Februar 2026
**Letzte Aktualisierung:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"