/**
    @file assets.c
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Asset loading and unloading for the Bowling mini-game.
*/
#include "raylib.h"
#include "utils/assets.h"
#include "assetPath.h"
#include "utils/configs.h"
#include "logger.h"

/**
    @brief Detects and extracts individual ball textures from a spritesheet.
    @param[out] textures Bowling textures structure to fill
    @param[in]  image    Spritesheet image
    @return Number of balls successfully extracted
*/
static int detectAndExtractBalls(BowlingTextures_St* textures, Image image) {
    if (image.width <= 0 || image.height <= 0) {
        log_error("Invalid image dimensions: %dx%d", image.width, image.height);
        return 0;
    }

    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    if (image.width < 128 || image.height < 128) {
        log_warn("Image too small: %dx%d", image.width, image.height);
        return 0;
    }

    // Coordonnées originales calibrées pour 1024x1024
    // On les scale à la taille réelle de l'image
    float scaleX = (float)image.width  / 1024.0f;
    float scaleY = (float)image.height / 1024.0f;

    int posX_ref[] = {0, 207, 419, 620, 820, 1024};
    int posY_ref[] = {0, 208, 430, 627, 831, 1024};

    int posX[6], posY[6];
    for (int i = 0; i < 6; i++) {
        posX[i] = (int)(posX_ref[i] * scaleX);
        posY[i] = (int)(posY_ref[i] * scaleY);
    }

    int ballCount = 0;

    for (int row = 0; row < 5 && ballCount < MAX_BALL_TEXTURES; row++) {
        for (int col = 0; col < 5 && ballCount < MAX_BALL_TEXTURES; col++) {
            int x_start = posX[col];
            int y_start = posY[row];
            int cell_w  = posX[col + 1] - posX[col];
            int cell_h  = posY[row + 1] - posY[row];

            if (cell_w <= 0 || cell_h <= 0) continue;

            Rectangle cropRect = {
                (float)x_start, (float)y_start,
                (float)cell_w,  (float)cell_h
            };

            Image ballImage = ImageFromImage(image, cropRect);
            if (ballImage.data == NULL) {
                log_warn("Failed to extract ball at grid position (%d, %d)", col, row);
                continue;
            }

            int    centerX     = cell_w / 2;
            int    centerY     = cell_h / 2;
            Color* pixels      = (Color*)ballImage.data;
            Color  centerPixel = pixels[centerY * cell_w + centerX];
            bool   isValid     = !(centerPixel.r > 250 && centerPixel.g > 250 && centerPixel.b > 250)
                                 && centerPixel.a > 10;

            if (isValid) {
                textures->ballTextures[ballCount] = LoadTextureFromImage(ballImage);
                if (IsTextureValid(textures->ballTextures[ballCount])) {
                    textures->ballCenterColors[ballCount] = centerPixel;
                    Mesh sphere = GenMeshSphere(1.0f, 24, 24);
                    textures->ballModels[ballCount] = LoadModelFromMesh(sphere);
                    SetMaterialTexture(
                        &textures->ballModels[ballCount].materials[0],
                        MATERIAL_MAP_DIFFUSE,
                        textures->ballTextures[ballCount]
                    );
                    ballCount++;
                } else {
                    log_warn("Failed to create texture for ball at (%d, %d)", col, row);
                }
            }

            UnloadImage(ballImage);
        }
    }

    return ballCount;
}

