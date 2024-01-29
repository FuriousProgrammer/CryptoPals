#include "Bytes_t.h"

#include <stdlib.h>
#include <string.h>

// reserves new buffer space, handling `malloc` and `realloc` as necessary
static
Bytes_Error_e reserve( size_t new_length, Bytes_t* const buffer ) {
    if ( buffer->capacity < new_length ) {
        void* new_ptr = realloc( buffer->data, sizeof(*buffer->data)*new_length );
        if ( !new_ptr ) { return Bytes_ALLOC_FAILURE; }

        buffer->data = new_ptr;
        buffer->capacity = new_length;
    }
    buffer->length = new_length;
    return Bytes_SUCCESS;
}

#include <stdio.h>

// assumes `c_str` is a valid C string
Bytes_Error_e import_cstring(
    const char*     restrict const c_str,
          String_t* restrict const out_buffer
) {
    size_t ret_len = strlen(c_str);
    Bytes_Error_e err = reserve( ret_len + 1, out_buffer );
    if ( err != Bytes_SUCCESS ) { return err; }
    out_buffer->data[ret_len] = '\0';
    out_buffer->length--; // NULL terminator is not part of C string

    memcpy( out_buffer->data, c_str, ret_len );

    return Bytes_SUCCESS;
}

// TODO: convert this to a LUT? use SIMD for import funcs?
static
uint8_t hexit2nybble( const char c ) {
    if ( c >= '0' && c <= '9' ) {
        return c - '0';
    } else if ( c >= 'a' && c <= 'f' ) {
        return c - 'a' + 0xA;
    } else if ( c >= 'A' && c <= 'F' ) {
        return c - 'A' + 0xA;
    } else {
        return 0xFF; // -1
    }
}

Bytes_Error_e import_hex(
    const String_t       hexstr,
          Bytes_t* const out_buffer
) {
    // each hexit is 4 bits, `(len + 1)/2` handles an odd hexstr length
    size_t ret_len = (hexstr.length + 1)/2;
    uint8_t* data = malloc( sizeof(*data)*ret_len ); // don't want to modify `out_buffer` for invalid encoding!
    if ( !data ) { return Bytes_ALLOC_FAILURE; }
    // TODO: be more efficient than `malloc` + `free` every call

    size_t data_pos = 0;
    size_t str_pos = 0;
    if ( hexstr.length%2 == 1 ) { // special case first nybble of odd-length hex string
        uint8_t nyb = hexit2nybble( hexstr.data[str_pos++] );
        if ( nyb == 0xFF ) {
            free(data);
            return Bytes_INVALID_HEX_STRING;
        }
        data[data_pos++] = nyb;
    }
    while ( str_pos < hexstr.length ) {
        uint8_t hi = hexit2nybble( hexstr.data[str_pos++] );
        uint8_t lo = hexit2nybble( hexstr.data[str_pos++] );
        if ( hi == 0xFF || lo == 0xFF ) {
            free(data);
            return Bytes_INVALID_HEX_STRING;
        }
        data[data_pos++] = (hi << 4) | lo;
    }


    Bytes_Error_e err = reserve( ret_len, out_buffer );
    if ( err != Bytes_SUCCESS ) { return err; }

    memcpy( out_buffer->data, data, ret_len );
    free(data);

    return Bytes_SUCCESS;
}

Bytes_Error_e format_hex(
    const Bytes_t         data,
          String_t* const out_buffer
) {
    static const char* const chars = "0123456789abcdef";

    size_t ret_len = data.length*2;
    Bytes_Error_e err = reserve( ret_len + 1, out_buffer );
    if ( err != Bytes_SUCCESS ) { return err; }
    out_buffer->data[ret_len] = '\0';

    size_t data_pos = 0;
    size_t str_pos = 0;
    while ( data_pos < data.length ) {
        uint8_t byte = data.data[data_pos++];
        out_buffer->data[str_pos++] = chars[ (byte & 0xF0) >> 4 ];
        out_buffer->data[str_pos++] = chars[ (byte & 0x0F) >> 0 ];
    }

    return Bytes_SUCCESS;
}

