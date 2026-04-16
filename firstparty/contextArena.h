/**
    @file contextArena.h
    @author Multi Mini-Games Team
    @date 2026-01-14
    @date 2026-04-14
    @brief Thin context-aware wrapper around a generic arena allocator.
*/
#ifndef CONTEXT_ARENA_H
#define CONTEXT_ARENA_H

#include "arena.h"

// ────────────────────────────────────────────────
// Global context arenas
// ────────────────────────────────────────────────

extern Arena globalArena;

extern Arena tempArena; ///< Temporary memory arena for short-term allocations.
extern Arena* contextArena; ///< Current pointer to the active arena used by context functions.

// ────────────────────────────────────────────────
// Core allocation API
// ────────────────────────────────────────────────

// ────────────────────────────────────────────────
// Core allocation API
// ────────────────────────────────────────────────

/**
    @brief Allocates memory from the current context arena.

    @param size_bytes Number of bytes to allocate.
    @return Pointer to allocated memory.
*/
void *context_alloc(size_t size_bytes);

/**
    @brief Reallocates memory from the current context arena.

    @param oldptr Previous pointer (or NULL).
    @param oldsz  Previous size (0 if oldptr is NULL).
    @param newsz  New size.
    @return Pointer to reallocated memory.
*/
void *context_realloc(void *oldptr, size_t oldsz, size_t newsz);

// ────────────────────────────────────────────────
// Convenience duplication helpers
// ────────────────────────────────────────────────

/**
    @brief Duplicates a C string in the current context arena.

    @param cstr Null-terminated string to duplicate.
    @return Allocated copy of the string.
*/
char *context_strdup(const char *cstr);

/**
    @brief Duplicates arbitrary data in the current context arena.

    @param data Pointer to data.
    @param size Size in bytes.
    @return Allocated copy of the data.
*/
void *context_memdup(void *data, size_t size);

// ────────────────────────────────────────────────
// Formatting helpers
// ────────────────────────────────────────────────

/**
    @brief sprintf into arena-allocated buffer.

    @param format printf format string.
    @param ...    Arguments.
    @return Allocated formatted string.
*/
char *context_sprintf(const char *format, ...);

/**
    @brief vsprintf into arena-allocated buffer.

    @param format printf format string.
    @param args   va_list arguments.
    @return Allocated formatted string.
*/
char *context_vsprintf(const char *format, va_list args);

#endif // CONTEXT_ARENA_H
