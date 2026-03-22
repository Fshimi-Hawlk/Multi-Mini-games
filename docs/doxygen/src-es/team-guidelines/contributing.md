@page contributing Directrices de contribución

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

Somos un pequeño equipo de cuatro estudiantes que aprenden la colaboración de Git mientras vamos.
Esta página describe nuestro flujo de trabajo actual para commits, ramas, solicitudes de tira y reseñas (al mes de marzo de 2026).
A pesar de que el proyecto sigue siendo WIP, ya hemos definido reglas básicas para mantener las cosas organizadas y rastreables.

El principio más importante sigue siendo el mismo: comunicarse abiertamente en el chat grupal cuando algo no está claro.

## Subdivisiones activas actuales

`main`actualmente se detiene y en su mayoría anticuado.
El desarrollo activo ocurre en estas ramas:

- Ramas de juego individuales (por ejemplo.`tetris`,`snake-classic`,`block-blast`, etc.) - confía en el desarrollo de juegos individuales y pulido
-`docs-setup`- documentación, estilo de código, convenciones, guías de configuración
-`sub-games-integration-test`(por lo general lo llamamos`sgit`) - mejoras del lobby, integración del juego, correcciones de errores compartidos

Flujo típico para un miembro del equipo:
1. Saque lo último de la rama en la que trabaja
2. Trabajar en su función / juego / arreglar
3. Compromiso con mensajes claros (véase infra)
4. Empuja tu rama
5. Abrir una Solicitud de Tiro (generalmente apuntando`sgit`para las integraciones, o la rama común pertinente)
6. Solicitar opiniones en grupo chat
7. Fijar la retroalimentación - contrato aprobado y probado en lobby - título merge

Una vez los juegos son sólidos en un solo jugador en`sgit`, más tarde fusionaremos todo en`main`(incluyendo futuras piezas multijugador).

Para los pasos previstos, el estado actual y la visión a largo plazo véase

## Commit Mensajes

Seguimos un estilo simplificado **Conventional Commits**.
Ayuda a todos a entender los cambios rápidamente durante las críticas y en el registro git.

Prefijos comunes que utilizamos con más frecuencia:

-`feat:`nueva función / cambio visible
-`fix:`bug
-`docs:`documentación, comentarios, guías
-`style:`formato, espacio blanco, nombre (sin cambio lógico)
-`refactor:`limpieza de código / mejor estructura (sin cambio de comportamiento)
-`test:`pruebas agregadas o fijadas
-`chore:`mantenimiento, herramientas, .gitignore, pequeñas limpiezas
-`build:`Makefile, construir banderas, opciones de compilador

También a veces usamos:
-`fetch`/`merge`-] traer cambios de otra rama
-`undo`- claramente sacando algo añadido antes

Ejemplos de nuestros recientes compromisos:
-`feat: added hold piece mechanic in Tetris`
-`fix: resolved player getting stuck on lobby platforms`
-`docs: updated API conversion guide with new error codes`
-`refactor: moved player globals into BallSystem_St`
-`undo: removed temporary pause menu draft`

Para cambios muy pequeños (tipo, espacio de seguimiento) o hacemos un pequeño`style:`/`chore:`comprometerlo o doblarlo en el siguiente compromiso real — ambos están bien.

Opcional pero recomendado: añadir un resumen **una línea** en la parte superior de mensajes de confirmación más largos.
Hace`git log --oneline`y GitHub compromete listas mucho más fáciles de escanear.

Ejemplo de buen mensaje de confirmación reciente:

```text
Docs update / less globals / temporary pause menu removal / fixes / new font
- docs: Updated creation date and last updated dates
- chore: Removed unused windowRect and appFont from globals
- refactor: Moved game globals to BallSystem_St fields
- undo: Removed pause menu draft - will reimplement later in lobby context
- fix: Fixed font size warning display
- fix: Can now place daub immediately when ball text appears
- chore: Switched font to NotoSansMono-Bold.ttf
```

No somos estrictos con los prefijos: la claridad y la historia útil importan más que la categorización perfecta.
Si no está seguro, elija el más cercano o pregunte en chat.

## Branch Naming for Pull Solicita

Utilizamos este patrón para las ramas PR:

`<target-branch>-PR<day-sequence-number>-DD/MM`

Ejemplos:
-`tetris-PR1-16/03`
-`sgit-PR3-16/03`
-`docs-setup-PR2-17/03`

Esto hace que sea obvio lo que la rama apunta, cuando fue creada, y su orden ese día.

## Solicitudes de admisión

Las relaciones públicas son nuestra principal manera de revisar, discutir e integrar cambios de forma segura.

### Antes de abrir un PR
- Hacer pruebas:`make MODE=clang-debug run-tests`(o al menos)`make tests`)
- Asegúrate de que construya sin advertencias/errores

### Creación de la rama PR (dos casos comunes)

**Caso 1: Ayudar en una rama de juego/fea** (por ejemplo, mejorar`tetris`)

```bash
git fetch origin
git checkout tetris
git pull origin tetris
git checkout -b tetris-PR1-16/03
# work -> commit -> ...
git push origin tetris-PR1-16/03
```

**Caso 2: Integración en rama compartida** (por ejemplo:.`sgit`,`docs-setup`)

```bash
git fetch origin
git checkout sub-games-integration-test
git pull origin sub-games-integration-test
git checkout -b sgit-PR1-16/03
# work -> commit -> ...
git push origin sgit-PR1-16/03
```

### Apertura y revisión
- Título: corto y claro (por ejemplo, "Colisión de Fix en plataformas de vestíbulo")
- Descripción: explicar qué + por qué, enlace docs/issues relacionados si hay
- Añadir revisores:
- Ramas de juego - principalmente el propietario de la rama
- Ramas compartidas - confiar todos los demás miembros del equipo
- Ping group chat: "opened PR #X for review"

A menudo abrimos como Borrador de PR temprano para obtener comentarios antes de que se termine.

### Reglas de fusión (convención de equipo)
- Sucursales de juego/feature - estipula al menos la aprobación del propietario de la rama
- solo el dueño de la rama (o delegado) debe fusionarse
- Ramas compartidas`sgit`,`docs-setup`,`main`, etc.) - estipula la aprobación de todos los demás miembros del equipo
- Solo el autor del PR se fusiona una vez que las aprobaciones están en (a menos que se le pida otra cosa)
- Uso ** "Crear un compromiso de fusión"** (por defecto) - Confía mantiene plena autoría e historia granular
- No squash o rebase a menos que el equipo acepte explícitamente que PR

### Después de la fusión
- Eliminar la rama de la PR de GitHub (con la pestaña de las ramas de mantenimiento limpio)
- Lo último en la rama de destino
- La historia sigue siendo completamente rastreable a través de merge commit

## Code Style

Ver TOTOK0/propios para nombrar, formatear, trazado de archivos, etc.

Se basa principalmente en cómo comenzó el proyecto: el objetivo es la consistencia de referencia, no la uniformidad perfecta.
Si algo se siente incómodo o mal, tráigalo en el chat o abra una PR para mejorar la guía de estilo.

## Preguntas

- Ping the group chat al abrir un PR o cuando necesites entrada
- Preguntar "¿se ve bien esto?" o "¿qué piensas de este enfoque?" es completamente normal
- Los conflictos de fusión ocurren - sólo pide ayuda, no hay problema en absoluto

Gracias por trabajar juntos — ¡Toma tu tiempo, haz preguntas, disfruta del proceso!

**Creado:** 15 de enero de 2026
** Última actualización:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"