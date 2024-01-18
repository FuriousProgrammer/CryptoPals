#pragma once

#include <stdint.h>
#include <stddef.h> // size_t

typedef struct {
    size_t capacity; // size of allocated buffer
    size_t length;   // size of actual data (may be less than capacity)
    uint8_t data[];  // raw bytes
} Bytes_t;

// Unless otherwise specified, all functions return NULL on error,
//   and all returned pointers must be free'd by the caller.

// parse encoded strings into Byte_t buffers
Bytes_t* import_hex( const char* const hexstr );

// encode Byte_t buffers into strings
char* format_hex( const Bytes_t* const data );
char* format_b64( const Bytes_t* const data );

// apply a XOR cipher on data using the bytes in key
// if the key is too short, it is repeated
// if the key is too long, it is truncated
Bytes_t* multi_xor( const Bytes_t* const data, const Bytes_t* const key );
Bytes_t* single_xor( const Bytes_t* const data, const uint8_t key );
