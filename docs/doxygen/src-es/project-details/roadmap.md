@page roadmap Project Roadmap

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

Esta página describe dónde estamos ahora mismo y el camino difícil que acordamos para mantener las cosas manejables.

## Situación actual (marzo 2026)

-`main`- Esqueleto temprano + algunos doctores, pero completamente anticuado ahora. Básicamente dejamos de tocarlo hace meses.
- Ramas de desarrollo activas:
- Ramas de juego individuales (por ejemplo.`tetris`,`snake-classic`,`block-blast`,`memory`, etc.) - confiar donde los juegos de un solo jugador se construyen y pulido independiente
-`docs-setup`- documentación compartida, reglas de estilo de código, guías de configuración, pequeños refactores que tocan múltiples partes
-`sub-games-integration-test`(lo llamamos`sgit`) - Confía en la actual rama de integración principal. Combinamos juegos de un solo jugador terminados aquí para conectarlos al vestíbulo, probarlos juntos, arreglar errores del vestíbulo, añadir características compartidas

Ahora mismo: varios juegos de trabajo independiente, una pareja ya son jugables dentro del vestíbulo vía`sgit`. El vestíbulo en sí tiene movimiento básico, desencadenantes de colisión y UI, pero sigue siendo duro alrededor de los bordes.

## Estructura prevista a largo plazo

Para evitar grandes conflictos cuando empezamos a añadir multijugador (código de red, problemas de sincronización, lógica de juego diferente), acordamos este enfoque gradual:

1. **Fase del jugador individual** (actual / próximos meses)
- Acabar y pulir versiones de un solo jugador en sus propias ramas
- Integre uno por uno en el vestíbulo`sgit`
- Una vez que tenemos 4-5+ juegos trabajando bien juntos - título tal vez renombre`sgit`algo más claro como`singleplayer-games-integration-test`

2. **Fase del jugador Multijugador** (el siguiente paso importante después de que un jugador se sienta sólido)
- Crear una nueva rama:`multiplayer-games-integration-test`(corto:`mgit`)
- Versiones multijugador prototipos allí (núcleos de red, basados en turnos o en tiempo real, diferencias de interfaz de usuario)
- Ganar las versiones MP en el lobby por separado - confiar mantener código de un solo jugador sin tocar

3. **Fase comunitaria** (cuando ambos SP y MP estén listos)
- Crear`combine-games-integration-test`(corto:`cgit`)
- Combinar ramas de un solo jugador y multijugador allí
- Resolver conflictos (modos de red local vs, toggles de interfaz de usuario, problemas de rendimiento, etc.)
- Prueba el vestíbulo completo con una mezcla de juegos SP + MP

4. **Stable release to main**
- Cuando`cgit`es jugable y razonablemente estable`main`
- Etiqueta una versión (por ejemplo v0.1-singleplayer o v1.0 si ambicioso)
- Tal vez crear GitHub versión con construcciones, capturas de pantalla, descripción corta

Este es nuestro plan para mantener las ramas testables y reducir el infierno de fusión.
En la práctica:
- Si algo es pequeño y libre de conflictos, podríamos saltarnos pasos y fusionarnos antes
- Si el multijugador tarda más de lo esperado, podemos enviar una versión solo para un jugador a`main`primero
- Todo es flexible - discutiremos en chat de grupo y ajustaremos cuando sea necesario

## Quick To-Do / Lista de ideas

- Consigue al menos 4-5 juegos de un solo jugador completamente integrados y jugables`sgit`
- Mejorar el cabildeo: una selección más agradable del juego (tal vez signos o puertas en lugar de sólo cajas de éxito), la pantalla de puntuaciones básicas, pausa / sueño, persistencia de la piel
- Iniciar prototipado multijugador: elegir 1–2 juegos simples (por ejemplo, basado en el turno como memoria o bloque-blast) y experimentar con el networking en una rama de características
- Mejoras compartidas: mejor manejo de audio, configuración de entrada unificada, mejor registro/recuperación de errores
- Documentación: añadir capturas de pantalla o GIF del lobby + juegos de ejecución, terminar guías de configuración

¡Si algo de esto suena interesante, no dude en saltar! Open issues, start Draft PRs, hacer preguntas en chat. Se esperan errores, todos estamos aprendiendo.

**Creado:** 16 de marzo de 2026
** Última actualización:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"