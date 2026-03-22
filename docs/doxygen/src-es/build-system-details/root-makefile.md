@page root_makefile Root Makefile Documentation

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

Esta página describe el **root Makefile** en el Multi Mini-Games monorepo.

La raíz Makefile es responsable de:
- Construcción de bibliotecas estáticas para todos los juegos integrados (`libtetris.a`, etc
- Copiar el encabezado público de cada juego API (`tetrisAPI.h`, etc.)`firstparty/APIs/`
- Vincular todo juntos en un solo vestíbulo ejecutable
- Hacer todo esto gradualmente cuando sea posible (sólo reconstruir lo que ha cambiado)

Soporta los mismos modos de construcción que el subproyecto Makefiles para que todo el monorepo se sienta consistente.

## Modos de construcción

Usted elige el modo añadiendo`MODE=xxx`a cualquier orden. Default es`release`.

Silencioso Modo Silencioso Compilador Silencio Banderas principales Silencio Notas / requerimientos adicionales
|-------------------|----------|----------------------------------------|-------------------------------|
Silencioso liberación Silencio gcc Silencio -O2 Silencio Optimizado, sin símbolos de depuración Silencio
Silencioso debug Silencio gcc Silencio -Wall -Wextra -g -O0 Silencio Depuración básica + advertencias Silencio
tención estricta-debug Silencio gcc Silencio -Werror -Wall -Wextra -pedantic -g Silencio Las advertencias se convierten en errores
tención clang-debug Silencio clang Silencio + -fsanitize=address,undefinido Silencio Necesitas clang instalado Silencio
tención valgrind-debug Silencio gcc Silencio -g -O0 (envoltorio de carreras de vacuno) Silencio Necesitas valgrind instalado

## Principales objetivos

-`make`/`all`/`bin`  
Construye/actúa bibliotecas + vincula el ejecutable del vestíbulo (incremental)

-`libs`  
Construye o actualiza sólo las bibliotecas estáticas + copias de los encabezados de API

-`rebuild`  
Limpio completo + reconstruir todo desde cero

-`clean`  
Elimina la raíz`build/`carpeta (bibliografías + exe)

-`clean-all`  
Limpia la raíz + las llamadas limpias en cada subproyecto

-`rebuild-exe`  
Fuerzas de reenlazamiento del vestíbulo ejecutable solamente (útil después de cambios de lib)

-`run-exe`  
Corre el vestíbulo desde`build/bin/main`(o lo que sea que sea MAIN NAME está listo)

-`tests`/`run-tests`  
Construye y ejecuta todas las pruebas de unidad (lobby + cada juego)

-`help`  
Imprime esta lista de objetivos + modos

## Comandos comunes (lo que más usamos)

Construcción rápida normal + ejecución:

```bash
make bin
make run-exe
```

Forzar todo en depuración con los sanitizantes (bueno para atrapar insectos):

```bash
make MODE=clang-debug rebuild run-exe
```

Recuperar las bibliotecas sólo (después de agregar / cambiar un juego):

```bash
make rebuild-libs
```

Ejecute todas las pruebas con cheques adicionales:

```bash
make MODE=clang-debug run-tests
```

## Asset Path Handling

Para hacer que los activos funcionen independientes y en el vestíbulo:

- Cuando se construye dentro de una carpeta de juego`assets/…`
- Cuando se construye desde root - usa`<gamename>/assets/…`(por ejemplo.`tetris/assets/`)

Esto es controlado por el paso`-DASSET_PATH="..."`via`EXTRA_CFLAGS`en el Makefile.

## API Header Copying

Durante`libs`,`bin`,`rebuild-libs`objetivos:
- Cada juego`<gamename>API.h`es copiado de`<gamename>/include/`a`firstparty/APIs/`
- Sólo copias si el encabezado existe

Esto permite el vestíbulo`#include "APIs/tetrisAPI.h"`sin perder el camino.

## Debugging " Logs

- Los exámenes crean carpetas como`logs/tests-2026-03-16_14-30/`
- El modo Valgrind crea`logs/valgrind-<timestamp>/`
- Añadir`VERBOSE=1`a cualquier orden de hacer para ver las líneas gcc/clang completa

## Generación de documentación

```bash
make docs
```

- No`open docs/doxygen/index.html`


**Creado:** Marzo 02, 2025
** Última actualización:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"