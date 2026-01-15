# Nob-inspired Dynamic Array and String Builder

Lightweight, header-only implementations of a generic dynamic array and a string builder, extracted and adapted from Tsoding's excellent single-file `nob.h`.

These headers are split out for personal use, keeping only the parts needed (dynamic array + string builder) while removing the rest of the original build-system-focused code.

Both are dedicated to the public domain under the Unlicense (same as the original `nob.h`).

## Files

- `dynamicArray.h` – Generic dynamic array implementation using macros.
- `stringBuilder.h` – Simple growable string builder built on top of the dynamic array (char variant).

## Usage Example

```c
#define STRING_BUILDER_IMPLEMENTATION  // Place in exactly one .c file
#include "stringBuilder.h"

int main(void) {
    StringBuilder_St sb = {0};

    sb_append_cstr(&sb, "Hello");
    sbAppendf(&sb, " %s %d!", "world", 2026);
    sb_append_null(&sb);  // Optional: add terminating '\0' for printf/etc.

    printf("%s\n", sb.items);  // Hello world 2026!

    sb_free(&sb);
    return 0;
}
```

## Dynamic Array Example

```c
#include "dynamicArray.h"
#include <stdio.h>

int main(void) {
    DA(int) numbers = {0};

    da_append(&numbers, 10);
    da_append(&numbers, 20);
    da_append_many(&numbers, (int[]){30, 40, 50}, 3);

    da_foreach(int, it, &numbers) {
        printf("%d\n", *it);
    }

    da_free(numbers);
    return 0;
}
```

## Features

- Generic dynamic array via macros (`DA(T)`, `da_append`, `da_append_many`, etc.)
- String builder with `sb_append_cstr`, `sbAppendf` (printf-style), buffer append, and padding utilities
- No external dependencies beyond the C standard library
- Minimal, self-contained headers

## Credits

Author: Tsoding

Originally extracted from: https://github.com/tsoding/nob.h

Feel free to use, modify, or delete anything — it's all public domain.