/**
    @file editor/codegen.c
    @author Fshimi-Hawlk
    @date 2026-03-30
    @brief Code generator that turns the current editor level into clean C source code.
*/

#include "editor/codegen.h"

#include "nob/dynamicArray.h"
#include "utils/globals.h"
#include "utils/userTypes.h"

#include "sharedUtils/debug.h"
#include "sharedUtils/container.h"

static const char* terrainTypeComment(TerrainKind_Et kind) {
    switch (kind) {
        case TERRAIN_KIND_NORMAL:       return "Normal";
        case TERRAIN_KIND_GRASS:        return "Grass";
        case TERRAIN_KIND_WOOD_PLANK:   return "Wood Plank";
        case TERRAIN_KIND_STONE:        return "Stone";
        case TERRAIN_KIND_ICE:          return "Ice";
        case TERRAIN_KIND_BOUNCY:       return "Bouncy";
        case TERRAIN_KIND_MOVING_H:     return "Moving Horizontal";
        case TERRAIN_KIND_MOVING_V:     return "Moving Vertical";
        case TERRAIN_KIND_WATER:        return "Water";
        case TERRAIN_KIND_DECORATIVE:   return "Decorative";
        case TERRAIN_KIND_PORTAL:       return "Portal";
        default:                        return "Unknown";
    }
}

bool editorGenerateCode(const LobbyGame_St* const game, const char* filename) {
    UNUSED(game);

    FILE* f = fopen(filename, "w");
    if (!f) {
        log_error("Failed to create code file: %s", filename);
        return false;
    }

    TerrainVec_St terrainGroups[__terrainKindCount] = {0};

    for (size_t i = 0; i < terrains.count; ++i) {
        const LobbyTerrain_St* t = &terrains.items[i];
        da_append(&terrainGroups[t->kind], *t);
    }

    fprintf(f, "/**\n");
    fprintf(f, "    @file generatedLevel.c\n");
    fprintf(f, "    @brief Auto-generated level data from editor.\n");
    fprintf(f, "    Generated on %s\n", __DATE__);
    fprintf(f, "*/\n\n");

    fprintf(f, "static LobbyTerrain_St _generatedTerrainContent[] = {\n");

    // Group by kind for nice comments

    for (TerrainKind_Et kind = 0; kind < __terrainKindCount; ++kind) {
        const TerrainVec_St group = terrainGroups[kind];
        if (group.count == 0) continue;

        if (kind > 0) fprintf(f, "\n");
        fprintf(f, "    // ==================================================================\n");
        fprintf(f, "    // %s\n", terrainTypeComment(kind));
        fprintf(f, "    // ==================================================================\n");

        for (u32 i = 0; i < group.count; ++i) {
            const LobbyTerrain_St* t = &group.items[i];

            fprintf(f, "    { .rect = {%.1ff, %.1ff, %.1ff, %.1ff}, "
                    ".color = {%d, %d, %d, %d}, .roundness = %.1ff",
                    t->rect.x, t->rect.y, t->rect.width, t->rect.height,
                    t->color.r, t->color.g, t->color.b, t->color.a,
                    t->roundness);

            // Extra fields when relevant
            if (t->kind == TERRAIN_KIND_MOVING_H || t->kind == TERRAIN_KIND_MOVING_V) {
                fprintf(f, ", .velocity = {%.1ff, %.1ff}, .moveDistance = %.1ff",
                        t->velocity.x, t->velocity.y, t->moveDistance);
            }

            if (t->kind == TERRAIN_KIND_PORTAL) {
                fprintf(
                    f, ", .portalTargetPosition = {%.1ff, %.1ff}, .isTwoWayPortal = %s, .isOnlyReceiverPortal = %s",
                    t->portalTargetPosition.x, t->portalTargetPosition.y,
                    boolStr(t->isTwoWayPortal), boolStr(t->isOnlyReceiverPortal)
                );
            }

            fprintf(f, " },\n");
        }
    }

    fprintf(f, "};\n\n");

    fclose(f);
    log_info("Generated clean C code: %s (%zu terrains)", filename, terrains.count);
    return true;
}