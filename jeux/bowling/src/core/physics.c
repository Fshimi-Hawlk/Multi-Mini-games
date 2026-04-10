/**
 * @file physics.c
 * @author Maxime CHAUVEAU
 * @date March 2026
 * @brief Physics – mesures officielles World Bowling / USBC (1 unité = 1 mètre).
 */

#include "core/physics.h"
#include "utils/types.h"
#include "utils/configs.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <math.h>
#include "utils/audio.h"

//  Mesures officielles 
// 1 unité = 1 mètre
#define LANE_WIDTH    1.05f   // largeur piste
#define LANE_LENGTH   18.29f  // ligne de faute → quille de tête
#define LANE_EDGE    (LANE_WIDTH / 2.0f)
#define GUTTER_WIDTH 0.23f   // largeur d'une gouttière
#define BALL_RADIUS   0.1092f // rayon max (diamètre 21.83 cm)
#define PIN_BASE_R    0.060f  // rayon base quille (diamètre 12 cm)
#define GRAVITY       9.8f
#define NUM_PINS      10
#define BALL_START_Z 4.0f

//  Positions officielles des quilles 
// Espacement centre-à-centre : 30,48 cm
// Quille 1 (tête) à z = -18.29 m (fin de la piste)
// Rangées s'éloignent du lanceur (-Z)
//  Rang 4: 7  8  9  10
//  Rang 3:   4  5  6
//  Rang 2:     2  3
//  Rang 1:       1
#define PIN_SPACING  0.3048f
#define PIN1_Z      -18.29f

static const Vector3 PIN_POSITIONS[NUM_PINS] = {
    // Rang 1
    { 0.0f,        0.0f, PIN1_Z                    },  // 1
    // Rang 2
    {-PIN_SPACING*0.5f, 0.0f, PIN1_Z - PIN_SPACING },  // 2
    { PIN_SPACING*0.5f, 0.0f, PIN1_Z - PIN_SPACING },  // 3
    // Rang 3
    {-PIN_SPACING,      0.0f, PIN1_Z - PIN_SPACING*2}, // 4
    { 0.0f,             0.0f, PIN1_Z - PIN_SPACING*2}, // 5
    { PIN_SPACING,      0.0f, PIN1_Z - PIN_SPACING*2}, // 6
    // Rang 4
    {-PIN_SPACING*1.5f, 0.0f, PIN1_Z - PIN_SPACING*3}, // 7
    {-PIN_SPACING*0.5f, 0.0f, PIN1_Z - PIN_SPACING*3}, // 8
    { PIN_SPACING*0.5f, 0.0f, PIN1_Z - PIN_SPACING*3}, // 9
    { PIN_SPACING*1.5f, 0.0f, PIN1_Z - PIN_SPACING*3}  // 10
};

//  Centre Y de la quille debout 
// Hauteur totale 38.1 cm → centre à 19.05 cm du sol
#define PIN_CENTER_Y  0.1905f

void physics_initParticles(Particle_St* particles, int* particleCount) {
    *particleCount = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].life = 0.0f;
}

void physics_spawnParticles(Particle_St* particles, int* particleCount,
                             Vector3 position, int count, Color baseColor) {
    for (int i = 0; i < count && *particleCount < MAX_PARTICLES; i++) {
        Particle_St* p = &particles[*particleCount];
        p->position = position;
        p->velocity = (Vector3){
            (float)(GetRandomValue(-100, 100)) / 100.0f * 1.5f,
            (float)(GetRandomValue(80,  200))  / 100.0f * 2.0f,
            (float)(GetRandomValue(-100, 100)) / 100.0f * 1.5f
        };
        p->color   = baseColor;
        p->life    = (float)(GetRandomValue(400, 900)) / 1000.0f;
        p->maxLife = p->life;
        p->size    = (float)(GetRandomValue(4, 12)) / 1000.0f; // en mètres
        (*particleCount)++;
    }
}

void physics_updateParticles(Particle_St* particles, int* particleCount, float deltaTime) {
    int w = 0;
    for (int i = 0; i < *particleCount; i++) {
        Particle_St* p = &particles[i];
        p->life -= deltaTime;
        if (p->life <= 0.0f) continue;
        p->velocity.y -= GRAVITY * deltaTime;
        p->position = Vector3Add(p->position, Vector3Scale(p->velocity, deltaTime));
        if (p->position.y < 0.0f) { p->position.y = 0.0f; p->velocity.y *= -0.3f; }
        if (i != w) particles[w] = *p;
        w++;
    }
    *particleCount = w;
}

