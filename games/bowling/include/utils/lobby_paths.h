/**
    @file utils/lobby_paths.h
    @brief Asset path overrides when bowling is built as a library for the lobby.

    Included when compiling libbowling.a for integration into the lobby.
    Overrides ASSETS_PATH and SOUNDS_PATH to point to bowling's asset directory
    relative to the lobby working directory (the project root).
*/
#ifndef BOWLING_LOBBY_PATHS_H
#define BOWLING_LOBBY_PATHS_H

#undef ASSETS_PATH
#define ASSETS_PATH "jeux/bowling/assets/"

#undef SOUNDS_PATH
#define SOUNDS_PATH "jeux/bowling/assets/sounds/"

#endif // BOWLING_LOBBY_PATHS_H
