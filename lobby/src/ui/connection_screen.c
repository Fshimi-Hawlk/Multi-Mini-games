/**
 * @file connection_screen.c
 * @brief Gestion de la scène "Pré-Lobby" pour la saisie de l'IPv4.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-05
 */

#include "raylib.h"
#include "ui/input_button.h"
#include <stdio.h>

// Définition de l'état local de la scène
static IaC_button ipInput;
static IaC_button connectButton;
static char ipBuffer[IP_MAX_LENGTH + 1] = {0}; // Tampon sécurisé pour l'IPv4 (+1 pour le terminateur '\0')
static int letterCount = 0;

/**
 * @brief Alloue et positionne les éléments UI pour l'écran de connexion.
 * À appeler une seule fois lors de la transition vers cette scène.
 */
void InitConnectionScreen(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Zone de texte pour l'adresse IP
    ipInput = InitIaCElement(
        (float)(screenWidth / 2 - 150), 
        (float)(screenHeight / 2 - 60), 
        300.0f, 40.0f, 
        "Entrez l'IP (ex: 127.0.0.1)", 
        LIGHTGRAY
    );

    // Bouton de validation (Verrouillé par défaut via la logique d'Update)
    connectButton = InitIaCElement(
        (float)(screenWidth / 2 - 100), 
        (float)(screenHeight / 2 + 10), 
        200.0f, 40.0f, 
        "Se Connecter", 
        DARKGRAY
    );
}

/**
 * @brief Boucle logique de l'écran de connexion.
 * @return true si la connexion est déclenchée (Bouton cliqué + IP valide)
 */
bool UpdateConnectionScreen(void) {
    // 1. Écoute du clavier et mise à jour de la validité interne (ipInput.isIPValid)
    UpdateIPInput(&ipInput, ipBuffer, &letterCount);

    // 2. Synchronisation du bouton : on injecte l'état de validation de la zone de texte
    bool isConnectionTriggered = UpdateConnectButton(&connectButton, ipInput.isIPValid);

    if (isConnectionTriggered) {
        // Le joueur a validé une IP considérée comme syntaxiquement valide
        // Injection de la logique réseau ici (ex: InitClient(ipBuffer))
    }

    return isConnectionTriggered;
}

/**
 * @brief Moteur de rendu appelé à chaque frame (ex: 60 FPS).
 */
void DrawConnectionScreen(void) {
    ClearBackground(RAYWHITE);

    const char* title = "CONNEXION AU SERVEUR";
    int titleWidth = MeasureText(title, 30);
    DrawText(title, GetScreenWidth() / 2 - titleWidth / 2, GetScreenHeight() / 2 - 150, 30, DARKGRAY);

    // Le moteur de rendu de la zone de texte intercepte le buffer en temps réel
    DrawIaCElement(ipInput, ipBuffer);

    // Pour le bouton, on passe un texte vide afin d'utiliser le label d'origine de la struct ("Se Connecter")
    DrawIaCElement(connectButton, "");
    
    // Feedback visuel si l'IP n'a pas la longueur minimale requise
    if (!ipInput.isIPValid && ipInput.state == STATE_ACTIVE) {
        DrawText("Format attendu : X.X.X.X", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 + 70, 15, RED);
    }
}



const char* GetEnteredIP(void) {
    return ipBuffer; // Renvoie le tampon géré par la zone de saisie
}