

/**
 * @file auth.c
 * @author Fshimi Hawlk
 * @date 2026-03-18
 * @brief Source file for authentication functionality.
 */

// auth.c
#include "auth.h"
#include "database.h"
#include "utils/utils.h"
#include "utils/globals.h"


/**
 * @brief Draws the authentication screen with the given parameters.
 * 
 * @param username The username entered by the user.
 * @param passwordDots The password entered by the user, displayed as dots.
 * @param hidePass Whether to hide the password dots or show the actual password.
 * @param selected The currently selected option (0: username, 1: password, 2: action).
 * @param error An error message to display, or NULL if there is none.
 */
static void drawAuthScreen(const char *username, const char *passwordDots, bool hidePass, int selected, const char *error) {
    CRP(1, 1); EDP(2); EDP(3);
    printf(COLOR_GREEN "=== Multiplayer Chat ===" FGRColor "\n\r");

    printf("  Username:\r\n");
    printf("   > %s%s\r\n\r\n", username, (selected == 0) ? "_" : " ");

    printf("  Password: %s%s" "\033[0m" "\r\n", selected == 1 ? "\033[38;2;255;255;0m" : "", hidePass ? "——" : "👁");
    printf("   > %s%s\r\n\r\n", passwordDots, (selected == 2) ? "_" : " ");

    if (selected == 3) printf(COLOR_BRIGHTGREEN "→ register" COLOR_RESET "  |  login\r\n");
    else if (selected == 4) printf("  register  | " COLOR_BRIGHTGREEN "→ login" COLOR_RESET "\r\n");
    else printf("  register  |  login\r\n");

    if (error) printf("\r\n  " COLOR_RED "%s" COLOR_RESET "\r\n", error);
    printf("\r\n  ↑↓ Move • Enter Select • Tab Toggle Hide\r\n");
    fflush(stdout);
}


/**
 * @brief Runs the authentication screen, allowing the user to log in or register.
 */
void runAuth(void) {
    if (!db_init()) {
        log_fatal("Failed to initialize database");
        exit(1);
    }

    enableRawMode();

    char username[64] = {0};
    char password[64] = {0};
    char passwordDots[64] = {0};
    bool hidePass = true;
    int selected = 0;
    const char *error = NULL;

    while (1) {
        memset(passwordDots, '*', strlen(password));
        passwordDots[strlen(password)] = '\0';

        drawAuthScreen(username, passwordDots[0] ? (hidePass ? passwordDots : password) : "(empty)", hidePass, selected, error);
        error = NULL;

        int key = readKey();

        if (key == KEY_UP && selected > 0) selected--;
        if (key == KEY_DOWN && selected < 4) selected++;

        if (key == KEY_RETURN) {
            if (selected == 3 || selected == 4) {
                bool isRegister = (selected == 3);
                if (strlen(username) < 3) error = "Username too short";
                else if (strlen(password) < 4) error = "Password too short";
                else if (isRegister) {
                    if (db_registerUser(username, password)) {
                        strncpy(g_currentUser.nickname, username, sizeof(g_currentUser.nickname)-1);
                        g_currentUser.nickname[sizeof(g_currentUser.nickname)-1] = '\0';
                        break;
                    } else {
                        error = "Username already taken";
                    }
                } else {
                    if (db_userExists(username)) {
                        if (db_checkPassword(username, password)) {
                            strncpy(g_currentUser.nickname, username, sizeof(g_currentUser.nickname)-1);
                        g_currentUser.nickname[sizeof(g_currentUser.nickname)-1] = '\0';
                            break;
                        } else error = "Wrong password";
                    } else error = "User does not exist";
                }
            }
        }
        else if (key == '\t' && selected == 1) hidePass = !hidePass;
        else if (key == KEY_BACKSPACE || key == 127) {
            if (selected == 0 && strlen(username)) username[strlen(username)-1] = 0;
            if (selected == 2 && strlen(password)) password[strlen(password)-1] = 0;
        }
        else if (key >= 32 && key <= 126) {
            if (selected == 0 && strlen(username) < 30) {
                username[strlen(username)] = key;
            }
            if (selected == 2 && strlen(password) < 30) {
                password[strlen(password)] = key;
            }
        }
    }

    disableRawMode();
    log_info("Logged in as %s", g_currentUser.nickname);
}