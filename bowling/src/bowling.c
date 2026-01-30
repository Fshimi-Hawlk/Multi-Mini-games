#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_PINS 10
#define MAX_BALLS 10

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
} Ball;

typedef struct {
    Vector2 position;
    bool knocked;
    float angle;
    float fallSpeed;
} Pin;

typedef struct {
    Pin pins[MAX_PINS];
    Ball ball;
    int currentFrame;
    int currentThrow;
    int scores[MAX_BALLS][3]; // [frame][throw1, throw2, total]
    float powerLevel;
    float angle;
    bool charging;
    bool ballRolling;
    bool gameOver;
    int totalScore;
} GameState;

// Assets
Texture2D texBall;
Texture2D texPin;
Texture2D texLane;
Texture2D texPowerBar;
Texture2D texArrow;

void InitPins(GameState* game) {
    // Configuration triangulaire des quilles
    float startX = GetScreenWidth() / 2.0f;
    float startY = GetScreenHeight() * 0.2f;
    float spacing = 60.0f;

    int pinIndex = 0;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col <= row; col++) {
            game->pins[pinIndex].position.x = startX + (col - row / 2.0f) * spacing;
            game->pins[pinIndex].position.y = startY + row * spacing * 0.866f;
            game->pins[pinIndex].knocked = false;
            game->pins[pinIndex].angle = 0.0f;
            game->pins[pinIndex].fallSpeed = 0.0f;
            pinIndex++;
        }
    }
}

void InitGame(GameState* game) {
    game->currentFrame = 0;
    game->currentThrow = 0;
    game->powerLevel = 0.5f;
    game->angle = 0.0f;
    game->charging = false;
    game->ballRolling = false;
    game->gameOver = false;
    game->totalScore = 0;

    for (int i = 0; i < MAX_BALLS; i++) {
        game->scores[i][0] = 0;
        game->scores[i][1] = 0;
        game->scores[i][2] = 0;
    }

    InitPins(game);

    game->ball.position = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() * 0.85f};
    game->ball.velocity = (Vector2){0, 0};
    game->ball.radius = 25.0f;
    game->ball.active = false;
}

void LoadAssets() {
    texBall = LoadTexture("assets/bowling_ball.png");
    texPin = LoadTexture("assets/bowling_pin.png");
    texLane = LoadTexture("assets/bowling_lane.png");
    texPowerBar = LoadTexture("assets/power_bar.png");
    texArrow = LoadTexture("assets/arrow.png");
}

void UnloadAssets() {
    UnloadTexture(texBall);
    UnloadTexture(texPin);
    UnloadTexture(texLane);
    UnloadTexture(texPowerBar);
    UnloadTexture(texArrow);
}

void UpdateBall(GameState* game) {
    if (!game->ball.active) return;

    // Appliquer la gravité et le mouvement
    game->ball.position.x += game->ball.velocity.x;
    game->ball.position.y += game->ball.velocity.y;

    // Friction
    game->ball.velocity.x *= 0.99f;
    game->ball.velocity.y *= 0.99f;

    // Vérifier les collisions avec les quilles
    for (int i = 0; i < MAX_PINS; i++) {
        if (game->pins[i].knocked) continue;

        float dist = Vector2Distance(game->ball.position, game->pins[i].position);
        if (dist < game->ball.radius + 15.0f) {
            game->pins[i].knocked = true;
            game->pins[i].fallSpeed = 5.0f;
            game->pins[i].angle = (rand() % 360) * DEG2RAD;

            // Rebond de la balle
            Vector2 normal = Vector2Normalize(Vector2Subtract(game->ball.position, game->pins[i].position));
            game->ball.velocity = Vector2Add(game->ball.velocity, Vector2Scale(normal, 2.0f));
        }
    }

    // Arrêter la balle si elle sort de l'écran ou est trop lente
    if (game->ball.position.y < -50 || Vector2Length(game->ball.velocity) < 0.5f) {
        game->ball.active = false;
        game->ballRolling = false;

        // Compter les quilles tombées
        int pinsDown = 0;
        for (int i = 0; i < MAX_PINS; i++) {
            if (game->pins[i].knocked) pinsDown++;
        }

        // Enregistrer le score
        game->scores[game->currentFrame][game->currentThrow] = pinsDown -
            (game->currentThrow == 0 ? 0 : game->scores[game->currentFrame][0]);

        // Passer au prochain lancer
        if (pinsDown == MAX_PINS || game->currentThrow == 1) {
            game->currentFrame++;
            game->currentThrow = 0;
            if (game->currentFrame >= MAX_BALLS) {
                game->gameOver = true;
            } else {
                InitPins(game);
            }
        } else {
            game->currentThrow = 1;
        }

        // Réinitialiser la balle
        game->ball.position = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() * 0.85f};
        game->ball.velocity = (Vector2){0, 0};
    }
}

void UpdatePins(GameState* game) {
    for (int i = 0; i < MAX_PINS; i++) {
        if (game->pins[i].knocked && game->pins[i].fallSpeed > 0) {
            game->pins[i].fallSpeed -= 0.2f;
        }
    }
}

void CalculateTotalScore(GameState* game) {
    game->totalScore = 0;
    for (int i = 0; i < game->currentFrame; i++) {
        int frameScore = game->scores[i][0] + game->scores[i][1];

        // Strike bonus
        if (game->scores[i][0] == 10 && i < MAX_BALLS - 1) {
            frameScore += game->scores[i + 1][0] + game->scores[i + 1][1];
        }
        // Spare bonus
        else if (frameScore == 10 && i < MAX_BALLS - 1) {
            frameScore += game->scores[i + 1][0];
        }

        game->scores[i][2] = frameScore;
        game->totalScore += frameScore;
    }
}

