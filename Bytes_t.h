#pragma once

#include <stdint.h>
#include <stddef.h> // size_t

typedef struct {
    size_t capacity; // size of allocated buffer
    size_t length;   // size of actual data (may be less than capacity)
    uint8_t data[];  // raw bytes
} Bytes_t;

typedef Bytes_t String_t; // String_t holds C Strings!

// Unless otherwise specified, all functions return NULL on error,
//   and all returned pointers must be free'd by the caller.
// `existing_buffer` is used if provided, otherwise a new one is allocated.
//   If needed, it will be resized using `realloc`.

// create String_t from C String
String_t* import_cstring(
    const char*     restrict const str,
          Bytes_t** restrict const existing_buffer
);

// parse encoded strings into Byte_t buffers
Bytes_t* import_hex(
    const String_t* restrict const hexstr,
          Bytes_t** restrict const existing_buffer
);

// encode Byte_t buffers into strings
String_t* format_hex(
    const Bytes_t*   restrict const data,
          String_t** restrict const existing_buffer
);
String_t* format_b64(
    const Bytes_t*   restrict const data,
          String_t** restrict const existing_buffer
);

// apply a XOR cipher on data using the bytes in key
// if the key is too short, it is repeated
// if the key is too long, it is truncated
Bytes_t* multi_xor(
    const Bytes_t*  restrict const data,
    const Bytes_t*  restrict const key,
          Bytes_t** restrict const existing_buffer
);
Bytes_t* single_xor(
    const Bytes_t*  restrict const data,
    const uint8_t                  key,
          Bytes_t** restrict const existing_buffer
);

Bytes_t* clone(
    const Bytes_t*  restrict const from,
          Bytes_t** restrict const to
);
