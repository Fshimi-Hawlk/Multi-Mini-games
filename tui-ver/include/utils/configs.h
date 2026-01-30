#ifndef CONFIG_H
#define CONFIG_H

#define MAX_BUFFER_SIZE 512

#define ASSETS_DIR      "assets/"
#define APP_DATA_DIR    ASSETS_DIR"appData/"
#define FONTS_DIR       ASSETS_DIR"fonts/"
#define IMAGES_DIR      ASSETS_DIR"images/"

#define FONT_PATH       FONTS_DIR"JetBrainsMono/ttf/JetBrainsMono-Regular.ttf"

#define MAX_TRACE_BACK_FRAMES        32

#define SERVER_PORT                  8080
#define MAXIMUM_NUMBER_OF_CLIENTS    64
#define RECEIVE_BUFFER_SIZE          4096
#define MAXIMUM_MESSAGE_PAYLOAD_SIZE (64 * 1024)
#define MAX_USERNAME_LENGTH          32
#define MAX_HISTORY                  8
#define MAX_CLIENTS                  64

#endif // CONFIG_H