/**
    @file save.c
    @author Fshimi Hawlk
    @date 2026-02-27
    @brief Implementation of game state serialization.
*/

#include "setups/save.h"

#include "setups/shape.h"

#include "utils/common.h"
#include "utils/globals.h"
#include "utils/userTypes.h"

#include <assert.h>

#define SERIAL_MAGIC (u32) (('K' << 0) | ('I' << 8) | ('M' << 16) | ('I' << 24))
#define SERIAL_VERSION 1

static u64 writeU8(u8* buffer, u64 bufferSize, u64 offset, u8 value) {
    if (offset + sizeof(value) > bufferSize) {
        log_error("Excess writing detected");
        abort();
        return 0;
    }
    if (buffer) memcpy(buffer + offset, &value, sizeof(value));
    return sizeof(value);
}

static u64 writeS8(u8* buffer, u64 bufferSize, u64 offset, s8 value) {
    if (offset + sizeof(value) > bufferSize) {
        log_error("Excess writing detected");
        abort();
        return 0;
    }
    if (buffer) memcpy(buffer + offset, &value, sizeof(s8));
    return sizeof(value);
}

static u64 writeU32(u8* buffer, u64 bufferSize, u64 offset, u32 value) {
    if (offset + sizeof(value) > bufferSize) {
        log_error("Excess writing detected");
        abort();
        return 0;
    }
    if (buffer) memcpy(buffer + offset, &value, sizeof(u32));
    return sizeof(value);
}

static u64 writeU64(u8* buffer, u64 bufferSize, u64 offset, u64 value) {
    if (offset + sizeof(value) > bufferSize) {
        log_error("Excess writing detected");
        abort();
        return 0;
    }
    if (buffer) memcpy(buffer + offset, &value, sizeof(u64));
    return sizeof(value);
}

static u64 writeF32(u8* buffer, u64 bufferSize, u64 offset, f32 value) {
    if (offset + sizeof(value) > bufferSize) {
        log_error("Excess writing detected");
        abort();
        return 0;
    }
    if (buffer) memcpy(buffer + offset, &value, sizeof(f32));
    return sizeof(value);
}


static u64 readU8(const u8* buffer, u64 bufferSize, u64 offset, u8* value) {
    if (value == NULL || buffer == NULL || bufferSize == 0) return 0;
    if (offset + sizeof(*value) > bufferSize) {
        log_error("Excess reading detected");
        abort();
        return 0;
    }

    memcpy(value, buffer + offset, sizeof(*value));
    return sizeof(*value);
}

static u64 readS8(const u8* buffer, u64 bufferSize, u64 offset, s8* value) {
    if (value == NULL || buffer == NULL || bufferSize == 0) return 0;
    if (offset + sizeof(*value) > bufferSize) {
        log_error("Excess reading detected");
        abort();
        return 0;
    }

    memcpy(value, buffer + offset, sizeof(*value));
    return sizeof(*value);
}

static u64 readU32(const u8* buffer, u64 bufferSize, u64 offset, u32* value) {
    if (value == NULL || buffer == NULL || bufferSize == 0) return 0;
    if (offset + sizeof(*value) > bufferSize) {
        log_error("Excess reading detected");
        abort();
        return 0;
    }

    memcpy(value, buffer + offset, sizeof(*value));
    return sizeof(*value);
}

static u64 readU64(const u8* buffer, u64 bufferSize, u64 offset, u64* value) {
    if (value == NULL || buffer == NULL || bufferSize == 0) return 0;
    if (offset + sizeof(*value) > bufferSize) {
        log_error("Excess reading detected");
        abort();
        return 0;
    }

    memcpy(value, buffer + offset, sizeof(*value));
    return sizeof(*value);
}

static u64 readF32(const u8* buffer, u64 bufferSize, u64 offset, f32* value) {
    if (value == NULL || buffer == NULL || bufferSize == 0) return 0;
    if (offset + sizeof(*value) > bufferSize) {
        log_error("Excess reading detected");
        abort();
        return 0;
    }

    memcpy(value, buffer + offset, sizeof(*value));
    return sizeof(*value);
}

