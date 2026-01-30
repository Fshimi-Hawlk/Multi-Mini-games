#include "server.h"
#include "utils/utils.h"

static ChatHistory_St g_serverHistory = {0};
static s32            g_connectedCount = 0;
static ClientInfo_St  allClients[MAXIMUM_NUMBER_OF_CLIENTS] = {0};

void broadcastMessage(const Message_St* msg, s32 senderSocketFd) {
    char serialized[2048];
    u64 len = serializeMessage(msg, serialized);
    u16 netLen = htons((u16)len);

    log_debug("Broadcasting %zu-byte payload to %d other client(s)",
              len, g_connectedCount - 1);

    print_stackrace(1);

    for (s32 i = 0; i < g_connectedCount; ++i) {
        s32 targetFd = allClients[i].socketFd;
        if (targetFd <= 0 || targetFd == senderSocketFd) continue;

        if (sendAllBytes(targetFd, &netLen, 2) < 0 ||
            sendAllBytes(targetFd, serialized, len) < 0
        ) {
            log_warn("client fd %d disappeared during broadcast", targetFd);
            close(targetFd);
            allClients[i].socketFd = -1;
            allClients[i].valid    = false;
            continue;
        }

        log_debug("-> sent to fd %d", targetFd);
    }
}

void unicastMessage(const Message_St* msg, s32 *targetFd) {
    if (targetFd == NULL || *targetFd <= 0) return;

    char serialized[2048];
    u64 len = serializeMessage(msg, serialized);
    u16 netLen = htons((u16)len);

    log_debug("Broadcasting %zu-byte payload to %d", len, *targetFd);

    if (sendAllBytes(*targetFd, &netLen, 2) < 0) {
        log_info("client fd %d disappeared during unicast", *targetFd);
        *targetFd = -1;
        return;
    }

    if (sendAllBytes(*targetFd, serialized, len) < 0) {
        log_info("client fd %d disappeared during unicast", *targetFd);
        *targetFd = -1;
        return;
    }
}

void broadcastUserCount(void) {
    Message_St msg = { .type = MSG_USER_COUNT };
    snprintf(msg.text, sizeof(msg.text), "%d", g_connectedCount);
    broadcastMessage(&msg, 0);
}

void sendHistoryToClient(ClientInfo_St* client) {
    s32 start = (g_serverHistory.count < MAX_HISTORY) ? 0 : g_serverHistory.head;
    for (s32 i = 0; i < g_serverHistory.count; ++i) {
        Message_St* msg = &g_serverHistory.messages[(start + i) % MAX_HISTORY];

        unicastMessage(msg, &client->socketFd);
    }
}

ClientInfo_St* findClientByUsername(const char *username) {
    for (s32 i = 0; i < g_connectedCount; ++i) {
        if (strcmp(allClients[i].nickname, username) == 0) {
            return &allClients[i];
        }
    }

    return NULL;
}

void handleCommand(ClientInfo_St* client, Message_St* msg) {
    if (strcmp(msg->text, "/list") == 0) {
        Message_St resp = { .type = MSG_COMMAND_RESPONSE, .sender = "Server" };
        char list[1024] = "Connected users:\r\n";
        for (s32 i = 0; i < g_connectedCount; ++i) {
            strcat(list, allClients[i].nickname);
            strcat(list, "\r\n");
        }
        
        strcpy(resp.text, list);

        unicastMessage(&resp, &client->socketFd);
    } else if (strcmp(msg->text, "/quit") == 0) {
        Message_St resp = { .type = MSG_COMMAND_RESPONSE, .sender = "Server" };
        
        strcpy(resp.text, "Good Bye ...- Socket Closed\r\n");
        unicastMessage(&resp, &client->socketFd);

        log_info("Client fd = %d disconnected", client->socketFd);
        close(client->socketFd);

        client->socketFd = -1;
        client->valid    = false;
        
        g_connectedCount--;
        broadcastUserCount();
    } else if (strcmp(msg->text, "/help") == 0) {
        Message_St resp = { .type = MSG_COMMAND_RESPONSE, .sender = "Server" };
        
        const char buf[1024] = \
            "Availble Commands:\r\n"
            "    - \"/help\": get the list of available commands\r\n"
            "    - \"/list\": get the list of tje connected users\r\n"
            "    - \"/whisper [username]\": toggle private message toward `username`\r\n"
            "    - \"/quit\": trigger a disconnection of the user\r\n";

        strcpy(resp.text, buf);
        unicastMessage(&resp, &client->socketFd);
    }
}