Bytes_Error_e format_b64(
    const Bytes_t         data,
          String_t* const out_buffer
) {
    static const char* const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const char pad = '=';

    // 3 bytes = 4 chars, missing bytes add padding
    size_t ret_len = (data.length/3 + (data.length % 3 > 0))*4;
    Bytes_Error_e err = reserve( ret_len + 1, out_buffer );
    if ( err != Bytes_SUCCESS ) { return err; }
    out_buffer->data[ret_len] = '\0';

    //  Byte A   Byte B   Byte C
    // 76543210 76543120 76543210
    // |----||-----||-----||----|
    //  chr1  chr2    chr3  chr4

    size_t data_pos = 0;
    size_t str_pos = 0;

    // complete octet triplets:
    for ( size_t i = 0; i < data.length/3; ++i ) {
        uint8_t a = data.data[data_pos++];
        uint8_t b = data.data[data_pos++];
        uint8_t c = data.data[data_pos++];

        out_buffer->data[str_pos++] = chars[ (a & 0xFC) >> 2 ];
        out_buffer->data[str_pos++] = chars[ (a & 0x03) << 4 | (b & 0xF0) >> 4 ];
        out_buffer->data[str_pos++] = chars[ (b & 0x0F) << 2 | (c & 0xC0) >> 6 ];
        out_buffer->data[str_pos++] = chars[ (c & 0x3F) >> 0 ];
    }
    // 1 extra byte => 2 padding:
    if ( data.length % 3 == 1 ) {
        uint8_t a = data.data[data_pos  ];

        out_buffer->data[str_pos++] = chars[ (a & 0xFC) >> 2 ];
        out_buffer->data[str_pos++] = chars[ (a & 0x03) << 4 ];
        out_buffer->data[str_pos++] = pad;
        out_buffer->data[str_pos  ] = pad;
    // 2 extra bytes => 1 padding:
    } else if ( data.length % 3 == 2 ) {
        uint8_t a = data.data[data_pos++];
        uint8_t b = data.data[data_pos  ];

        out_buffer->data[str_pos++] = chars[ (a & 0xFC) >> 2 ];
        out_buffer->data[str_pos++] = chars[ (a & 0x03) << 4 | (b & 0xF0) >> 4 ];
        out_buffer->data[str_pos++] = chars[ (b & 0x0F) << 2 ];
        out_buffer->data[str_pos  ] = pad;
    }

    return Bytes_SUCCESS;
}

Bytes_Error_e multi_xor(
    const Bytes_t        data,
    const Bytes_t        key,
          Bytes_t* const out_buffer
) {
    if ( key.length == 0 ) { return Bytes_MULTI_XOR_EMPTY_KEY; }

    Bytes_Error_e err = reserve( data.length, out_buffer );
    if ( err != Bytes_SUCCESS ) { return err; }

    size_t key_pos = 0;
    for ( size_t i = 0; i < out_buffer->length; ++i ) {
        out_buffer->data[i] = data.data[i] ^ key.data[key_pos];
        key_pos = (key_pos + 1)%(key.length);
    }

    return Bytes_SUCCESS;
}

Bytes_Error_e single_xor(
    const Bytes_t        data,
    const uint8_t        key,
          Bytes_t* const out_buffer
) {
    Bytes_Error_e err = reserve( data.length, out_buffer );
    if ( err != Bytes_SUCCESS ) { return err; }

    for ( size_t i = 0; i < out_buffer->length; ++i ) {
        out_buffer->data[i] = data.data[i] ^ key;
    }

    return Bytes_SUCCESS;
}

Bytes_Error_e clone(
    const Bytes_t        from,
          Bytes_t* const to
) {
    Bytes_Error_e err = reserve( from.length, to );
    if ( err != Bytes_SUCCESS ) { return err; }
    memcpy( to->data, from.data, from.length );

    return Bytes_SUCCESS;
}
