#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bytes_t.h"

int main() {
    const char* input_hex = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    const char* expected_b64 = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

    Bytes_t* data = import_hex( input_hex );
    if ( !data ) { return 0; } // TODO: logging

    char* test = format_hex( data );
    if ( !test) { return 0; } // TODO: logging
    puts(input_hex);
    puts(test);
    printf("equal: %s\n", strcmp(test, input_hex) == 0 ? "true" : "false" );

    free(test);

    test = format_b64( data );
    if ( !test ) { return 0; } // TODO: logging

    puts(expected_b64);
    puts(test);
    printf("equal: %s\n", strcmp(test, expected_b64) == 0 ? "true" : "false" );

    free(data);
    free(test);

    return 0;
}