s32 startServerOnInterface(const char *bindIp) {
    s32 listenSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocketFd < 0) { perror("socket() failed"); return -1; }

    s32 reuseAddressOption = 1;
    setsockopt(listenSocketFd, SOL_SOCKET, SO_REUSEADDR,
               &reuseAddressOption, sizeof(reuseAddressOption));

    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port   = htons(SERVER_PORT);
    inet_pton(AF_INET, bindIp, &serverAddress.sin_addr);

    if (bind(listenSocketFd, (struct sockaddr *)&serverAddress,
             sizeof(serverAddress)) < 0) {
        perror("bind() failed"); close(listenSocketFd); return -1;
    }

    if (listen(listenSocketFd, SOMAXCONN) < 0) {
        perror("listen() failed"); close(listenSocketFd); return -1;
    }

    if (makeSocketNonBlocking(listenSocketFd) < 0) {
        close(listenSocketFd); return -1;
    }

    setLogOpts(.hideLineId = true);
    log_info("Server is listening on port %u", SERVER_PORT);

    fd_set readfds;
    s32 maxFd = listenSocketFd;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(listenSocketFd, &readfds);

        for (s32 i = 0; i < g_connectedCount; ++i) {
            s32 fd = allClients[i].socketFd;
            if (fd > 0) FD_SET(fd, &readfds);
        }

        if (select(maxFd + 1, &readfds, NULL, NULL, NULL) < 0) { 
            if (errno == EINTR) continue;

            perror("select()");
            break;
        }

        /* ---- accept new client ---- */
        if (FD_ISSET(listenSocketFd, &readfds)) {
            struct sockaddr_in clientAddr;
            socklen len = sizeof(clientAddr);
            s32 newFd = accept(listenSocketFd, (struct sockaddr *)&clientAddr, &len);
            if (newFd >= 0) {
                if (makeSocketNonBlocking(newFd) < 0) { close(newFd); continue; }

                if (g_connectedCount >= MAXIMUM_NUMBER_OF_CLIENTS) {
                    const char *msg = "Server full\r\n";
                    u16 netLen = htons((u16)strlen(msg));
                    write(newFd, &netLen, 2);
                    write(newFd, msg, strlen(msg));
                    close(newFd);
                } else {
                    allClients[g_connectedCount] = (ClientInfo_St){
                        .socketFd               = newFd,
                        .receiveBufferUsedBytes = 0,
                        .expectedPayloadLength  = 0,
                        .valid                  = true,
                    };

                    strcpy(allClients[g_connectedCount].nickname, "Anonymous"); // will be updated on first message
                    g_connectedCount++;

                    if (newFd > maxFd) maxFd = newFd;
                    setLogOpts(.hideLineId = true);
                    log_info("New client connected, fd %d (total=%d)", newFd, g_connectedCount);

                    sendHistoryToClient(&allClients[g_connectedCount]);
                    broadcastUserCount();
                }
            }
        }

        /* ---- read from ready clients ---- */
        for (s32 i = 0; i < g_connectedCount; ++i) {
            ClientInfo_St* c = &allClients[i];
            s32 fd = c->socketFd;
            if (fd <= 0 || !FD_ISSET(fd, &readfds)) continue;

            ssize n = read(fd, c->receiveBuffer + c->receiveBufferUsedBytes, RECEIVE_BUFFER_SIZE - c->receiveBufferUsedBytes);
            if (n <= 0) {
                if (n < 0 && errno == EAGAIN) continue;

                log_info("Client fd = %d disconnected", fd);
                close(fd);

                c->socketFd = -1;
                c->valid    = false;
                
                g_connectedCount--;
                broadcastUserCount();

                continue;
            }

            c->receiveBufferUsedBytes += (u64)n;

            u64 inputPos = 0;
            while (inputPos + 2 <= c->receiveBufferUsedBytes) {
                if (c->expectedPayloadLength == 0) {
                    u16 netLen;
                    memcpy(&netLen, c->receiveBuffer + inputPos, 2);
                    c->expectedPayloadLength = ntohs(netLen);
                    inputPos += 2;

                    if (c->expectedPayloadLength > MAXIMUM_MESSAGE_PAYLOAD_SIZE) {
                        log_warn("fd %d payload too large (%zu)", fd, c->expectedPayloadLength);
                        close(fd);
                        
                        c->socketFd = -1;
                        c->valid = false;

                        g_connectedCount--;
                        broadcastUserCount();
                        break;
                    }
                }

                u64 needed = (u64)c->expectedPayloadLength;
                if (inputPos + needed > c->receiveBufferUsedBytes) break;

                char payload[needed + 1];
                memcpy(payload, c->receiveBuffer + inputPos, needed);
                payload[needed] = '\0';

                Message_St msg;
                deserializeMessage(payload, &msg);

                // Update nickname on first message if needed
                if (allClients[i].nickname[0] == '\0' || strcmp(allClients[i].nickname, "Anonymous") == 0) {
                    strcpy(allClients[i].nickname, msg.sender);
                }

                if (msg.type == MSG_COMMAND) {
                    handleCommand(c, &msg);
                } else if (msg.type == MSG_WHISPER) {
                    ClientInfo_St* target = findClientByUsername(msg.target);
                    if (target && target->valid) {
                        unicastMessage(&msg, &target->socketFd);
                        unicastMessage(&msg, &fd);  // echo back to sender
                    } else {
                        Message_St resp = { .type = MSG_COMMAND_RESPONSE, .sender = "Server" };
                        strcpy(resp.text, "User not found");
                        unicastMessage(&resp, &fd);
                    }
                } else { // MSG_BROADCAST
                    broadcastMessage(&msg, fd);
                    addToHistory(&g_serverHistory, &msg);
                }

                inputPos += needed;
                c->expectedPayloadLength = 0;
            }

            if (inputPos > 0 && inputPos < c->receiveBufferUsedBytes) {
                memmove(c->receiveBuffer, c->receiveBuffer + inputPos, c->receiveBufferUsedBytes - inputPos);
            }

            c->receiveBufferUsedBytes -= inputPos;
        }

        // Compact client array (remove dead entries)
        s32 writeIdx = 0;
        for (s32 readIdx = 0; readIdx < g_connectedCount; ++readIdx) {
            if (allClients[readIdx].valid && allClients[readIdx].socketFd > 0) {
                if (writeIdx != readIdx) {
                    allClients[writeIdx] = allClients[readIdx];
                }
                writeIdx++;
            }
        }

        g_connectedCount = writeIdx;

        // Update maxFd
        maxFd = listenSocketFd;
        for (s32 i = 0; i < g_connectedCount; ++i) {
            if (allClients[i].socketFd > maxFd) {
                maxFd = allClients[i].socketFd;
            }
        }
    }

    close(listenSocketFd);
    return 0;
}