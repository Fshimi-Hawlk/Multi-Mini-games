#include "chat.h"
#include "utils/globals.h"
#include "utils/userTypes.h.h"
#include "utils/utils.h"

static ChatHistory_St g_clientHistory = {0};
static int            g_connectedCount = 0;

static void printHistory(void) {
    int start = (g_clientHistory.count < MAX_HISTORY) ? 0 : g_clientHistory.head;
    for (int i = 0; i < g_clientHistory.count; ++i) {
        Message_St msg = g_clientHistory.messages[(start + i) % MAX_HISTORY];
        printf("[%s]\r\n%s\r\n", msg.sender, msg.text);
    }
}

int startChatClient(const char *serverIp) {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) { 
        log_fatal("socket() failed"); return -1; 
    }

    struct sockaddr_in srv = {0};
    srv.sin_family = AF_INET;
    srv.sin_port   = htons(SERVER_PORT);
    inet_pton(AF_INET, serverIp, &srv.sin_addr);

    if (connect(serverFd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        log_error("Cannot connect to %s", serverIp);
        close(serverFd);
        sleep(2);
        return -1;
    }

    enableRawMode();

    makeSocketNonBlocking(serverFd);

    // log_info("Connected! Type and press Enter. ESC to quit.");

    char inputBuffer[1024] = {0};
    u64 inputPos = 0;
    strcpy(gargetUser, "");  // everyone

#define NL "\r\n"

    CRP(1, 1); EDP(2); EDP(3);
    printf("=== Online Chat: 0 Connected ===" NL NL NL);
    printf("=== User: %s to %s ===" NL, g_currentUser.nickname, gargetUser[0] ? gargetUser : "everyone");
    printf("> %s", inputBuffer);
    fflush(stdout);
    
    while (1) {
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(serverFd, &readFds);
        FD_SET(0, &readFds);

        struct timeval tv = {0, 100000}; // 100 ms timeout
        if (select(serverFd + 1, &readFds, NULL, NULL, &tv) < 0) continue;

        // 1. Did we receive something from the server?
        if (FD_ISSET(serverFd, &readFds)) {
            char netBuf[2048];
            ssize n = read(serverFd, netBuf, sizeof(netBuf));
            if (n <= 0) break;

            // We expect length prefix
            if (n >= 2) {
                u16 payloadLen = ntohs(*(u16*)netBuf);
                if (payloadLen <= n - 2) {
                    Message_St msg;
                    deserializeMessage(netBuf + 2, &msg);

                    if (msg.type == MSG_COMMAND_RESPONSE) {
                        int x, y;
                        getCursorPosition(&x, &y);

                        CRU(2); EDP(0);
                        printf("\r-- Only you can see that --" NL NL);
                        printf("[From %s]:" NL "%s" NL, msg.sender, msg.text);
                        printf("-- Only you can see that --" NL NL NL);
                        printf("=== User: %s to %s ===" NL, g_currentUser.nickname, gargetUser[0] ? gargetUser : "everyone");
                        printf("> %s", inputBuffer);
                        fflush(stdout);

                    } else if (msg.type == MSG_USER_COUNT) {
                        g_connectedCount = atoi(msg.text);

                        int x, y;
                        getCursorPosition(&x, &y);

                        CRP(1, 1); ELN(2);
                        printf("=== Online Chat: %u Connected ===", g_connectedCount);
                        
                        printf("\033[%d;%dH", x, y);
                        fflush(stdout);
                    } else {
                        addToHistory(&g_clientHistory, &msg);

                        int x, y;
                        getCursorPosition(&x, &y);

                        CRU(2); EDP(0);
                        printf("\r%s%s> %s" NL NL, msg.sender, ((msg.type == MSG_WHISPER) ? " whispered to you" : ""), msg.text);
                        printf(NL "=== User: %s to %s ===" NL, g_currentUser.nickname, gargetUser[0] ? gargetUser : "everyone");
                        printf("> %s", inputBuffer);
                        fflush(stdout);
                        fflush(stdout);
                    }
                }
            }
        }

        // 2. Did the user type something?
        if (FD_ISSET(0, &readFds)) {
            int key = readKey();
            if (key == KEY_RETURN) {
                if (inputPos > 0) {
                    Message_St msg = {0};
                    
                    strcpy(msg.sender, g_currentUser.nickname);
                    strcpy(msg.target, gargetUser);
                    strcpy(msg.text, inputBuffer);

                    msg.type = gargetUser[0] ? MSG_WHISPER : MSG_BROADCAST;
                    msg.type = (msg.text[0] == '/') ? MSG_COMMAND : msg.type;

                    if (strncmp(msg.text, "/whisper ", 9) == 0) {
                        // Parse /whisper user
                        char *target = msg.text + 9;
                        strcpy(gargetUser, target);
                        inputPos = 0;
                        inputBuffer[0] = '\0';

                        CRU(1); ELN(2);
                        printf("\r=== User: %s to %s ===" NL, g_currentUser.nickname, gargetUser[0] ? gargetUser : "everyone");
                    } else {
                        char buf[2048];
                        u64 len = serializeMessage(&msg, buf);
                        u16 netLen = htons((u16)len);
                        write(serverFd, &netLen, 2);
                        write(serverFd, buf, len);
    
                        inputPos = 0;
                        inputBuffer[0] = '\0';
    
                        if (gargetUser[0]) gargetUser[0] = '\0';
                    }
                }
            } else if (key == KEY_BACKSPACE && inputPos > 0) {
                inputPos--;
                inputBuffer[inputPos] = '\0';
            } else if (key >= 32 && key <= 126 && inputPos < sizeof(inputBuffer)-1) {
                inputBuffer[inputPos++] = (char)key;
                inputBuffer[inputPos] = '\0';
            } else if (key == KEY_ESC) break;  // ESC

            // Redraw input line
            printf("\r\033[K> %s", inputBuffer);
            fflush(stdout);
        }
    }

    close(serverFd);
    disableRawMode();

    return 0;
}