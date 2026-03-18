/**
 * @file input_button.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Definition of the IaC (Input and Connect) UI elements.
 */

#ifndef INPUT_BUTTON_H
#define INPUT_BUTTON_H

#include "../../../thirdparty/raylib.h" 
#include <stdbool.h>

/** @brief Maximum length for an IP address string. */
#define IP_MAX_LENGTH 15

/**
 * @brief States for text areas and buttons.
 */
typedef enum {
    STATE_NORMAL = 0,   ///< Default state.
    STATE_HOVER,        ///< Mouse is over the element.
    STATE_ACTIVE,       ///< Element is focused (for input).
    STATE_CLICK,        ///< Element is being clicked.
    STATE_DISABLED      ///< Element is disabled.
} ButtonState;

/**
 * @brief Structure for Input and Connect (IaC) UI elements.
 */
typedef struct {
    Rectangle rect;      ///< Bounds of the element.
    ButtonState state;   ///< Current state of the element.
    Color baseColor;     ///< Base color for the element.
    char *text;          ///< Label or placeholder text.
    bool isIPValid;      ///< Validation state of the entered IP.
} IaC_button;

/**
 * @brief Initializes an IaC element (Input or Button).
 * @param x X coordinate of the element.
 * @param y Y coordinate of the element.
 * @param width Width of the element.
 * @param height Height of the element.
 * @param text Initial text or placeholder.
 * @param color Base color of the element.
 * @return The initialized IaC_button structure.
 */
IaC_button InitIaCElement(float x, float y, float width, float height, char *text, Color color);

/**
 * @brief Handles text input for the IP field.
 * @param input Pointer to the IaC_button input element.
 * @param buffer Buffer to store the entered text.
 * @param letterCount Pointer to the current number of characters in the buffer.
 * @return true if the text has been modified, false otherwise.
 */
bool UpdateIPInput(IaC_button *input, char *buffer, int *letterCount);

/**
 * @brief Handles interaction with the connect button.
 * @param button Pointer to the IaC_button button element.
 * @param canClick Flag indicating if the button can be clicked.
 * @return true if the button is clicked and the IP is valid.
 */
bool UpdateConnectButton(IaC_button *button, bool canClick);

/**
 * @brief Renders the IaC element to the screen.
 * @param element The IaC_button element to draw.
 * @param currentText The current text to display inside the element.
 */
void DrawIaCElement(IaC_button element, const char *currentText);

#endif