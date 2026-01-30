#ifndef UTILS_H
#define UTILS_H

#include "utils/userTypes.h"

#pragma region Defines

#define INVALID_PTR (void *) -1

#define COLOR_RESET       FGRColor BGRColor
#define COLOR_CYAN        FGColor(0, 255, 255)
#define COLOR_YELLOW      FGColor(255, 255, 0)
#define COLOR_RED         FGColor(255, 0, 0)
#define COLOR_GRAY        FGColor(150, 150, 150)
#define COLOR_BRIGHTGREEN FGColor(0, 255, 100)

#define COLOR_GREEN       FGColor(53, 220, 118)
#define COLOR_DARKGREEN   FGColor(78, 201, 176)
#define COLOR_SKYBLUE     FGColor(156, 220, 254)
#define COLOR_PALE_YELLOW FGColor(220, 220, 170)

#pragma endregion Defines

#pragma region Macros

#define dcall log_debug("called")

#define max(a, b) (a) > (b) ? (a) : (b)
#define min(a, b) (a) < (b) ? (a) : (b)

#define signof(v) ((v) < 0 ? -1 : 1)
#define clamp(v, min, max) v = (v) < (min) ? (min) : (v) > (max) ? (max) : (v)

/* Sequence Introducer */
#define ES          "\033["

/* Cursor Up */
#define CRU(n)      printf(ES "%uA", n)

/* Cursor Down */
#define CRD(n)      printf(ES "%uB", n)

/* Cursor Forward */
#define CRF(n)      printf(ES "%uC", n)

/* Cursor Backward */
#define CRB(n)      printf(ES "%uD", n)

/* Cursor New Line */
#define CNL(n)      printf(ES "%uE", n)

/* Cursor Previous Line */
#define CPL(n)      printf(ES "%uF", n)

/* Cursor Horizontal Absolute (moves to the n-th column) */
#define CHA(n)      printf(ES "%uG", n)

/* Cursor Position (moves to n-th line and m-th column) */
#define CRP(n, m)   printf(ES "%u;%uH", n, m)

/* Erase in Display 
    0: clear from cursor to end of screen. 
    1: clear from cursor to beginning of the screen. 
    2: clear entire screen (and moves cursor to upper left). 
    3: clear entire screen and delete all lines saved in the scrollback buffer
*/
#define EDP(n)      printf(ES "%uJ", n)

/* Erase in Line 
    0: clear from cursor to the end of the line. 
    1: clear from cursor to beginning of the line. 
    2: clear entire line. 
    
    Note: Cursor position does not change.
*/
#define ELN(n)      printf(ES "%uK", n)

#define FGRColor    ES "39m"
#define BGRColor    ES "49m"

#define FGColor(r, g, b) ES "38;2;" #r ";" #g ";" #b "m"
#define BGColor(r, g, b) ES "48;2;" #r ";" #g ";" #b "m"

#define flushInput() while (getchar() != '\n' && !feof(stdin))

#pragma endregion Macros

#pragma region Prototypes

void logHex(const char *prefix, const void *data, u64 len);

u64 randInt(u64 min, u64 max);

int makeSocketNonBlocking(int socketFd);
u64 serializeMessage(Message_St* msg, char *buf);
void deserializeMessage(const char *buf, Message_St* msg);
int sendAllBytes(int destinationFd, const void *dataPointer, u64 dataLength);
bool isLocalServerRunning(void);
void addToHistory(ChatHistory_St* history, Message_St* msg);

void enableRawMode(void);
void disableRawMode(void);
int readKey(void);  // returns char or special keys like KEY_UP, KEY_DOWN, etc.
bool getCursorPosition(int *row, int *col);

#pragma endregion Prototypes

#endif // UTILS_H