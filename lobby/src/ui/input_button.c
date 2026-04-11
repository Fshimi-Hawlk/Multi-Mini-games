/**
 * @file input_button.c
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Implementation of the IaC (Input and Connect) UI elements.
 */

#include "ui/input_button.h"
#include <string.h>
#include <stdlib.h>

/** @brief State of the text zone. */
int texte_zone = STATE_NORMAL;
/** @brief State of the confirmation button. */
int confirmation_button = STATE_DISABLED;

/**
 * @brief Initializes an IaC element (Input or Button).
 * Allotes the necessary memory for the text and configures the base color.
 * @param x X coordinate of the element.
 * @param y Y coordinate of the element.
 * @param width Width of the element.
 * @param height Height of the element.
 * @param text Initial text or placeholder.
 * @param color Base color of the element.
 * @return The initialized IaC_button structure.
 */
IaC_button InitIaCElement(float x, float y, float width, float height, char *text, Color color) {
    IaC_button element = {
        .rect = { x, y, width, height },
        .text = NULL,
        .baseColor = color,
        .state = STATE_NORMAL,
        .isIPValid = false,
    };

    if (text != NULL) {
        element.text = malloc(strlen(text) + 1);
        if (element.text != NULL) {
            strcpy(element.text, text);
        }
    }

    return element;
}

/**
 * @brief Handles text input for the IP field.
 * Listens for key presses and keeps the input within the IP_MAX_LENGTH limit.
 * @param input Pointer to the IaC_button input element.
 * @param buffer Buffer to store the entered text.
 * @param letterCount Pointer to the current number of characters in the buffer.
 * @return true if the text has been modified during this frame.
 */
bool UpdateIPInput(IaC_button *input, char *buffer, int *letterCount) {
    bool isModified = false;
    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, input->rect)) {
        if (input->state != STATE_ACTIVE) input->state = STATE_HOVER;
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            input->state = STATE_ACTIVE;
        }
    } else {
        if (input->state == STATE_HOVER) input->state = STATE_NORMAL;
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            input->state = STATE_NORMAL;
        }
    }

    if (input->state == STATE_ACTIVE) {
        int key = GetCharPressed();
        
        while (key > 0) {
            if ((key >= 48 && key <= 57) || key == 46) {
                if (*letterCount < IP_MAX_LENGTH) {
                    buffer[*letterCount] = (char)key;
                    buffer[*letterCount + 1] = '\0';
                    (*letterCount)++;
                    isModified = true;
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            if (*letterCount > 0) {
                (*letterCount)--;
                buffer[*letterCount] = '\0';
                isModified = true;
            }
        }
    }
    
    input->isIPValid = (*letterCount >= 7); 

    return isModified;
}

bool UpdateTextInput(IaC_button *input, char *buffer, int *letterCount, int maxLen) {
    bool isModified = false;
    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, input->rect)) {
        if (input->state != STATE_ACTIVE) input->state = STATE_HOVER;
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            input->state = STATE_ACTIVE;
        }
    } else {
        if (input->state == STATE_HOVER) input->state = STATE_NORMAL;
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            input->state = STATE_NORMAL;
        }
    }

    if (input->state == STATE_ACTIVE) {
        int key = GetCharPressed();
        
        while (key > 0) {
            if (key >= 32 && key <= 125) { // Visible ASCII characters
                if (*letterCount < maxLen) {
                    buffer[*letterCount] = (char)key;
                    buffer[*letterCount + 1] = '\0';
                    (*letterCount)++;
                    isModified = true;
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            if (*letterCount > 0) {
                (*letterCount)--;
                buffer[*letterCount] = '\0';
                isModified = true;
            }
        }
    }

    return isModified;
}

/**
 * @brief Handles interaction with the connect button.
 * Incorporates a check to disable the element if canClick is false.
 * @param button Pointer to the IaC_button button element.
 * @param canClick Flag indicating if the button can be clicked.
 * @return true if the click is confirmed (button released over the element).
 */
bool UpdateConnectButton(IaC_button *button, bool canClick) {
    if (!canClick) {
        button->state = STATE_DISABLED;
        return false;
    }

    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, button->rect)) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            button->state = STATE_CLICK;
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            button->state = STATE_HOVER;
            return true;
        } else {
            button->state = STATE_HOVER;
        }
    } else {
        button->state = STATE_NORMAL;
    }
    
    return false;
}

/**
 * @brief Renders the element to the screen, dynamically managing visual feedback.
 * @param element The IaC_button element to draw.
 * @param currentText The current text to display inside the element.
 */
void DrawIaCElement(IaC_button element, const char *currentText) {
    Color drawColor = element.baseColor;
    Color textColor = BLACK;
    
    switch (element.state) {
        case STATE_HOVER:    drawColor = Fade(element.baseColor, 0.8f); break;
        case STATE_ACTIVE:   drawColor = Fade(element.baseColor, 1.0f); break;
        case STATE_CLICK:    drawColor = Fade(element.baseColor, 0.6f); break;
        case STATE_DISABLED: drawColor = LIGHTGRAY; textColor = GRAY;   break;
        case STATE_NORMAL:   drawColor = Fade(element.baseColor, 0.9f); break;
    }

    DrawRectangleRec(element.rect, drawColor);
    
    Color outlineColor = (element.state == STATE_ACTIVE) ? RED : DARKGRAY;
    DrawRectangleLines((int)element.rect.x, (int)element.rect.y, (int)element.rect.width, (int)element.rect.height, outlineColor);

    const char* textToDraw = (currentText != NULL && strlen(currentText) > 0) ? currentText : element.text;
    int textWidth = MeasureText(textToDraw, 20);
    
    int textX = (int)(element.rect.x + (element.rect.width - textWidth) / 2);
    int textY = (int)(element.rect.y + (element.rect.height - 20) / 2);
    
    if (currentText != NULL) {
        textX = (int)(element.rect.x + 10);
        if (strlen(currentText) == 0) textColor = GRAY;
    }
    
    DrawText(textToDraw, textX, textY, 20, textColor);
    
    if (element.state == STATE_ACTIVE && currentText != NULL) {
        if (((int)(GetTime() * 2)) % 2 == 0) {
            DrawRectangle(textX + textWidth + 2, textY, 2, 20, RED);
        }
    }
}