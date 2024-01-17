#pragma once
#ifndef BYTES_T_H

#include <stdint.h>
#include <stddef.h> // size_t

typedef struct {
    size_t capacity; // size of allocated buffer
    size_t length;   // size of actual data (may be less than capacity)
    uint8_t data[];  // raw bytes
} Bytes_t;

// parse encoded strings into Byte_t buffers
// returns NULL on error
// returned pointers must be free'd by caller
Bytes_t* import_hex( const char* const hexstr );

// encode Byte_t buffers into strings
// returns NULL on error
// returned pointers must be free'd by caller
char* format_hex( const Bytes_t* const data );
char* format_b64( const Bytes_t* const data );

#define BYTES_T_H
#endif