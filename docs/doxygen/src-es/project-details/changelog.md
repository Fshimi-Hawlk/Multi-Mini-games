@page changelog Cambio

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

Esta página enumera todos los cambios notables al monorepo Multi Mini-Games.

El formato sigue a los convenios de derechos humanos.
Usamos Versiones Semánticas donde tiene sentido, o Versión de Calendario para etapas de desarrollo tempranas.

### Cómo mantenemos este cambio

- Sólo se registran cambios significativos aquí (nueva integración del juego, importante código compartido, grandes refactores, importantes actualizaciones de documentación, etc.)
- Todo va por debajo **[Sin publicar]** hasta que el equipo decida que una etiqueta de versión está lista (ejemplo: 0.1.0 cuando el lobby + varios juegos son jugables juntos)

### [Sin liberación]

#### Añadido
- Fundamento Monorepo: carpetas compartidas (`firstparty/`,`thirdparty/`,`assets/`)
- Archivos de documentación en root: README.md, CONTRIBUTING.md, TODO.md, CodeStyleAndConventions.md
- Carpeta de plantilla`sub-project-example/`guía nueva estructura de juego
- Bases jurídicas: archivos LICENSE y NOTICE (créditos de terceros)
- Root Makefile con perezosos static-lib, API header copying to`firstparty/APIs/`, y el lobby ejecutable enlace
- Sistema de configuración central:`gameConfig.h`definición`GameConfig_St`,`AudioConfig_St`,`VideoConfig_St`+ macros predeterminadas
- Primera API de juego completo:`solitaireAPI.h`( patrón estándar de minijuegos, implementado por Maxime CHAUVEAU)
- encabezado inicial de la API de Tetris:`tetrisAPI.h`(previa aplicación)

#### Cambios
- README de raíz mejorada: explicación de proyecto más clara + comandos Git amigable para principiantes
- Simplificado y suavizado CONTRIBUTING.md: pasos de flujo de trabajo más fáciles, ejemplos prácticos Git, tono menos estricto en estilo
- Este cambio en sí mismo: reglas de actualización más claras y refleja recientes mejoras de doc
- Ha hecho más directa la redacción TODO.md preservando la lista completa de tareas
- Actualizado CodeStyleAndConventions.md: ahora presentado como un estilo recomendado (no obligatorio), con invitación a discutir reglas inconvenientes
- Manejo extendido de errores: nuevos códigos`ERROR_INVALID_CONFIG`,`ERROR_AUDIO_LOAD`,`ERROR_WINDOW_INIT`,`ERROR_ASSET_LOAD`a)`generalAPI.h`
- Patrón de API de minijuego estandarizado: uso consistente de`GameConfig_St*`, añadido`isRunning()`helper, unificado nombre a través de juegos

#### Fijación
- (ninguno registrado todavía)

#### Eliminado
- (ninguno registrado todavía)

**Creado:** 15 de enero de 2025
** Última actualización:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"