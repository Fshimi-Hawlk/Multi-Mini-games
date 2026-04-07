#include "ui/game.h"

void snake_drawBoard(const Board_t board) {
    for (int y = 0; y < SIZE_BOARD; y++) {
        for (int x = 0; x < SIZE_BOARD; x++) {
            int posX = x * CELL_SIZE;
            int posY = y * CELL_SIZE;

            if ((x + y) % 2 == 0) {
                DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, (Color) {100, 200, 100, 255});
            } else {
                DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, (Color) {80, 170, 80, 255});
            }

            if (board[y][x] == GAME_TILE_APPLE) {
                DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, RED);
            }
        }
    }
}

void snake_drawSnake(const Snake_St* snake, f32 interpolation, iVector2 direction) {
    SnakeBodyPart_St* current = snake->head;

    while (current != NULL) {
        f32 renderX, renderY;

        if (current->suivant) {
            // Interpoler de la position actuelle vers la suivante
            renderX = current->coord.x + (current->suivant->coord.x - current->coord.x) * interpolation;
            renderY = current->coord.y + (current->suivant->coord.y - current->coord.y) * interpolation;
        } else {
            // Pour la tête
            renderX = current->coord.x + direction.x * interpolation;
            renderY = current->coord.y + direction.y * interpolation;
        }

        f32 posX = roundf(renderX * CELL_SIZE);
        f32 posY = roundf(renderY * CELL_SIZE);

        if (current == snake->tail) {
            DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, DARKBLUE);
        } else {
            DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, BLUE);
        }

        current = current->suivant;
    }
}
