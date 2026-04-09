
/**
 * @file utils.c
 * @author Fshimi Hawlk
 * @date 2026-03-18
 * @brief Source file for utility functions.
 */

#include "utils/utils.h"

#include <termios.h>

/**
 * @brief Logs the hexadecimal representation of a buffer.
 * 
 * @param prefix The prefix to log before the hex dump.
 * @param data The data buffer to log.
 * @param len The length of the data buffer.
 */

void logHex(const char *prefix, const void *data, u64 len) {
    const u8 *p = (const u8 *)data;
    char line[128] = {0};
    u64 pos = 0;

    logger_log("%s (%zu bytes):", prefix, len);
    for (u64 i = 0; i < len; ++i) {
        pos += snprintf(line + pos, sizeof(line) - pos, "%02X ", p[i]);
        if ((i + 1) % 16 == 0 || i + 1 == len) {
            logger_log("%s", line);
            pos = 0;
            memset(line, 0, sizeof(line));
        }
    }
}

// double getTimeSec(void) {
// struct timeval tv;
// gettimeofday(&tv, NULL);
// return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
// }



/**
 * @brief Generates a random integer between min and max (inclusive).
 * 
 * @param min The minimum value.
 * @param max The maximum value.
 * @return A random integer between min and max.
 */
u64 randInt(u64 min, u64 max) {
    return min + (rand() % (max - min + 1));
}


/**
 * @brief Makes a socket non-blocking.
 * 
 * @param socketFd The file descriptor of the socket.
 * @return 0 on success, -1 on failure.
 */
int makeSocketNonBlocking(int socketFd) {
    int currentFlags = fcntl(socketFd, F_GETFL, 0);
    if (currentFlags == -1) {
        perror("fcntl(F_GETFL)");
        return -1;
    }

    if (fcntl(socketFd, F_SETFL, currentFlags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL O_NONBLOCK)");
        return -1;
    }

    return 0;
}

/**
 * @brief Serializes a message into a buffer.
 * 
 * @param msg The message to serialize.
 * @param buf The buffer to write the serialized message into.
 * @return The number of bytes written to the buffer.
 */
u64 serializeMessage(Message_St* msg, char *buf) {
    u64 offset = 0;
    memcpy(buf + offset, &msg->type, sizeof(MessageType_Et)); 
    offset += sizeof(MessageType_Et);

    strcpy(buf + offset, msg->sender); 
    offset += strlen(msg->sender) + 1;

    strcpy(buf + offset, msg->target); 
    offset += strlen(msg->target) + 1;

    strcpy(buf + offset, msg->text); 
    offset += strlen(msg->text) + 1;

    return offset;
}

/**
 * @brief Deserializes a message from a buffer.
 * 
 * @param buf The buffer containing the serialized message.
 * @param msg The message structure to populate with the deserialized data.
 */
void deserializeMessage(const char *buf, Message_St* msg) {
    u64 offset = 0;
    memcpy(&msg->type, buf + offset, sizeof(MessageType_Et));
    offset += sizeof(MessageType_Et);

    strncpy(msg->sender, buf + offset, sizeof(msg->sender) - 1);
    msg->sender[sizeof(msg->sender) - 1] = '\0';
    offset += strlen(buf + offset) + 1;

    strncpy(msg->target, buf + offset, sizeof(msg->target) - 1);
    msg->target[sizeof(msg->target) - 1] = '\0';
    offset += strlen(buf + offset) + 1;

    strncpy(msg->text, buf + offset, sizeof(msg->text) - 1);
    msg->text[sizeof(msg->text) - 1] = '\0';
}



/**
 * @brief Sends all bytes from a buffer to a destination socket.
 * 
 * @param destinationFd The file descriptor of the destination socket.
 * @param dataPointer The pointer to the data buffer.
 * @param dataLength The length of the data buffer.
 * @return 0 on success, -1 on failure.
 */
