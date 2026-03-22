@page sub_project_makefile Documentación de Makefile Subproyecto

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

Esta página describe el Makefile utilizado dentro de cada carpeta subproyecto (tetris/, lobby/, nuevos juegos copiados del subproyecto-example/, etc.).

El Makefile maneja la construcción del juego como un ejecutable independiente para la prueba, produciendo una biblioteca estática para la integración del vestíbulo, pruebas de unidad y soporte múltiples modos de depuración/optimización.

## Sinopsis

Principales responsabilidades:
- Construir ejecutable independiente`src/main.c`(para pruebas rápidas)
- Construir biblioteca estática`lib<gameName>.a`(utilizado por root Makefile para conectarse al lobby)
- Realizar pruebas de unidad desde`tests/`
- Soporte varios modos de construcción (liberación / depuración / sanitizers / valgrind)
- Seguimiento automático de la dependencia del encabezado (-MMD -MP)
- Silent by default, verbose with VERBOSE=1
- Banderas personalizadas a través de EXTRA CFLAGS / EXTRA LDFLAGS
- Prueba de registro y salida de valgrind en carpetas de timetamped bajo`logs/`

Rutas de salida típicas (en relación con la carpeta subproyecto):
- Objetos:`build/obj/`
- Standalone binario:`build/bin/main`
- La lib estática:`build/lib/lib<gameName>.a`
- Boletines de prueba:`build/bin/tests/<test-name>`

## Modos de construcción compatibles

tención Modo Silencioso Compilador Silencio Banderas principales Silencio Requisitos para usos típicos caso Silencio
|-----------------|----------|------------------------------------------|--------------------|----------------------------------------|
Silencioso liberación Silencio gcc Silencio -O2 Silencio - Silencio Final / optimizado standalone runs Silencio
Silencioso debug Silencio gcc Silencio -Wall -Wextra -g -O0 Silencio - Silencio Desarrollo normal Silencio
tención estricta-debug Silencio gcc Silencio -Werror -Wall -Wextra -pedantic -g -O0 Silencio - Silencio Catch advertencias antes de fusionarse Silencio
Silencio clang-debug Silencio clang Silencio mismo + -fsanitize=address,undefinido Silencio clang instalado Silencio Memoria / UB bug hunting latitud
Silencio valgrind-debug Silencio gcc Silencio -Werror -Wall -Wextra -pedantic -g -O0 Silencio valgrind instaló Silencio Runtime fuga / cheques de lectura inválidos

El modo predeterminado es`release`.

## Objetivos más útiles

-`all`/ objetivo predeterminado : construir ejecutable independiente
-`static-lib`: construir biblioteca estática para la integración del vestíbulo
-`tests`: construir todos los binarios de prueba
-`run-main`: construcción + ejecución independiente ejecutable
-`run-tests`: construir + ejecutar todas las pruebas (con salida en vivo + troncos)
-`rebuild`: limpio + todo
-`clean`: eliminar compilación / carpeta
-`run-gdb`: ejecutar standalone binario bajo gdb

## Ejemplos rápidos

Basic standalone build + run:

```bash
make
make run-main
```

Debug / modo estricto:

```bash
make MODE=debug
make MODE=strict-debug rebuild run-main
```

Con los sanitizantes o Valgrind:

```bash
make MODE=clang-debug run-main
make MODE=valgrind-debug run-tests
```

Construye sólo la lib estática (para el lobby):

```bash
make static-lib
```

Verbose + advertencias adicionales:

```bash
make VERBOSE=1 EXTRA_CFLAGS="-Wshadow -Wconversion" MODE=strict-debug
```

## Notas de salida

-`run-tests`creaciones`logs/tests-<timestamp>/`con un archivo .log por prueba (stdout + stderr)
- En`valgrind-debug`modo: extra`logs/valgrind-<timestamp>/`carpetas con informes Valgrind
- Los registros solo ahorran salida no vacía
- Fiabilidad de salida de cierre: usos`stdbuf --output=L --error=L`(Búferes en línea) cuando está disponible para que las últimas líneas de printf() aparezcan incluso en SIGABRT o ASan choque
- Si`stdbuf`falta - aviso impreso una vez, la salida podría truncate en el fallo - añadir explicit`fflush(stdout); fflush(stderr);`en el código de prueba si es necesario

## Requisitos " Portabilidad

-`clang-debug`necesita un clan
-`valgrind-debug`necesita valgrind
-`stdbuf`(de Coreutils) es opcional pero recomendado para la salida de prueba limpia en Linux

## Páginas relacionadas

- No. Cómo el Makefile es **internally estructurado**

**Creado:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"