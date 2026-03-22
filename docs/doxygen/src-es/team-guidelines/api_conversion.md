@page api_conversion API Conversión & Integración

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

**Convertir un minijuego independiente en una API compatible con el lobby**

Esta página explica los pasos para tomar un mini-juego independiente (por ejemplo, el`tetris/`carpeta) y convertirlo en algo que el vestíbulo puede cargar y ejecutar directamente dentro de la misma ventana. No desperdiciar nuevos procesos, no hay ventanas adicionales — sólo cambiar escena limpia.

El enfoque se basa en cómo integramos Tetris, y sigue los patrones en`generalAPI.h`, el ejemplo de archivos API, y`lobby/src/main.c`.

**Principios básicos que seguimos**
- Utilice las estructuras opacas para que el vestíbulo nunca acceda a los internos del juego
- Proporcionar exactamente tres funciones principales: init, loop, free
- Errores de retorno usando el compartido`Error_Et`enum
- Tratar cada mini-juego como una "escena" temporal el vestíbulo puede empezar y parar

## Prerrequisitos

Antes de comenzar la conversión asegúrese:
- Tu carpeta de juego coincide con la`sub-project-example/`diseño`src/`,`include/`,`tests/`,`Makefile`)
- El modo independiente funciona:`make rebuild run-main`ejecuta el juego sin errores
- Raylib está conectado correctamente a través del Makefile

Si alguno de estos faltan, copie la carpeta de la plantilla y haga funcionar independiente primero.

## Paso 1 – Crear el encabezado público de API

Archivo:`include/<gameName>API.h`(ejemplo:`tetrisAPI.h`)

Este encabezado es lo único que verá el vestíbulo. Mantenlo mínimo: tipo opaco, declaraciones de función, estructura de configuración opcional.

Contenido típico:

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

**Nota importante**
In the implementation (.c file), the first member of your struct **must** be`Game_St base;`para que el vestíbulo pueda lanzarse con seguridad al`Game_St*`tipo.

Vea los archivos reales en`firstparty/APIs/`o`tetris/include/tetrisAPI.h`para referencia.

## Paso 2 – Implementar las funciones de API

Archivo:`src/<gameName>API.c`

Aquí usted define la estructura completa y escribe las tres funciones.

Esqueleto básico:

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

Nunca llames`InitWindow`,`CloseWindow`o gestionar la ventana dentro de estas funciones — el vestíbulo ya hizo eso.

## Paso 3 – Refactora tu lógica de juego

Mueva todo el código de juego en las tres funciones de API:
- La inicialización entra`init`
- El trabajo por marco entra`loop`
- La limpieza entra`free`

Eliminar cualquier ventana de creación / cierre de llamadas de su código original.

## Paso 4 – Integrar en el vestíbulo

In`lobby/src/main.c`:
- Añadir`#include "APIs/<gameName>API.h"`
- Agregue su juego al`GameScene_Et`enum
- Cuando el jugador choca con la zona de activación:
- Toma el puntero:`Game_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_YOURGAME];`
- Una vez`<GameName>Game_St** ref = (<GameName>Game_St**) miniRef;`
- Si es la primera vez: llame y compruebe los errores
- Llama al bucle cada marco
- Cuando`!(*miniRef)->running`: llamar gratis y volver a la escena del vestíbulo

Usamos`miniRef`para evitar el casting cada vez que comprobamos`running`.

Mira cómo se hace para Tetris en`lobby/src/main.c`.

## Paso 5 – Construir y verificar

Desde la raíz del repositorio:

```bash
# Normal full rebuild + run
make MODE=strict-debug rebuild run-exe

# With sanitizers (good for catching bugs)
make MODE=clang-debug rebuild run-exe

# Or valgrind if you don't have clang
make MODE=valgrind-debug rebuild run-exe
```

Desde el interior de la carpeta de juego (para cheques independientes):

```bash
make MODE=strict-debug rebuild run-main
```

Mira los registros de la consola si algo falla.

## Problemas comunes en los que nos encontramos

-`Game_St base`no primero en struct - título falla silenciosamente
- Llamadas de juego`InitWindow`/`CloseWindow`- estrecho de doble arranque
- Texturas/sonidos no liberados - filtraciones de memoria del título
- No regreso temprano en el bucle cuando`!running`- juego de títulos sigue corriendo después de renunciar
- Retrigger instantáneo al salir - agregar refresco en caja de seguridad
- Olvídate de copiar el encabezado de API - cabildeo incluido fallas

Si algo se rompe, compruebe la página "TOK0"`tetrisAPI.c`, o preguntar en el chat grupal.

**Creado:** 10 de febrero de 2026
** Última actualización:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"