int sendAllBytes(int destinationFd, const void *dataPointer, u64 dataLength) {
    if (destinationFd < 0) return -1;

    const char *buffer = (const char *)dataPointer;
    u64 remaining = dataLength;

    while (remaining > 0) {
        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(destinationFd, &writefds);

        struct timeval tv = {0, 0};  // non-blocking check

        int ready = select(destinationFd + 1, NULL, &writefds, NULL, &tv);
        if (ready < 0) {
            if (errno == EINTR) continue;
            log_warn("select() error in sendAllBytes: %d", errno);
            return -1;
        }

        if (ready == 0) {
            // Should not happen with timeout 0, but if it does → treat as dead
            log_warn("select() timeout on fd %d — treating as disconnected", destinationFd);
            return -1;
        }

        // select says it's writable → safe to send
        s64 sent = send(destinationFd, buffer, remaining, MSG_DONTWAIT);
        if (sent <= 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) continue;
            // Any other error → peer gone
            
            setLogOpts(.hideLineId = true, .enableTrace = true);
            log_warn("Couldn't reach fd (%u)", destinationFd);
            return -1;
        }

        buffer += sent;
        remaining -= sent;
    }

    return 0;
}


/**
 * @brief Checks if the local server is running by attempting to connect to it.
 * 
 * @return true if the server is running, false otherwise.
 */
bool isLocalServerRunning(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return false;

    // Make socket non-blocking
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    int result = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    
    if (result == 0) {
        // Connected immediately → server is running
        close(fd);
        return true;
    }

    if (errno == EINPROGRESS) {
        // Connection in progress → server is listening
        close(fd);
        return true;
    }

    // Any other error → no server
    close(fd);
    return false;
}

/**
 * @brief Adds a message to the chat history.
 * 
 * @param history The chat history structure to update.
 * @param msg The message to add.
 */
void addToHistory(ChatHistory_St* history, Message_St* msg) {
    history->messages[history->head] = *msg;
    history->head = (history->head + 1) % MAX_HISTORY;
    if (history->count < MAX_HISTORY) 
        history->count++;
}

static struct termios orig_termios;



/**
 * @brief Disables raw mode for the terminal.
 */
void disableRawMode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/**
 * @brief Enables raw mode for the terminal.
 */
void enableRawMode(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


/**
 * @brief Reads a key from the terminal without blocking.
 * 
 * @return The key code, or KEY_NONE if no key was pressed.
 */
int readKey(void) {
    char c = '\0';
    s64 nread = read(STDIN_FILENO, &c, 1);
    if (nread <= 0) return KEY_NONE;

    if (c == '\033') {
        char seq[3] = {0};
        // Non-blocking peek for escape sequences
        struct timeval tv = {0, 0};
        fd_set fds;
        FD_ZERO(&fds); FD_SET(STDIN_FILENO, &fds);

        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) <= 0) return KEY_ESC;
        if (read(STDIN_FILENO, seq, 1) != 1) return KEY_ESC;
        if (read(STDIN_FILENO, seq+1, 1) != 1) return KEY_ESC;

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
                case 'H': return KEY_UP;    // Home key sometimes
                case 'F': return KEY_DOWN;  // End key
            }
        }
        return KEY_ESC;
    }

    return (unsigned char)c;
}

/**
 * @brief Gets the current cursor position in the terminal.
 * 
 * @param row Pointer to store the row position.
 * @param col Pointer to store the column position.
 * @return true if the cursor position was successfully retrieved, false otherwise.
 */
bool getCursorPosition(int *row, int *col) {
    struct termios oldt, newt;
    char buf[32];
    int i = 0;
    unsigned int timeout = 0;

    // Only proceed if stdout is a terminal
    if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO))
        return false;

    // Save terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // Raw input, no echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Request cursor position
    if (write(STDOUT_FILENO, "\033[6n", 4) != 4) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return false;
    }

    // Read response with timeout (max ~200ms)
    while (timeout++ < 2000) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        struct timeval tv = {0, 100};  // 100us
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) <= 0)
            continue;

        int n = read(STDIN_FILENO, buf + i, 1);
        if (n <= 0) break;

        if (buf[i] == 'R') {
            buf[i + 1] = '\0';
            break;
        }
        if (++i >= (int)sizeof(buf) - 1)
            break;
    }

    // Restore terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    // Parse \033[row;colR
    int r = 0, c = 0;
    if (sscanf(buf, "\033[%d;%dR", &r, &c) == 2) {
        if (row) *row = r;
        if (col) *col = c;
        return true;
    }

    return false;
}
