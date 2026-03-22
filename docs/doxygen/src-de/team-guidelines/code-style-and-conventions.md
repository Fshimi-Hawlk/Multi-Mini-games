@page code_style_and_conventions Code Style & Konventionen

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

Diese Seite listet die Kodierung Stil und Konventionen, die wir im Multi Mini-Games-Projekt verfolgen.
Es bedeutet, den Code über das Team konsistent zu halten, damit die Zweige miteinander verschmelzen und die Arbeit des anderen lesen bleibt einfach.

Letzte Prüfung gegen Projektstruktur: März 2026

## Namenskonventionen

Funktionen und Variablen -> camelCase beginnend mit Kleinbuchstaben.

Typdef-Typen sind PascalCase mit einem Suffix, das zeigt, was sie sind:

- Kein Suffix für Basis/Primitär-ähnliche Typen (auch kleine Bausteine) ->`f32Vector2`,`u8Vector2`in`baseTypes.h`
- Ja`_St`für regelmäßige anleitungen (einschließlich anbau-arrays) ->`PlayerArray_St`
- Ja`_Et`für enums ->`GameScene_Et`
- Ja`_Ut`für gewerkschaften
- Ja`_Ft`für funktionstypdefs
- Ja`_t`als rückfall, wenn nichts anderes passt

Wir haben kurze Alias in`baseTypes.h`für den normalen code (wenn die genaue größe nicht kritisch ist):

```c
typedef unsigned int   uint;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef float          f32;
typedef double         f64;
// plus vector shortcuts like iVector2, uVector2, etc.
```

Leitlinie:
Verwendung von Festbreitentypen (`u32`,`s32`, etc.) wenn größe/zeichen wichtig ist (networking, serialization, bit ops).
Verwenden Sie die kurzen (`uint`,`iVector2`, etc.) überall sonst für saubereres Gameplay/UI-Code.
Vermeiden Sie die Ebene`int`/`unsigned int`/`float`in neuem code, es sei denn, eine dritte partei lib zwingt es.

Parameterreihenfolge in Funktionen (Hilfe Lesbarkeit):

- Kontext/Handle zuerst (arena, Logger, Spielerstruktur, etc.)
- Input Params nächste
- Größe/Gegenstand direkt nach dem Pointer (Daten, Daten)
- Endpunkte der Ausgabe

## Static Keyword Verwendung

Verwendung`static`für alles, was privat zu einer .c-datei bleiben sollte:

- Ja`static`funktionen, wenn nicht Teil der öffentlichen API
- Ja`static`datei-skop-variablen / globals

Bevorzugen Sie Zustand als Params über Dateiglobals, um versteckte Abhängigkeiten zu vermeiden.

## Formatierung & Layout

Einbuchtung: 4 Räume (keine Tabs).

Klammern auf der gleichen Linie für Steuerbefehle, Funktionen, typedefs:

```c
if (condition) {
    // code
} else {
    // other
}

while (running) {
    // loop body
}

typedef struct {
    int x;
    int y;
} Point_St;

void someFunc(int a) {
    // body
}
```

Single-line Trivial-Funktionen sind ok, wenn kurz:

```c
static u32 hashU32(u32 v) { return v * 2654435761u; }
```

Spazierregeln:

- Raum nach Stichworten:`if (`,`while (`,`for (`
- Raum um binäre Operatoren:`a + b`,`x = y`
- Hinweise:`Type* ptr`(Punkt zu Typ),`Type *array`(Array vom Typ)
- const platzierung: lieber`const Type* ptr`wenn sich kein hinweis ändern sollte
- Kein nachlaufender Komma in Paramlisten,`void func()`für leere params

Linienlänge: weiche Grenze ~100 Zeichen, logisch brechen.

Deklarieren Sie Variablen in der Nähe der ersten Verwendung, oder Gruppe oben auf Block, wenn klarer.

Parenthesize Mixed-Operator-Ausdrücke, wenn vorangehen könnte verwirren:

```c
// good
if ((flags & MASK) == VALUE) { ... }

// bad (surprising precedence)
if (flags & MASK == VALUE) { ... }
```

## Macros & Preprozessor

Konstanten: SCREAMING CASE ->`#define MAX_PLAYERS 4`

Funktionsähnliche Makros: camelCase oder vorfixiert, immer parenthesize args + ganz expr:

```c
#define MUL(a, b) ((a) * (b))
#define do { ... } while(0)   // for multi-statement
```

## Datei und Header Organisation

Dateinamen: camelCase, bevorzugen einzelne Wörter (baseTypes.h, LobbyAPI. c)

Kopfschützen:`#ifndef PATH_LIKE_NAME_H`stil ->`#ifndef LOBBY_API_H`

Bestellen Sie in .c / .h:

1. Korrespondierende Header zuerst (in .c)
2. Andere Projektleiter
3. Utils/common headers
4. Standard/Drittanbieter nur, wenn es wirklich benötigt wird (bevorzugt über common.h)

## Kommentare & Doxygen Dokumentation

Jede .h und .c-Datei beginnt mit diesem Header-Block:

```
/**
    @file filename.c
    @author Your Name [or list multiple]
    @date YYYY-MM-DD          // creation date - don't change
    @date YYYY-MM-DD          // last meaningful change - update only for big stuff
    @brief One clear sentence what this file does.

    More context if needed (2-5 lines).

    @note Any warnings, ugly parts we know about, future plans

    Contributors:
        - Name: what you did (keep short)
        - ...
*/
```

Öffentliche Funktionen (in .h) erhalten vollen Doxygen Block:

```
/**
    @brief Short what the function does.

    Longer explanation if tricky (side effects, special cases).

    @param[in] player Current player state
    @param[out] outRect Filled with collision rect
    @return Pointer to texture or NULL if missing

    @pre player != NULL
    @note Assumes circle shape for collision
*/
Rectangle getPlayerCollisionBox(const Player_st* player);
```

Structs/enums in .h erhalten kurze + Feldkommentare:

```
/**
    @brief Player visual state, separate from physics on purpose.
*/
typedef struct {
    Rectangle defaultTextureRect;   ///< Default source rect
    bool      isTextureMenuOpen;    ///< Skin menu visible?
    Texture   textures[__playerTextureCount];   ///< Indexed by PlayerTextureId_Et
} PlayerVisuals_St;
```

Kurze Inline-Kommentare für offensichtliche Globale/Makros:

```c
extern Rectangle skinButtonRect;        ///< Skin menu toggle area
#define APP_TEXT_FONT_SIZE 32           ///< Base UI font size
```

## Fehlerbehandlung und Sicherheit

- Verwendung`UNREACHABLE`makro für unmögliche fälle
- Defensive null/bounds-Kontrollen
- Loggen Sie sich mit unserem Logger (log info, log warn, etc.)
- Ja`goto`ok für die reinigung in fehlerpfaden - kommentar hinzufügen warum
- Keine magischen Zahlen -> Verwendung`#define`oder enums

## Speicherverwaltung

Niedriger Zuordnungscode kann einfach`malloc`/`calloc`.
Prefer arena allocators (`globalArena`,`tempArena`,`context_alloc`) für die meisten dinge - einfachere lecks tracking.

**Erstellt:* 15. Januar 2025
**Letzte Aktualisierung:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"