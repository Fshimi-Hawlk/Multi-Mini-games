#include "utils/utils.h"

#include <termios.h>

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
//     struct timeval tv;
//     gettimeofday(&tv, NULL);
//     return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
// }


u64 randInt(u64 min, u64 max) {
    return min + (rand() % (max - min + 1));
}

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

void deserializeMessage(const char *buf, Message_St* msg) {
    u64 offset = 0;
    memcpy(&msg->type, buf + offset, sizeof(MessageType_Et)); 
    offset += sizeof(MessageType_Et);

    strcpy(msg->sender, buf + offset); 
    offset += strlen(msg->sender) + 1;

    strcpy(msg->target, buf + offset); 
    offset += strlen(msg->target) + 1;

    strcpy(msg->text, buf + offset);
}

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

void addToHistory(ChatHistory_St* history, Message_St* msg) {
    history->messages[history->head] = *msg;
    history->head = (history->head + 1) % MAX_HISTORY;
    if (history->count < MAX_HISTORY) 
        history->count++;
}

static struct termios orig_termios;

void disableRawMode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

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
