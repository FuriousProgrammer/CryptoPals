#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bytes_t.h"

int main() {
    const char* const input1_hex = "1c0111001f010100061a024b53535009181c";
    const char* const input2_hex = "686974207468652062756c6c277320657965";
    const char* const expected_hex = "746865206b696420646f6e277420706c6179";
    // input1 XOR input2 = expected

    String_t str = {0};
    Bytes_t input = {0};
    Bytes_t key = {0};
    Bytes_t result = {0};

    import_cstring( input1_hex, &str );
    import_hex( str, &input );
    import_cstring( input2_hex, &str );
    import_hex( str, &key );

    multi_xor( input, key, &result );
    format_hex( result, &str );

    puts(expected_hex);
    puts((char*)str.data);
    printf("equal: %s\n", strcmp((char*)str.data, expected_hex) == 0 ? "true" : "false" );

    free(str.data);
    free(input.data);
    free(key.data);
    free(result.data);

    return 0;
}
