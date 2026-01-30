#include "client.h"
#include "utils/utils.h"

int runTestClient(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return -1; }

    struct sockaddr_in srv = {0};
    srv.sin_family = AF_INET;
    srv.sin_port   = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &srv.sin_addr);

    if (connect(fd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("connect"); close(fd); return -1;
    }

    log_info("Successfully connected to server");

    /* ---- first message ---- */
    const char *msg1 = "Hello everybody!";
    u16 len1 = htons((u16)strlen(msg1));
    write(fd, &len1, 2);
    write(fd, msg1, strlen(msg1));
    log_info("Sent: \"%s\"", msg1);

    /* give the server a moment to broadcast */
    usleep(200000);   // 0.2 s

    /* ---- second message (so you see echo even with ONE client) ---- */
    const char *msg2 = "Second message!";
    u16 len2 = htons((u16)strlen(msg2));
    write(fd, &len2, 2);
    write(fd, msg2, strlen(msg2));
    log_info("Sent: \"%s\"", msg2);

    /* ---- receive loop ---- */
    char buf[1024];
    while (1) {
        ssize n = read(fd, buf, sizeof(buf));
        if (n <= 0) break;

        /* The first 2 bytes are the length header – skip them for pretty print */
        if (n >= 2) {
            u16 payloadLen = ntohs(*(u16 *) buf);
            if (payloadLen <= n - 2) {
                buf[2 + payloadLen] = '\0';   // null-terminate for printing
                log_info("Received: \"%s\"", buf + 2);
            }
        }
        /* raw hex dump for debugging */
        logHex("raw recv", buf, (u64)n);
    }

    close(fd);
    return 0;
}