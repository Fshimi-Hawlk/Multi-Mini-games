#include "ui/game.h"

void drawBoard(int board[SIZE_BOARD][SIZE_BOARD]) {
    for (int y = 0; y < SIZE_BOARD; y++) {
        for (int x = 0; x < SIZE_BOARD; x++) {
            int posX = x * CELL_SIZE;
            int posY = y * CELL_SIZE;

            if ((x + y) % 2 == 0)
                DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, (Color){100, 200, 100, 255});
            else
                DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, (Color){80, 170, 80, 255});

            if (board[y][x] == APPLE) {
                DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, RED);
            }
        }
    }
}

void drawSnake(Queue_St* q, float interpolation, iVector2 direction) {
    t_element* current = q->head;

    while (current != NULL) {
        float renderX, renderY;

        if (current->suivant) {
            // Interpoler de la position actuelle vers la suivante
            renderX = current->coord.x + (current->suivant->coord.x - current->coord.x) * interpolation;
            renderY = current->coord.y + (current->suivant->coord.y - current->coord.y) * interpolation;
        } else {
            // Pour la tête
            renderX = current->coord.x + direction.x * interpolation;
            renderY = current->coord.y + direction.y * interpolation;
        }

        float posX = roundf(renderX * CELL_SIZE);
        float posY = roundf(renderY * CELL_SIZE);

        if (current == q->tail) {
            DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, DARKBLUE);
        } else {
            DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, BLUE);
        }

        current = current->suivant;
    }
}
