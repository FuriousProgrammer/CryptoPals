#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bytes_t.h"

int main() {
    const char* const input1_hex = "1c0111001f010100061a024b53535009181c";
    const char* const input2_hex = "686974207468652062756c6c277320657965";
    const char* const expected_hex = "746865206b696420646f6e277420706c6179";
    // input1 XOR input2 = expected

    String_t* txt = import_cstring( input1_hex, NULL );
    if ( !txt ) { return 0; }

    Bytes_t* input = import_hex( txt, NULL );
    if ( !input ) { return 0; }

    String_t* tmp = import_cstring( input2_hex, &txt );
    if ( !tmp ) { return 0; }

    Bytes_t* key = import_hex( txt, NULL );
    if ( !key ) { return 0; }

    Bytes_t* data = multi_xor( input, key, NULL );
    if ( !data ) { return 0; }

    String_t* test = format_hex( data, NULL );
    if ( !test ) { return 0; }
    puts(expected_hex);
    puts((char*)test->data);
    printf("equal: %s\n", strcmp((char*)test->data, expected_hex) == 0 ? "true" : "false" );

    free(txt);
    free(input);
    free(key);
    free(data);
    free(test);

    return 0;
}