void physics_setupPins(Pin_St* pins) {
    for (int i = 0; i < NUM_PINS; i++) {
        pins[i].position        = PIN_POSITIONS[i];
        pins[i].position.y      = PIN_CENTER_Y;
        pins[i].velocity        = (Vector3){0,0,0};
        pins[i].angularVelocity = (Vector3){0,0,0};
        pins[i].rotation        = (Vector3){0,0,0};
        pins[i].isStanding      = true;
        pins[i].mass            = 1.57f; // 1.5–1.64 kg (milieu de fourchette)
        pins[i].fallTime        = 0.0f;
        pins[i].rotationAngle   = 0.0f;
        pins[i].rotationAxis    = (Vector3){1,0,0};
    }
}

void physics_resetBall(Ball_St* ball) {
    ball->position       = (Vector3){0.0f, BALL_RADIUS, BALL_START_Z}; // zone d'élan
    ball->velocity       = (Vector3){0,0,0};
    ball->spin           = (Vector3){0,0,0};
    ball->spinAmount     = 0.0f;
    ball->isRolling      = false;
    ball->visualRotation = 0.0f;
    ball->rollAxis       = (Vector3){1,0,0};
}

void physics_launchBall(Ball_St* ball, Vector3 direction, float power, float spinAmount) {
    ball->velocity   = Vector3Scale(direction, power);
    ball->spinAmount = spinAmount;
    ball->spin       = (Vector3){0, 0, spinAmount > 0 ? 1.0f : -1.0f};
    ball->isRolling  = true;

    PlaySound(sound_ballFall);
}

void physics_updateBallSpin(Ball_St* ball, float deltaTime) {
    if (!ball->isRolling) return;
    if (fabsf(ball->spinAmount) > 0.005f) {
        // Effet d'effet latéral (spin)
        ball->velocity.x += ball->spinAmount * deltaTime * 1.0f;
        ball->spinAmount  *= powf(0.995f, deltaTime * 60.0f);
        if (fabsf(ball->spinAmount) < 0.005f) ball->spinAmount = 0.0f;
    }
    float speed = Vector3Length(ball->velocity);
    if (speed > 0.05f) {
        Vector3 vn = Vector3Normalize(ball->velocity);
        Vector3 c  = (Vector3){-vn.z, 0.0f, vn.x};
        float   l  = Vector3Length(c);
        if (l > 0.01f) ball->rollAxis = Vector3Scale(c, 1.0f / l);
        ball->visualRotation += (speed * deltaTime / ball->radius) * RAD2DEG;
    }
}

void physics_checkCollisions(Ball_St* ball, Pin_St* pins,
                              Particle_St* particles, int* particleCount) {
    float minDist = ball->radius + PIN_BASE_R;

    // Balle ↔ quilles
    for (int i = 0; i < NUM_PINS; i++) {
        if (!pins[i].isStanding) continue;
        if (Vector3Distance(ball->position, pins[i].position) >= minDist) continue;

        pins[i].isStanding = false;
        pins[i].fallTime   = 0.0f;

        Vector3 impact = Vector3Normalize(Vector3Subtract(pins[i].position, ball->position));
        float   ispeed = Vector3Length(ball->velocity);
        // Impulsion proportionnelle à la vitesse d'impact
        pins[i].velocity = (Vector3){
            impact.x * ispeed * 1.6f,
            0.5f,
            impact.z * ispeed * 1.6f
        };
        // Axe de rotation : perpendiculaire à la direction d'impact
        Vector3 iH = Vector3Normalize((Vector3){impact.x, 0.0f, impact.z});
        pins[i].rotationAxis    = Vector3Normalize((Vector3){-iH.z, 0.0f, iH.x});
        float   angSpd          = 8.0f + ispeed * 2.0f;
        pins[i].angularVelocity = (Vector3){
            pins[i].rotationAxis.x * angSpd, 0.0f,
            pins[i].rotationAxis.z * angSpd
        };
        physics_spawnParticles(particles, particleCount, pins[i].position, 10,
                               (Color){255,220,180,200});

    }

    // Quille ↔ quille (réaction en chaîne)
    float pinMinDist = PIN_BASE_R * 2.2f;
    for (int i = 0; i < NUM_PINS; i++) {
        for (int j = i + 1; j < NUM_PINS; j++) {
            if (Vector3Distance(pins[i].position, pins[j].position) >= pinMinDist) continue;
            bool im = !pins[i].isStanding || Vector3Length(pins[i].velocity) > 0.05f;
            bool jm = !pins[j].isStanding || Vector3Length(pins[j].velocity) > 0.05f;
            if (im && pins[j].isStanding) {
                pins[j].isStanding = false; pins[j].fallTime = 0.0f;
                Vector3 n  = Vector3Normalize(Vector3Subtract(pins[j].position, pins[i].position));
                pins[j].velocity = (Vector3){n.x*1.8f, 0.4f, n.z*1.8f};
                Vector3 nH = Vector3Normalize((Vector3){n.x,0,n.z});
                pins[j].rotationAxis    = Vector3Normalize((Vector3){-nH.z,0,nH.x});
                pins[j].angularVelocity = (Vector3){pins[j].rotationAxis.x*5.0f, 0, pins[j].rotationAxis.z*5.0f};
                physics_spawnParticles(particles, particleCount, pins[j].position, 5,
                                       (Color){240,200,160,180});
                PlaySound(sound_pinFall);

            }
            if (jm && pins[i].isStanding) {
                pins[i].isStanding = false; pins[i].fallTime = 0.0f;
                Vector3 n  = Vector3Normalize(Vector3Subtract(pins[i].position, pins[j].position));
                pins[i].velocity = (Vector3){n.x*1.8f, 0.4f, n.z*1.8f};
                Vector3 nH = Vector3Normalize((Vector3){n.x,0,n.z});
                pins[i].rotationAxis    = Vector3Normalize((Vector3){-nH.z,0,nH.x});
                pins[i].angularVelocity = (Vector3){pins[i].rotationAxis.x*5.0f, 0, pins[i].rotationAxis.z*5.0f};
                physics_spawnParticles(particles, particleCount, pins[i].position, 5,
                                       (Color){240,200,160,180});
                PlaySound(sound_pinFall);
}
}

bool physics_hasBallReachedPins(Ball_St* ball) {
    return ball->position.z <= PIN1_Z + ball->radius;
}

    }
}