u64 getSerializedGameStateSize(const GameState_St* const state) {
    const PrefabManager_St* manager = &state->prefabManager;
    u64 size = 0;

    // Magic + version
    size += sizeof(u32);
    size += sizeof(u8);

    // prefab variant
    size += sizeof(u8);

    // Scoring
    size += sizeof(state->scoring.score);
    size += sizeof(state->scoring.streakCount);
    size += sizeof(state->scoring.streakGrace);

    // Board width & height
    size += sizeof(state->board.height);
    size += sizeof(state->board.width);

    // Board blocks (hitsLeft + colorIndex per cell)
    u64 blockSize = sizeof(Block_St);
    size += blockSize * state->board.width * state->board.height;

    // runTimeWeights
    size += sizeof(state->prefabManager.sizeWeights.runTimeWeights);

    // Bags (MAX_SHAPE_SIZE fixed)
    for (u8 i = 0; i < MAX_SHAPE_SIZE; ++i) {
        const PrefabIndexBagVec_St* bag = &manager->bags[i];
        size += sizeof(bag->count) + bag->count * sizeof(*bag->items);
    }

    // Slots (3 fixed)
    size += (sizeof(u64) 
         + sizeof((u8) state->prefabManager.slots[0].colorIndex)
         + sizeof((u8) state->prefabManager.slots[0].placed)
    ) * 3;  // placed, colorIndex, bagIndex

    return size;
}

u64 serializeGameState(const GameState_St* const state, u8* buffer, const u64 bufferSize) {
    if (state == NULL || buffer == NULL) return false;

    const PrefabManager_St* manager = &state->prefabManager;

    u64 offset = 0;

    // Write magic + version
    offset += writeU32(buffer, bufferSize, offset, SERIAL_MAGIC);
    offset += writeU8(buffer, bufferSize, offset, SERIAL_VERSION);

    // prefab variant
    offset += writeU8(buffer, bufferSize, offset, (u8) prefabVariant);

    // Scoring
    offset += writeU64(buffer, bufferSize, offset, state->scoring.score);
    offset += writeU8(buffer, bufferSize, offset, state->scoring.streakCount);
    offset += writeU8(buffer, bufferSize, offset, state->scoring.streakGrace);

    // Board
    offset += writeU8(buffer, bufferSize, offset, state->board.width);
    offset += writeU8(buffer, bufferSize, offset, state->board.height);
    for (u8 r = 0; r < state->board.height; ++r) {
        for (u8 c = 0; c < state->board.width; ++c) {
            const Block_St block = state->board.blocks[r][c];
            offset += writeS8(buffer, bufferSize, offset, block.hitsLeft);
            offset += writeU8(buffer, bufferSize, offset, block.colorIndex);
        }
    }

    // runTimeWeights
    for (u8 i = 0; i < MAX_SHAPE_SIZE; ++i) {
        offset += writeF32(buffer, bufferSize, offset, manager->sizeWeights.runTimeWeights[i]);
    }

#define serializeDA(buffer, bufferSize, offset, da, writeItem)                      \
    do {                                                                            \
        offset += writeU64(buffer, (bufferSize), (offset), (da)->count);            \
        for (u64 i = 0; i < (da)->count; ++i) {                                     \
            offset += writeItem(buffer, (bufferSize), (offset), (da)->items[i]);    \
        }                                                                           \
    } while (0)

    // Bags
    for (u8 i = 0; i < MAX_SHAPE_SIZE; ++i) {
        const PrefabIndexBagVec_St* bag = &manager->bags[i];
        serializeDA(buffer, bufferSize, offset, bag, writeU32);
    }

    // Slots
    for (u8 i = 0; i < 3; ++i) {
        const Shape_St* slot = &manager->slots[i];

        assert(slot->prefab >= prefabs && "Slot wasn't proprely init");
        u64 prefabIndex = (slot->prefab - prefabsBag.items);
        u8 colorIndex = (u8) slot->colorIndex;
        u8 placed = (u8) slot->placed;
        offset += writeU64(buffer, bufferSize, offset, prefabIndex);
        offset += writeU8(buffer, bufferSize, offset, colorIndex);
        offset += writeU8(buffer, bufferSize, offset, placed);
    }

    return bufferSize - offset;
}