/**
    @brief Loads all textures and models for the bowling game.
    @param[out] textures Bowling textures structure to fill
    @return Error_Et status
*/
Error_Et bowling_loadTextures(BowlingTextures_St* textures) {
    textures->ballTextureCount = 0;
    textures->ballModelCount   = 0;
    textures->crowdModelLoaded = false;
    for (int i = 0; i < MAX_BALL_TEXTURES; i++) {
        textures->ballTextures[i].id = 0;
    }

    /* Resolve asset base path — works whether launched from project root or bowling/ dir */
    static const char* assetCandidates[] = {
        ASSETS_PATH,                    /* value baked at compile time */
        "assets/",                      /* standalone from bowling/ dir */
        "jeux/bowling/assets/",         /* from project root */
        "../jeux/bowling/assets/",      /* from lobby/ */
        NULL
    };
    static char resolvedBase[512] = {0};
    if (resolvedBase[0] == '\0')
        findAssetBase("boules.png", assetCandidates, resolvedBase, sizeof(resolvedBase));

    /* Build full paths using resolved base */
    char ballTexPath[600], pinTexPath[600], laneTexPath[600], crowdPath[600];
    snprintf(ballTexPath,  sizeof(ballTexPath),  "%sboules.png",    resolvedBase);
    snprintf(pinTexPath,   sizeof(pinTexPath),   "%squilles.png",   resolvedBase);
    snprintf(laneTexPath,  sizeof(laneTexPath),  "%spiste.png",     resolvedBase);
    snprintf(crowdPath,    sizeof(crowdPath),    "%scrowd.obj",     resolvedBase);

    /*  Ball textures + models */
    Image ballImage = LoadImage(ballTexPath);
    if (!IsImageValid(ballImage)) {
        log_error("Failed to load ball image: %s", ballTexPath);
        return ERROR_ASSET_LOAD;
    }

    int extractedCount = detectAndExtractBalls(textures, ballImage);

    if (extractedCount <= 0) {
        log_error("Ball extraction failed: no valid ball sprites found in %s", ballTexPath);
        UnloadImage(ballImage);
        return ERROR_ASSET_LOAD;
    }

    textures->ballTextureCount = extractedCount;
    textures->ballModelCount   = extractedCount;
    log_info("Extracted %d ball textures from boules.png", extractedCount);

    UnloadImage(ballImage);

    //  Pin texture (non-fatal) 
    textures->pinTexture = LoadTexture(pinTexPath);
    if (!IsTextureValid(textures->pinTexture))
        log_warn("Failed to load pin texture: %s", pinTexPath);

    //  Lane texture (non-fatal) 
    textures->laneTexture = LoadTexture(laneTexPath);
    if (!IsTextureValid(textures->laneTexture))
        log_warn("Failed to load lane texture: %s", laneTexPath);

    //  Crowd OBJ model 
    textures->crowdModel = LoadModel(crowdPath);
    if (textures->crowdModel.meshCount > 0) {
        textures->crowdModelLoaded = true;
        log_info("Crowd model loaded (%d meshes)", textures->crowdModel.meshCount);
    } else {
        log_warn("Could not load crowd model: %s (not fatal)", crowdPath);
    }

    log_info("Textures loaded successfully");
    return OK;
}

/**
    @brief Unloads all preloaded textures and models.
    @param[in,out] textures Bowling textures structure to clean up
*/
void bowling_unloadTextures(BowlingTextures_St* textures) {
    for (int i = 0; i < textures->ballTextureCount; i++) {
        if (IsTextureValid(textures->ballTextures[i])) {
            UnloadTexture(textures->ballTextures[i]);
            textures->ballTextures[i].id = 0;
        }
    }
    // Detach texture before unloading model to avoid double-free
    for (int i = 0; i < textures->ballModelCount; i++) {
        textures->ballModels[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;
        UnloadModel(textures->ballModels[i]);
    }
    textures->ballTextureCount = 0;
    textures->ballModelCount   = 0;

    if (IsTextureValid(textures->pinTexture)) {
        UnloadTexture(textures->pinTexture);
        textures->pinTexture.id = 0;
    }

    if (IsTextureValid(textures->laneTexture)) {
        UnloadTexture(textures->laneTexture);
        textures->laneTexture.id = 0;
    }

    if (textures->crowdModelLoaded) {
        UnloadModel(textures->crowdModel);
        textures->crowdModelLoaded = false;
    }

    log_info("Textures unloaded");
}

/**
    @brief Fills the transparent parts of ball textures with their center color.
    @param[in,out] textures Bowling textures structure
*/
void bowling_fillBallTextures(BowlingTextures_St* textures) {
    for (int i = 0; i < textures->ballTextureCount; i++) {
        if (!IsTextureValid(textures->ballTextures[i])) continue;
        
        // Obtenir la couleur centrale de la boule
        Color centerColor = textures->ballCenterColors[i];
        
        // Charger l'image de la texture pour la modifier
        Image img = LoadImageFromTexture(textures->ballTextures[i]);
        if (!IsImageValid(img)) continue;
        
        // Assurer que le format est correct
        ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        
        Color* pixels = (Color*)img.data;
        int total = img.width * img.height;
        
        // Remplir tous les pixels transparents avec la couleur centrale
        for (int px = 0; px < total; px++) {
            if (pixels[px].a < 128) {
                pixels[px] = centerColor;
                pixels[px].a = 255;
            } else {
                pixels[px].a = 255;
            }
        }
        
        // Mettre à jour la texture avec l'image modifiée
        UpdateTexture(textures->ballTextures[i], img.data);
        UnloadImage(img);
        
        log_info("Filled ball texture %d with color (%d, %d, %d)", 
                 i, centerColor.r, centerColor.g, centerColor.b);
    }
}
