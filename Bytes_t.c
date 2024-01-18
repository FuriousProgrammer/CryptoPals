#include "Bytes_t.h"

#include <stdlib.h>
#include <string.h>

static Bytes_t* reserve( size_t new_length, Bytes_t** const buffer ) {
    Bytes_t* ret;

    if ( !buffer || !(*buffer) ) {
        ret = malloc( sizeof(*ret) + sizeof(*ret->data)*new_length );
        if ( !ret ) { return NULL; }
        ret->capacity = new_length;
        if ( buffer ) {
            *buffer = ret;
        }
    } else {
        ret = *buffer;
        if ( ret->capacity < new_length ) {
            Bytes_t* temp = realloc( ret, sizeof(*ret) + sizeof(*ret->data)*new_length );
            if ( !temp ) { return NULL; }
            temp->capacity = new_length;
            *buffer = ret = temp;
        }
    }
    ret->length = new_length;

    return ret;
}

String_t* import_cstring(
    const char*     restrict const c_str,
          Bytes_t** restrict const existing_buffer
) {
    size_t ret_len = strlen(c_str) + 1;
    String_t* ret = reserve( ret_len, existing_buffer );
    if ( !ret ) { return NULL; }
    ret->length = ret_len - 1;
    memcpy( ret->data, c_str, ret_len );

    return ret;
}

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

Bytes_t* import_hex(
    const String_t* restrict const hexstr,
          Bytes_t** restrict const existing_buffer
) {
    // each hexit is 4 bits, `(len + 1)/2` handles an odd hexstr length
    size_t ret_len = (hexstr->length + 1)/2;

    Bytes_t* ret = reserve( ret_len, existing_buffer );
    if ( !ret ) { return NULL; }

    size_t data_pos = 0;
    size_t str_pos = 0;
    if ( hexstr->length%2 == 1 ) { // special case first nybble of odd-length hex string
        uint8_t nyb = hexit2nybble( hexstr->data[str_pos++] );
        if ( nyb == -1 ) {
            free(ret);
            return NULL; // TODO: logging
        }
        ret->data[data_pos++] = nyb;
    }
    while ( str_pos < hexstr->length ) {
        uint8_t hi = hexit2nybble( hexstr->data[str_pos++] );
        uint8_t lo = hexit2nybble( hexstr->data[str_pos++] );
        if ( hi == -1 || lo == -1 ) {
            free(ret);
            return NULL; // TODO: logging
        }
        ret->data[data_pos++] = (hi << 4) | lo;
    }

    return ret;
}

String_t* format_hex(
    const Bytes_t*   restrict const data,
          String_t** restrict const existing_buffer
) {
    static const char* const chars = "0123456789abcdef";

    size_t ret_len = data->length*2 + 1;
    String_t* ret = reserve(ret_len, existing_buffer);
    if ( !ret ) { return NULL; }
    ret->data[ret_len - 1] = '\0';

    size_t data_pos = 0;
    size_t str_pos = 0;
    while ( data_pos < data->length ) {
        uint8_t byte = data->data[data_pos++];
        ret->data[str_pos++] = chars[ (byte & 0xF0) >> 4 ];
        ret->data[str_pos++] = chars[ (byte & 0x0F) >> 0 ];
    }

    return ret;
}

String_t* format_b64(
    const Bytes_t*   restrict const data,
          String_t** restrict const existing_buffer
) {
    static const char* const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const char pad = '=';

    // 3 bytes = 4 chars, missing bytes add padding
    size_t ret_len = (data->length/3 + (data->length % 3 > 0))*4 + 1;
    String_t* ret = reserve( ret_len, existing_buffer );
    if ( !ret ) { return NULL; }
    ret->data[ret_len - 1] = '\0';

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

        ret->data[str_pos++] = chars[ (a & 0xFC) >> 2 ];
        ret->data[str_pos++] = chars[ (a & 0x03) << 4 | (b & 0xF0) >> 4 ];
        ret->data[str_pos++] = chars[ (b & 0x0F) << 2 | (c & 0xC0) >> 6 ];
        ret->data[str_pos++] = chars[ (c & 0x3F) >> 0 ];
    }
    // 1 extra byte => 2 padding:
    if ( data->length % 3 == 1 ) {
        uint8_t a = data->data[data_pos  ];

        ret->data[str_pos++] = chars[ (a & 0xFC) >> 2 ];
        ret->data[str_pos++] = chars[ (a & 0x03) << 4 ];
        ret->data[str_pos++] = pad;
        ret->data[str_pos  ] = pad;
    // 2 extra bytes => 1 padding:
    } else if ( data->length % 3 == 2 ) {
        uint8_t a = data->data[data_pos++];
        uint8_t b = data->data[data_pos  ];

        ret->data[str_pos++] = chars[ (a & 0xFC) >> 2 ];
        ret->data[str_pos++] = chars[ (a & 0x03) << 4 | (b & 0xF0) >> 4 ];
        ret->data[str_pos++] = chars[ (b & 0x0F) << 2 ];
        ret->data[str_pos  ] = pad;
    }

    return ret;
}

Bytes_t* multi_xor(
    const Bytes_t*  restrict const data,
    const Bytes_t*  restrict const key,
          Bytes_t** restrict const existing_buffer
) {
    Bytes_t* ret = reserve( data->length, existing_buffer );
    if ( !ret ) { return NULL; }

    size_t key_pos = 0;
    for ( size_t i = 0; i < ret->length; ++i ) {
        ret->data[i] = data->data[i] ^ key->data[key_pos];
        key_pos = (key_pos + 1)%(key->length);
    }

    return ret;
}

Bytes_t* single_xor(
    const Bytes_t*  restrict const data,
    const uint8_t                  key,
          Bytes_t** restrict const existing_buffer
) {
    Bytes_t* ret = reserve( data->length, existing_buffer );
    if ( !ret ) { return NULL; }

    for ( size_t i = 0; i < ret->length; ++i ) {
        ret->data[i] = data->data[i] ^ key;
    }

    return ret;
}

Bytes_t* clone(
    const Bytes_t*  restrict const from,
          Bytes_t** restrict const to
) {
    Bytes_t* ret = reserve( from->length, to );
    if ( !ret ) { return NULL; }
    ret->capacity = from->length;
    memcpy( ret->data, from->data, from->length );

    return ret;
}
