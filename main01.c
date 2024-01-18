#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bytes_t.h"

int main() {
    const char* const input_hex = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    const char* const expected_b64 = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

    String_t* input = import_cstring( input_hex, NULL );
    if ( !input ) { return 0; }

    Bytes_t* data = import_hex( input, NULL );
    if ( !data ) { return 0; }

    String_t* test = format_hex( data, NULL );
    if ( !test ) { return 0; }
    puts(input_hex);
    puts((char*)test->data);
    printf("equal: %s\n", strcmp((char*)test->data, input_hex) == 0 ? "true" : "false" );


    String_t* tmp = format_b64( data, &test );
    if ( !tmp ) { return 0; }

    puts(expected_b64);
    puts((char*)test->data);
    printf("equal: %s\n", strcmp((char*)test->data, expected_b64) == 0 ? "true" : "false" );

    free(input);
    free(data);
    free(test);

    return 0;
}
