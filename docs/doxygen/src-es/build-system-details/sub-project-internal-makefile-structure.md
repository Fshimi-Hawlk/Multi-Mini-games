@page sub_project_internal_makefile_structure Estructura de perfil interno

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

Esta página explica la estructura interna** del sistema Makefile utilizado en cada subproyecto (juegos, cabildeo, etc.).

Utilizamos un diseño **modular split-Makefile** para mantener las cosas mantenibles, legibles y fáciles de extender más adelante (especialmente para diferentes plataformas).

Toda la verdadera lógica Makefile vive en`make/`carpeta.
La raíz`Makefile`en cada subproyecto se mantiene muy corto - sólo incluye los archivos modulares en un orden fijo y declara`.PHONY`objetivos.

## Directorio Diseño

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

## Orden de Inclusión (muy importante)

La raíz Makefile incluye estos archivos **exactamente en esta secuencia**:

1.`00-config.mk`- establece compiladores, banderas, MODE, directorios, detecta OS, incluye plataforma/*.mk
2.`10-sources.mk`- escaneos src/ y pruebas/ para archivos .c, crea listas de dependencia de objetos
3.`20-build-rules.mk`- define cómo compilar archivos .o y enlaces ejecutables/ libs estáticas
4.`30-targets-main.mk`- objetivos de usuario para el programa principal (todos, reconstruir, ejecutar-main, run-gdb, static-lib)
5.`40-targets-tests.mk`- objetivos relacionados con las pruebas (pruebas, pruebas de ejecución con registros de timetamped)
6.`50-tools.mk`- material de ayuda: modo silencioso/verboso, limpio-todo, ayuda a la salida
7.`99-overrides.mk`- último archivo (opcional) para los ajustes de la máquina local, nunca comprometido

Los archivos tempranos definen principalmente variables.
Los archivos posteriores usan esas variables para crear reglas y objetivos reales.

## ¿Por qué dividir y numerar por aquí?

- Evaluar los archivos en el orden que están incluidos - numerar fuerzas comportamiento predecible
- La separación clara de las preocupaciones hace más fácil encontrar / editar una parte sin romper otros
- 00 = entorno de configuración
- 10 = descubrimiento de fuentes
- 20 = mecánico de construcción de núcleo
- 30/40 = lo que la gente realmente corre (hacer correr-mano, hacer pruebas, etc.)
- 50 = herramientas de conveniencia del desarrollador
- 99 = overrides personales sin ginebra contaminantes
- Las cosas específicas de la plataforma se mantienen aisladas`platform/`así podemos añadir las diferencias Windows/macOS/Linux limpiamente más tarde

## Mesa de referencia rápida

Silencio Archivo Silencio Principal trabajo Silencio Aspectos importantes que define / crea Silencio
|-----------------------|--------------------------------------------------------|----------------------------------------------------|
TEN 00-config.mk ANTE Compiler, banderas, modos, caminos, Detección de sistemas operativos TEN CC, CFLAGS, LDFLAGS, MODE, OBJ DIR, BIN, plataforma incluyen TEN
tención 10-sources.mk  durable Escanes para archivos fuente, construye listas TEN LIB SOURCES, LIB OBJECTS, TEST BINS, DEPS TEN
TEN 20-build-rules.mk ANTE Compilation " linking rules TEN %.o pattern, $(BIN), $(STATIC LIB), test linking TEN
tención 30-targets-main.mk ← Principales objetivos del programa  durable all, reconstruir, ejecutar-main, run-gdb, static-lib
TENCIÓN 40-objetivos-tests.mk Pruebas de construcción " en funcionamiento con troncos , pruebas de duración, pruebas de ejecución
tención 50-tools.mk Silencio control, limpio, ayuda, envoltorio de stdbuf TEN SILENT PREFIX, limpio, ayuda
tención 99-overrides.mk ← Máquina local tweaks (git ignorado) Silencio cualquier override que necesites
Silencioso plataforma/*.mk  durable OS-specific flags " libs  durable BASE CFLAGS, BASE LDFLAGS, raylib paths, etc

## Páginas relacionadas

- < > Cómo utilizar realmente los objetivos y modos

**Creado:** 23 de febrero de 2026
** Última actualización:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"