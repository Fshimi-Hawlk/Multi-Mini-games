@page todo TODO

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

Esta página enumera las principales tareas abiertas para todo el monorepo.
La mayoría de estas son cosas que impactan el lobby, código compartido o la colección general de minijuegos.

Las tareas se clasifican aproximadamente de "hacer esto pronto" a "estaría bien más tarde".
Actualizamos este archivo a medida que vamos - no dude en añadir temas o cambiar prioridades después de la discusión de grupo.

## Alta prioridad - hacer esto bastante pronto

- [x] Crear un Makefile global a nivel raíz
- Done. Ahora maneja la biblioteca perezosa construye, copia los encabezados de API a la primera parte/APIs/, soporta construcciones incrementales, redes de fuerza, modos de depuración, sanitarios, etc.

Acaba de configurar la raíz`docs/`carpeta correctamente
- No. Ya creó la carpeta y movió varias guías allí (`API_Conversion.md`,`CodeStyleAndConventions.md`,`makefile.md`, etc.) para mantener la raíz limpia.
- Main Doxyfile está en`docs/`y documentos lobby + código compartido
- Siguiente: ampliar para incluir documentación para cada módulo de juego una vez más se integran

## Después del primer juego se fusiona completamente en el principal

- [x] Escribir reglas claras para convertir un juego independiente en una API compatible con lobby
- título Done. Ver

- [ ] Decide guardar / progreso / configuración del sistema
- No. Opciones hasta ahora: archivos de texto simples`assets/<game-name>/saves/`, simple JSON, o algo personalizado?
- Plan: añadir ayudantes compartidos de carga y ahorro en`firstparty/`para que cada juego pueda utilizar el mismo código

- [ ] Definir el formato de la clasificación local
-] Mantenlo sencillo por ahora (sólo archivos locales, no online).
- No. Tal vez JSON o binario en`assets/leaderboards/`?

## Es bueno tener / largo plazo

- [x] Añadir una hoja de ruta visible
- Done. Ver el
- título adicional, referir a

- [ ] Agregue la configuración básica de audio (reductores de volumen, toggles mudos) en el vestíbulo
- [ ] Añadir desbloqueo de piel persistente / cosméticos a través de juegos
Experimento con un multijugador local muy básico (split-screen o hotseat)
- [ ] Añadir simple recuperación de errores en el lobby (recargar juego en el accidente en lugar de salida)

Los grandes cambios deben discutirse primero en el chat de grupo para que todos estén de acuerdo.

**Creado:** Marzo 02, 2025
** Última actualización:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"