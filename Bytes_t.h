#pragma once

#include <stdint.h>
#include <stddef.h> // size_t


typedef enum {
    Bytes_SUCCESS,             // no error
    Bytes_ALLOC_FAILURE,       // realloc fail for some reason
    Bytes_INVALID_HEX_STRING,  // hex string contains invalid characters
    Bytes_MULTI_XOR_EMPTY_KEY, // multi_xor called with an `key` containing no data
} Bytes_Error_e;

typedef struct {
    size_t capacity; // size of allocated buffer
    size_t length;   // size of actual data (may be less than capacity)
    uint8_t* data;   // raw bytes
} Bytes_t;

typedef Bytes_t String_t; // String_t holds C Strings
// (size is at least 1 more than length, containing the NULL terminator)

// !! USAGE NOTE: zero-initialize new Bytes_t for `out_buffer`s !!

// create String_t from C String
// assumes `c_str` is a valid, NULL-terminated C string.
// copies the `c_str` data, doesn't re-use the pointer!
Bytes_Error_e import_cstring(
    const char*     restrict const c_str,
          String_t* restrict const out_buffer
);

// parse encoded strings into Byte_t buffers
Bytes_Error_e import_hex(
    const String_t       hexstr,
          Bytes_t* const out_buffer
);

// encode Byte_t buffers into strings
Bytes_Error_e format_hex(
    const Bytes_t         data,
          String_t* const out_buffer
);
Bytes_Error_e format_b64(
    const Bytes_t         data,
          String_t* const out_buffer
);

// apply a XOR cipher on data using the bytes in key
// if the key is too short, it is repeated
// if the key is too long, it is truncated
// for multi_xor, errors if the `key` buffer is empty.
Bytes_Error_e multi_xor(
    const Bytes_t        data,
    const Bytes_t        key,
          Bytes_t* const out_buffer
);
Bytes_Error_e single_xor(
    const Bytes_t        data,
    const uint8_t        key,
          Bytes_t* const out_buffer
);

// copy an entire Byte_t buffer
Bytes_Error_e clone(
    const Bytes_t        from,
          Bytes_t* const to
);
