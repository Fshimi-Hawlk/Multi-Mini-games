#include "menu.h"
#include "server.h"
#include "chat.h"

#include "utils/utils.h"
#include "utils/globals.h"

static void drawHeader(void) {
    CRP(1, 1); EDP(2); EDP(3);
    printf(COLOR_GREEN "=== Multiplayer Chat ===" FGRColor "\n\r");
    printf(COLOR_SKYBLUE "Username: " COLOR_PALE_YELLOW "%s" FGRColor "\n\r\n\r", g_currentUser.nickname);
    fflush(stdout);
}

static void drawMenu(const char *title, const char **options, int count, int selected, const bool *enabled) {
    drawHeader();
    printf(COLOR_YELLOW "  %s\r\n" COLOR_RESET, title);
    printf("  ────────────────────────────────────────\r\n");

    for (int i = 0; i < count; ++i) {
        if (!enabled[i]) {
            printf("  " COLOR_GRAY "  • %s (unavailable)" COLOR_RESET "\r\n", options[i]);
        }
        else if (i == selected) {
            printf("    " COLOR_BRIGHTGREEN "→ %s" COLOR_RESET "\r\n", options[i]);
        }
        else {
            printf("    • %s\r\n", options[i]);
        }
    }

    printf("\r\n  ↑↓ to move • Enter to select • ESC to go back\r\n");
    fflush(stdout);
}

static int selectOption(const char *title, const char **options, int count, const bool *enabled) {
    int selected = 0;

    // Find first enabled item
    while (selected < count && !enabled[selected]) selected++;
    if (selected >= count) selected = 0;

    drawMenu(title, options, count, selected, enabled);

    int key;

    while (1) {
        switch ((key = readKey())) {
            case KEY_UP: {
                do {
                    if (selected <= 0) selected = count - 1;
                    else selected--;
                } while (!enabled[selected]);
            } break;
    
            case KEY_DOWN: {
                do {
                    if (selected >= count - 1) selected = 0;
                    else selected++;
                } while (!enabled[selected]);
            } break;
    
            case KEY_RETURN: {
                return selected;
            } break;
    
            case KEY_ESC: {
                return -1;
            } break;
    
            default: {
                log_debug("key pressed: (%u) '%c'", key, key);
            }
        }

        drawMenu(title, options, count, selected, enabled);
    }
}

void runMainMenu(void) {
    enableRawMode();

    while (1) {
        const char *mainOptions[] = { "Launch a server", "Join a server" };
        int choice = selectOption("Main Menu", mainOptions, 2, (bool[]){true, true});
        if (choice < 0) {
            disableRawMode();
            printf(COLOR_RESET);
            EDP(2); CRP(1, 1);
            printf("Good Bye !\n");
            exit(0);
        }

        if (choice == 0) {
            // ── Launch server ──
            const char *hostModes[] = {
                "Loopback only (127.0.0.1)",
                "Local network (LAN)",
                "Internet (requires port forwarding)"
            };
            int mode = selectOption("Host Mode", hostModes, 3, (bool[]){true,true,true});
            if (mode < 0) continue;

            disableRawMode();
            printf(COLOR_RESET);

            const char *bindIp = (mode == 0) ? "127.0.0.1" : "0.0.0.0";
            setLogOpts(.hideLineId = true);
            log_info("Starting server on %s...", (mode == 0) ? "loopback" : "all interfaces");

            startServerOnInterface(bindIp);
            return;
        } else {
            // ── Join server ──
            bool localAvailable = isLocalServerRunning();
            const char *joinOptions[] = {
                "Connect to local server (127.0.0.1)",
                "Enter server IP manually"
            };
            bool enabled[] = { localAvailable, true };
            int joinChoice = selectOption("Join Server", joinOptions, 2, enabled);
            if (joinChoice < 0) continue;

            char *targetIp;
            if (joinChoice == 0) {
                targetIp = "127.0.0.1";
            } else {
                disableRawMode();
                CRP(1, 1); EDP(2); EDP(3);

                char input[64] = {0};

                printf("Enter server IP: ");
                fflush(stdout);

                if (!fgets(input, sizeof(input), stdin)) exit(0);
                input[strcspn(input, "\n")] = '\0';

                if (input[0] == '\0') { 
                    enableRawMode(); 
                    continue; 
                }

                targetIp = temp_strdup(input);  // simple, we exit soon anyway
                enableRawMode();
            }

            disableRawMode();
            printf(COLOR_RESET);

            log_info("Connecting to %s...", targetIp);
            
            startChatClient(targetIp);
            return;
        }
    }
}