#include "menu.h"
#include "auth.h"
#include "chat.h"
#include "server.h"

#include <signal.h>

static void *serverThread(void *arg) {
    const char *bindIp = (const char *)arg;
    startServerOnInterface(bindIp);
    return NULL;
}

void hostAndJoin(const char *bindIp) {
    pthread_t tid;
    pthread_create(&tid, NULL, serverThread, (void*)bindIp);

    // Give server time to start
    usleep(500000);

    startChatClient(bindIp);
}

int main(void) {
    if (init_logger() < 0) return  EXIT_FAILURE;

    signal(SIGPIPE, SIG_IGN);

    runAuth();
    runMainMenu();

    return EXIT_SUCCESS;
}