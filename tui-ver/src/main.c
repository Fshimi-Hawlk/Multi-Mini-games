

/**
 * @file main.c
 * @author Fshimi Hawlk
 * @date 2026-03-18
 * @brief Source file for main functionality.
 */

#include "menu.h"
#include "auth.h"
#include "chat.h"
#include "server.h"

#include <signal.h>

/**
 * @brief Thread function for starting the server on the given interface.
 * 
 * @param arg The IP address of the interface to bind to.
 * @return NULL on success.
 */
static void *serverThread(void *arg) {
    const char *bindIp = (const char *)arg;
    startServerOnInterface(bindIp);
    return NULL;
}

/**
 * @brief Hosts and joins a chat session on the given interface.
 * 
 * @param bindIp The IP address of the interface to bind to.
 */
void hostAndJoin(const char *bindIp) {
    pthread_t tid;
    pthread_create(&tid, NULL, serverThread, (void*)bindIp);

    // Give server time to start
    usleep(500000);

    startChatClient(bindIp);
}


/**
 * @brief Main entry point for the application.
 * 
 * @return int 0 on success, EXIT_FAILURE on failure.
 */
int main(void) {
    if (init_logger() < 0) return  EXIT_FAILURE;

    signal(SIGPIPE, SIG_IGN);

    runAuth();
    runMainMenu();

    return EXIT_SUCCESS;
}