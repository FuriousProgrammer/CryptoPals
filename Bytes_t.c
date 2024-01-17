#include "Bytes_t.h"

#include <stdlib.h>
#include <string.h> // TODO: remove this dependency!

// TODO: convert this to a LUT? use SIMD for import funcs?
static uint8_t hexit2nybble( const char c ) {
    if ( c >= '0' && c <= '9' ) {
        return c - '0';
    } else if ( c >= 'a' && c <= 'f' ) {
        return c - 'a' + 0xA;
    } else if ( c >= 'A' && c <= 'F' ) {
        return c - 'A' + 0xA;
    } else {
        return -1;
    }
}

Bytes_t* import_hex( const char* const hexstr ) {
    // TODO: make the caller provide the length?
    size_t len = strlen(hexstr);

    // each hexit is 4 bits, `(len + 1)/2` handles an odd `len`
    Bytes_t* ret = malloc( sizeof(*ret) + sizeof(*ret->data)*((len + 1)/2) );
    if ( !ret ) { return NULL; } // TODO: logging
    ret->capacity = (len + 1)/2;
    ret->length = ret->capacity;

    size_t data_pos = 0;
    size_t str_pos = 0;
    if ( len%2 == 1 ) { // special case first nybble of odd-length hex string
        uint8_t nyb = hexit2nybble( hexstr[str_pos++] );
        if ( nyb == -1 ) {
            free(ret);
            return NULL; // TODO: logging
        }
        ret->data[data_pos++] = nyb;
    }
    while ( str_pos < len ) {
        uint8_t hi = hexit2nybble( hexstr[str_pos++] );
        uint8_t lo = hexit2nybble( hexstr[str_pos++] );
        if ( hi == -1 || lo == -1 ) {
            free(ret);
            return NULL; // TODO: logging
        }
        ret->data[data_pos++] = (hi << 4) | lo;
    }

    return ret;
}

char* format_hex( const Bytes_t* const data ) {
    static const char* const chars = "0123456789abcdef";

    // TODO: make the caller provide the memory for output string?
    char* ret = malloc( sizeof(*ret)*(data->length*2) );
    if ( !ret ) { return NULL; } // TODO: logging

    size_t data_pos = 0;
    size_t str_pos = 0;
    while ( data_pos < data->length ) {
        uint8_t byte = data->data[data_pos++];
        ret[str_pos++] = chars[ (byte & 0xF0) >> 4 ];
        ret[str_pos++] = chars[ (byte & 0x0F) >> 0 ];
    }

    return ret;
}

char* format_b64( const Bytes_t* const data ) {
    static const char* const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const char pad = '=';

    // 3 bytes = 4 chars, missing bytes add padding
    size_t ret_len = (data->length/3 + (data->length % 3 > 0))*4;
    char* ret = malloc( sizeof(*ret)*ret_len );

    //  Byte A   Byte B   Byte C
    // 76543210 76543120 76543210
    // |----||-----||-----||----|
    //  chr1  chr2    chr3  chr4

    size_t data_pos = 0;
    size_t str_pos = 0;

    // complete octet triplets:
    for ( size_t i = 0; i < data->length/3; ++i ) {
        uint8_t a = data->data[data_pos++];
        uint8_t b = data->data[data_pos++];
        uint8_t c = data->data[data_pos++];

        ret[str_pos++] = chars[ (a & 0xFC) >> 2 ];
        ret[str_pos++] = chars[ (a & 0x03) << 4 | (b & 0xF0) >> 4 ];
        ret[str_pos++] = chars[ (b & 0x0F) << 2 | (c & 0xC0) >> 6 ];
        ret[str_pos++] = chars[ (c & 0x3F) >> 0 ];
    }
    // 1 extra byte => 2 padding:
    if ( data->length % 3 == 1 ) {
        uint8_t a = data->data[data_pos  ];

        ret[str_pos++] = chars[ (a & 0xFC) >> 2 ];
        ret[str_pos++] = chars[ (a & 0x03) << 4 ];
        ret[str_pos++] = pad;
        ret[str_pos  ] = pad;
    // 2 extra bytes => 1 padding:
    } else if ( data->length % 3 == 2 ) {
        uint8_t a = data->data[data_pos++];
        uint8_t b = data->data[data_pos  ];

        ret[str_pos++] = chars[ (a & 0xFC) >> 2 ];
        ret[str_pos++] = chars[ (a & 0x03) << 4 | (b & 0xF0) >> 4 ];
        ret[str_pos++] = chars[ (b & 0x0F) << 2 ];
        ret[str_pos  ] = pad;
    }

    return ret;
}