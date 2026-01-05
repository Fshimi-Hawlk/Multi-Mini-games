#ifndef CONTEXT_ARENA_H
#define CONTEXT_ARENA_H

#include "arena.h"

static Arena globalArena;
static Arena tempArena;
static Arena* contextArena;

void *context_alloc(size_t size_bytes);
void *context_realloc(void *oldptr, size_t oldsz, size_t newsz);
char *context_strdup(const char *cstr);
void *context_memdup(void *data, size_t size);
char *context_sprintf(const char *format, ...);
char *context_vsprintf(const char *format, va_list args);

#endif // CONTEXT_ARENA_H

#ifdef CONTEXT_ARENA_IMPLEMENTATION

static Arena globalArena = {0};
static Arena tempArena = {0};
static Arena* contextArena = &globalArena;

void *context_alloc(size_t size_bytes) {
    ARENA_ASSERT(contextArena != NULL);
    return arena_alloc(contextArena, size_bytes);
}

void *context_realloc(void *oldptr, size_t oldsz, size_t newsz) {
    ARENA_ASSERT(contextArena != NULL);
    return arena_realloc(contextArena, oldptr, oldsz, newsz);
}

char *context_strdup(const char *cstr) {
    ARENA_ASSERT(contextArena != NULL);
    return arena_strdup(contextArena, cstr);
}

void *context_memdup(void *data, size_t size) {
    ARENA_ASSERT(contextArena != NULL);
    return arena_memdup(contextArena, data, size);
}

char *context_sprintf(const char *format, ...) {
    ARENA_ASSERT(contextArena != NULL);

    va_list args;
    va_start(args, format);
    char *result = context_vsprintf(format, args);
    va_end(args);

    return result;
}

char *context_vsprintf(const char *format, va_list args) {
    ARENA_ASSERT(contextArena != NULL);
    return arena_vsprintf(contextArena, format, args);
}

#define ARENA_IMPLEMENTATION
#include "arena.h"

#endif // CONTEXT_ARENA_IMPLEMTATION