bool deserializeGameState(GameState_St* const state, const u8* buffer, const u64 bufferSize, bool init) {
    if (state == NULL) {
        log_warn("Received NULL state");
        return false;
    }
    
    if (buffer == NULL) {
        log_warn("Received NULL buffer");
        return false;
    }

    if (bufferSize == 0) {
        log_warn("Received 0 sized buffer");
        return false;
    }

    u32 magic;
    u8 version;
    u64 offset = 0;

    offset += readU32(buffer, bufferSize, offset, &magic);
    if (magic != SERIAL_MAGIC) {
        log_error("Data corruption detected: \"SERIAL_MAGIC\" doesn't correspond.");
        abort();
        return false;
    }

    offset += readU8(buffer, bufferSize, offset, &version);
    if (version != SERIAL_VERSION) {
        log_error("Data corruption detected: \"SERIAL_VERSION\" doesn't correspond: got %zu, expected: %zu", version, SERIAL_VERSION);
        abort();
        return false;
    }

    PrefabManager_St* manager = &state->prefabManager;
    
    u8 prefabVariantU8;
    offset += readU8(buffer, bufferSize, offset, &prefabVariantU8);
    if (init) {
        prefabVariant = (GamePrefabVariant_Et) prefabVariantU8;
        initPrefabsAndVariants(&prefabsBag, prefabVariant);
    }

    // Scoring
    offset += readU64(buffer, bufferSize, offset, &state->scoring.score);
    offset += readU8(buffer, bufferSize, offset, &state->scoring.streakCount);
    offset += readU8(buffer, bufferSize, offset, &state->scoring.streakGrace);

    // Board
    offset += readU8(buffer, bufferSize, offset, &state->board.width);
    offset += readU8(buffer, bufferSize, offset, &state->board.height);

    assert(state->board.width == state->board.height);
    for (u8 r = 0; r < state->board.height; ++r) {
        for (u8 c = 0; c < state->board.width; ++c) {
            offset += readS8(buffer, bufferSize, offset, &state->board.blocks[r][c].hitsLeft);
    
            u8 colorIndexU8;
            offset += readU8(buffer, bufferSize, offset, &colorIndexU8);
    
            state->board.blocks[r][c].colorIndex = (BlockColor_Et) colorIndexU8;
        }
    }

    // runTimeWeights
    for (u8 i = 0; i < MAX_SHAPE_SIZE; ++i) {
        offset += readF32(buffer, bufferSize, offset, &manager->sizeWeights.runTimeWeights[i]);
    }

#define deserializeDA(buffer, bufferSize, offset, T, da, readItem)          \
    do {                                                                    \
        T __item;                                                           \
        u64 __count = 0;                                                    \
        offset += readU64((buffer), (bufferSize), (offset), &__count);      \
        (bag)->count = 0;                                                   \
        while (__count > 0) {                                               \
            __count--;                                                      \
            offset += readItem((buffer), (bufferSize), (offset), &__item);  \
            da_append((da), __item);                                        \
        }                                                                   \
    } while (0);

    // Bags (load directly, skipping default init from prefabsBag)
    for (u8 i = 0; i < MAX_SHAPE_SIZE; ++i) {
        PrefabIndexBagVec_St* bag = &manager->bags[i];
        deserializeDA(buffer, bufferSize, offset, u32, bag, readU32);
    }

    // Slots
    for (u8 i = 0; i < 3; ++i) {
        Shape_St* slot = &manager->slots[i];
        u64 prefabIndex = prefabsBag.count;
        u8 placed, colorIndex;
        offset += readU64(buffer, bufferSize, offset, &prefabIndex);
        offset += readU8(buffer, bufferSize, offset, &colorIndex);
        offset += readU8(buffer, bufferSize, offset, &placed);

        slot->placed = (bool) placed;
        slot->colorIndex = (BlockColor_Et) colorIndex;
        slot->dragging = false;
        slot->id = i;
        slot->center = defaultPositions[i];

        if (prefabIndex >= prefabsBag.count) {
            log_error("Prefab index of slot %u was corrupted: got %zu (Max: %u)", i, prefabIndex, prefabCount - 1);
            abort();
            return false;
        }

        slot->prefab = &prefabsBag.items[prefabIndex];
    }

    state->gameOver = false;

    return true;
}