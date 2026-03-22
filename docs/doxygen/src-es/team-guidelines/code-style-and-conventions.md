@page code_style_and_conventions Code Style " Conventions

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

Esta página enumera el estilo de codificación y las convenciones que seguimos en el proyecto Multi Mini-Games.
Está destinado a mantener el código consistente en todo el equipo, así que fusionar ramas y leer el trabajo del otro se mantiene fácil.

Última comprobación contra la estructura del proyecto: marzo 2026

## Naming Conventions

Funciones y variables - título camelCase empezando con letra minúscula.

Tipos de tipo son PascalCase con un sufijo que muestra lo que son:

- No sufijo para tipos de base/primitivos (incluso pequeñas estructuras) - Confía`f32Vector2`,`u8Vector2`dentro`baseTypes.h`
-`_St`para estructuras regulares (incluyendo arrays cultivables) -`PlayerArray_St`
-`_Et`para enums - título`GameScene_Et`
-`_Ut`sindicales
-`_Ft`para tipodefs de función
-`_t`como retroceso si nada más encaja

Tenemos apodos cortos en`baseTypes.h`para el código normal (cuando el tamaño exacto no es crítico):

```c
typedef unsigned int   uint;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef float          f32;
typedef double         f64;
// plus vector shortcuts like iVector2, uVector2, etc.
```

Directriz:
Use tipos de ancho fijo (`u32`,`s32`, etc.) cuando el tamaño/signedness importa (networking, serialization, bit ops).
Use los cortos (`uint`,`iVector2`, etc.) en todas partes para el juego limpio / Código de usuario.
Evite la llanura`int`/`unsigned int`/`float`en nuevo código a menos que una lib de terceros lo obligue.

Ordenación de parámetros en funciones (ayuda legibilidad):

- Context/handle first (arena, logger, struct de jugador, etc.)
- Entrada params siguiente
- Tamaño/cuenta justo después de su puntero (datos, dataCount)
- Los punteros de salida duran

## Uso de la palabra clave estática

Uso`static`para cualquier cosa que debe permanecer privada a un archivo .c:

-`static`funciones si no parte de la API pública
-`static`variables de archivo-scopio/globals

Preferir el estado que pasa como params sobre los archivos-globals para evitar dependencias ocultas.

## Formato " Diseño "

Indentación: 4 espacios (sin pestañas).

Brazos en la misma línea para estructuras de control, funciones, tipodefs:

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

Las funciones triviales de línea simple están bien si son cortas:

```c
static u32 hashU32(u32 v) { return v * 2654435761u; }
```

Reglas de selección:

- Espacio tras palabras clave:`if (`,`while (`,`for (`
- Espacio alrededor de operadores binarios:`a + b`,`x = y`
- Insinuaciones de puntero:`Type* ptr`(punto a tipo),`Type *array`(arrayo de tipo)
- colocación en const: preferir`const Type* ptr`cuando pointee no debería cambiar
- No hay comunicaciones en listas de param,`void func()`para paramillas vacías

Longitud de la línea: límite suave ~100 chars, romper lógicamente.

Declarar variables cercanas al primer uso, o grupo en la parte superior de bloque si más clara.

Parenthesize mix-operator expressions when precedence could confuse:

```c
// good
if ((flags & MASK) == VALUE) { ... }

// bad (surprising precedence)
if (flags & MASK == VALUE) { ... }
```

## Macros " Preprocesador

Constantes: SCREAMING CASE - título`#define MAX_PLAYERS 4`

Función-como macros: camelCase o prefijado, siempre paréntesis args + entero expr:

```c
#define MUL(a, b) ((a) * (b))
#define do { ... } while(0)   // for multi-statement
```

## File " Header Organization

Nombres de archivo: camelCase, prefiere palabras individuales (baseTypes.h, lobbyAPI. c)

Guardias de cabecera:`#ifndef PATH_LIKE_NAME_H`- estilo`#ifndef LOBBY_API_H`

Incluye el orden en .c / .h:

1. Cabecera correspondiente primero (en .c)
2. Otros directores de proyectos
3. Utils / cabeceras comunes
4. Standard/third-party only if really needed (prefer via common.h)

## Comentarios " Doxygen Documentation

Cada archivo .h y .c comienza con este bloque de cabecera:

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

Las funciones públicas (en .h) consiguen el bloque Doxygen completo:

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

Structs/enums in .h get brief + comentarios de campo:

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

Comentarios breves en línea para mundos obvios/macros:

```c
extern Rectangle skinButtonRect;        ///< Skin menu toggle area
#define APP_TEXT_FONT_SIZE 32           ///< Base UI font size
```

## Manejo de errores " Seguridad

- Uso`UNREACHABLE`macro para casos imposibles
- Comprobaciones de nulidad y límites defensivos
- Inicia sesión con nuestro logger (log info, log warn, etc.)
-`goto`ok para limpiar en caminos de error - añadir comentario por qué
- No números mágicos - uso de confianza`#define`o enums

## Gestión de memoria

Código de asignación bajo puede usar`malloc`/`calloc`.
Preferir a todos los creadores de arena (`globalArena`,`tempArena`,`context_alloc`) para la mayoría de las cosas - seguimiento de fugas más fácil.

**Creado:** 15 de enero de 2025
** Última actualización:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"