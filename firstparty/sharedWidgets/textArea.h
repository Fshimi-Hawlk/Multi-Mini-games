/**
    @file sharedWidgets/textArea.h
    @author Fshimi-Hawlk
    @date 2026-04-13
    @brief Multi-line TextArea widget (editable + display-only with word-wrap).

    Contributors:
        - Fshimi-Hawlk:
            - Initial design
            - Adopted named-parameter init macro pattern
            - StringBuilder_St buffer
            - Embedded ScrollFrame_St
*/

#ifndef WIDGETS_TEXT_AREA_H
#define WIDGETS_TEXT_AREA_H

#include "sharedWidgets/types.h"

/**
    @brief Configuration for textAreaInit() – uses named-parameter macro pattern.
           All fields have safe defaults when zero-initialized.
*/
typedef struct {
    char           _;                ///< sentinel for safe C99 compound literal
    Rectangle      bounds;
    bool           editable;         ///< default: false
    bool           wordWrap;         ///< default: true
    f32            roundness;        ///< default: 0.1f
    const char    *placeholder;      ///< default: NULL
    u32            initialCapacity;  ///< default: 2048
} TextAreaConfig_St;

/**
    @brief Convenience macro for C99 compound literal initialization with defaults.

    Example:
        TextArea_St area = {0};
        textAreaInit(&area, .bounds = myRect, .editable = true, .roundness = 0.2f);
*/
#define textAreaInit(area, ...) \
    textAreaInit__full((area), (TextAreaConfig_St){ ._ = 0, __VA_ARGS__ })

/**
    @brief Full initializer (called by the macro). Do not call directly.
*/
void textAreaInit__full(TextArea_St* area, TextAreaConfig_St config);

/**
    @brief Updates the text area (scrolling, typing when editable, cursor movement, etc.).
    @return true if the text content changed this frame
*/
bool textAreaUpdate(TextArea_St* area, Vector2 mouseScreen);

/**
    @brief Draws the text area (wrapped text + cursor when active + scrollbars via ScrollFrame).
*/
void textAreaDraw(const TextArea_St* area, Font font, f32 fontSize);

/**
    @brief Append text (works in both editable and display modes).
           Very useful for chat history: textAreaAppend(&area, "Player: Hello!\n");
*/
void textAreaAppend(TextArea_St* area, const char* text);

#endif // WIDGETS_TEXT_AREA_H