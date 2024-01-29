#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bytes_t.h"

int main() {
    const char* const input_hex = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    const char* const expected_b64 = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

    String_t str = {0};
    Bytes_t data = {0};
    import_cstring( input_hex, &str );
    import_hex( str, &data );


    format_hex( data, &str );

    puts(input_hex);
    puts((char*)str.data);
    printf("equal: %s\n", strcmp((char*)str.data, input_hex) == 0 ? "true" : "false" );

    format_b64( data, &str );

    puts(expected_b64);
    puts((char*)str.data);
    printf("equal: %s\n", strcmp((char*)str.data, expected_b64) == 0 ? "true" : "false" );

    free(data.data);
    free(str.data);

    return 0;
}