bool physics_isGutterBall(Ball_St* ball, float laneWidth, float gutterWidth) {
    float edge = LANE_EDGE - gutterWidth * 0.5f;
    return ball->position.x < -edge || ball->position.x > edge;
}

void physics_updateBall(Ball_St* ball, float deltaTime, float laneWidth,
                         float gutterWidth, bool bumpers) {
    (void)gutterWidth;
    (void)laneWidth;
    physics_updateBallSpin(ball, deltaTime);

    // Friction de roulement (coefficient réaliste sur piste huilée)
    // k ≈ 0.4 → portée max ≈ v0/k mètres (ex: 10 m/s → 25 m)
    Vector3 friction = Vector3Scale(ball->velocity, -0.4f);
    ball->velocity   = Vector3Add(ball->velocity, Vector3Scale(friction, deltaTime));

    // Invisible barrier along gutter edges - always active (prevents ball leaving lane)
    float edge = LANE_EDGE;
    if (ball->position.x < -edge || ball->position.x > edge) {
        // Only apply bumper bounce if bumpers enabled, otherwise just block
        if (bumpers) {
            ball->velocity.x *= -0.55f;
        } else {
            ball->velocity.x *= -0.3f; // Simple bounce back
        }
        ball->position.x = (ball->position.x < 0) ? -edge + ball->radius : edge - ball->radius;
    }

    ball->position = Vector3Add(ball->position, Vector3Scale(ball->velocity, deltaTime));
    if (ball->position.y < ball->radius) ball->position.y = ball->radius;
}

void physics_updatePins(Pin_St* pins, int pinCount, float deltaTime,
                         float laneWidth, float laneLength) {
    const float MAX_FALL_ANGLE = PI / 2.0f;
    for (int i = 0; i < pinCount; i++) {
        if (pins[i].isStanding) continue;
        pins[i].fallTime += deltaTime;
        pins[i].velocity.y -= GRAVITY * deltaTime;
        pins[i].velocity    = Vector3Scale(pins[i].velocity, powf(0.98f, deltaTime * 60.0f));
        pins[i].position    = Vector3Add(pins[i].position,
                                         Vector3Scale(pins[i].velocity, deltaTime));

        float angSpd = Vector3Length(pins[i].angularVelocity);
        pins[i].angularVelocity = Vector3Scale(pins[i].angularVelocity, powf(0.96f, deltaTime * 60.0f));
        pins[i].rotationAngle  += angSpd * deltaTime;
        if (pins[i].rotationAngle >= MAX_FALL_ANGLE) {
            pins[i].rotationAngle   = MAX_FALL_ANGLE;
            pins[i].angularVelocity = (Vector3){0,0,0};
        }

        float groundY = 0.04f; // quille couchée, centre à ~4 cm
        if (pins[i].position.y < groundY) {
            pins[i].position.y  = groundY;
            pins[i].velocity.y *= -0.10f;
            pins[i].velocity.x *= 0.75f;
            pins[i].velocity.z *= 0.75f;
        }

        float hw = laneWidth + 0.5f;
        if (pins[i].position.x < -hw || pins[i].position.x > hw ||
            pins[i].position.z < -(laneLength + 2.0f) || pins[i].position.z > 1.0f) {
            pins[i].velocity        = (Vector3){0,0,0};
            pins[i].angularVelocity = (Vector3){0,0,0};
        }
    }

}