void Update(GameState* game) {
    if (game->gameOver) {
        if (IsKeyPressed(KEY_R)) {
            InitGame(game);
        }
        return;
    }

    if (game->ballRolling) {
        UpdateBall(game);
        UpdatePins(game);
        CalculateTotalScore(game);
        return;
    }

    // Contrôle de l'angle
    if (IsKeyDown(KEY_LEFT)) game->angle -= 2.0f;
    if (IsKeyDown(KEY_RIGHT)) game->angle += 2.0f;
    game->angle = Clamp(game->angle, -45.0f, 45.0f);

    // Contrôle de la puissance
    if (IsKeyDown(KEY_SPACE)) {
        game->charging = true;
        game->powerLevel += 0.02f;
        if (game->powerLevel > 1.0f) game->powerLevel = 0.0f;
    }

    if (game->charging && IsKeyReleased(KEY_SPACE)) {
        game->charging = false;
        game->ballRolling = true;
        game->ball.active = true;

        float speed = 15.0f * game->powerLevel;
        float angleRad = game->angle * DEG2RAD;
        game->ball.velocity.x = sinf(angleRad) * speed;
        game->ball.velocity.y = -cosf(angleRad) * speed;
    }
}

void Draw(GameState* game) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Dessiner la piste
    DrawTexturePro(texLane,
        (Rectangle){0, 0, texLane.width, texLane.height},
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
        (Vector2){0, 0}, 0.0f, WHITE);

    // Dessiner les quilles
    for (int i = 0; i < MAX_PINS; i++) {
        if (!game->pins[i].knocked) {
            DrawTexturePro(texPin,
                (Rectangle){0, 0, texPin.width, texPin.height},
                (Rectangle){game->pins[i].position.x, game->pins[i].position.y, 40, 60},
                (Vector2){20, 30}, 0.0f, WHITE);
        }
    }

    // Dessiner la balle
    if (!game->ballRolling || game->ball.active) {
        DrawTexturePro(texBall,
            (Rectangle){0, 0, texBall.width, texBall.height},
            (Rectangle){game->ball.position.x, game->ball.position.y,
                        game->ball.radius * 2, game->ball.radius * 2},
            (Vector2){game->ball.radius, game->ball.radius}, 0.0f, WHITE);
    }

    // Dessiner la flèche de direction (quand pas en mouvement)
    if (!game->ballRolling) {
        Vector2 arrowPos = {GetScreenWidth() / 2.0f, GetScreenHeight() * 0.7f};
        DrawTexturePro(texArrow,
            (Rectangle){0, 0, texArrow.width, texArrow.height},
            (Rectangle){arrowPos.x, arrowPos.y, 60, 60},
            (Vector2){30, 30}, -game->angle, WHITE);
    }

    // Dessiner la barre de puissance
    if (!game->ballRolling) {
        float barX = GetScreenWidth() / 2.0f - 150;
        float barY = GetScreenHeight() * 0.9f;

        DrawRectangle(barX, barY, 300, 30, DARKGRAY);
        DrawRectangle(barX, barY, 300 * game->powerLevel, 30,
            game->powerLevel < 0.3f ? GREEN :
            game->powerLevel < 0.7f ? YELLOW : RED);
        DrawRectangleLines(barX, barY, 300, 30, BLACK);
    }

    // Afficher le score
    DrawText(TextFormat("Frame: %d/%d", game->currentFrame + 1, MAX_BALLS), 10, 10, 30, BLACK);
    DrawText(TextFormat("Throw: %d", game->currentThrow + 1), 10, 45, 25, BLACK);
    DrawText(TextFormat("Score: %d", game->totalScore), 10, 75, 30, DARKBLUE);

    // Tableau des scores
    int scoreX = GetScreenWidth() - 550;
    int scoreY = 10;
    DrawRectangle(scoreX, scoreY, 540, 60, Fade(WHITE, 0.8f));
    DrawRectangleLines(scoreX, scoreY, 540, 60, BLACK);

    for (int i = 0; i < MAX_BALLS && i <= game->currentFrame; i++) {
        int x = scoreX + 5 + i * 54;
        DrawText(TextFormat("%d", i + 1), x + 15, scoreY + 5, 15, BLACK);
        if (i < game->currentFrame) {
            DrawText(TextFormat("%d|%d", game->scores[i][0], game->scores[i][1]),
                x + 5, scoreY + 25, 15, DARKGREEN);
            DrawText(TextFormat("%d", game->scores[i][2]), x + 10, scoreY + 42, 15, BLUE);
        }
        DrawLine(x + 52, scoreY, x + 52, scoreY + 60, BLACK);
    }

    if (game->gameOver) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
        DrawText("GAME OVER!", GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 60, 60, WHITE);
        DrawText(TextFormat("Final Score: %d", game->totalScore),
            GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 + 20, 40, YELLOW);
        DrawText("Press R to Restart", GetScreenWidth() / 2 - 150,
            GetScreenHeight() / 2 + 80, 30, WHITE);
    } else if (!game->ballRolling) {
        DrawText("LEFT/RIGHT: Aim | SPACE: Power", 10, GetScreenHeight() - 40, 25, DARKGRAY);
    }

    EndDrawing();
}

int main(void) {
    InitWindow(0, 0, "Bowling Game");
    ToggleFullscreen();
    SetTargetFPS(60);

    LoadAssets();

    GameState game;
    InitGame(&game);

    while (!WindowShouldClose()) {
        Update(&game);
        Draw(&game);
    }

    UnloadAssets();
    CloseWindow();

    return 0;
}
