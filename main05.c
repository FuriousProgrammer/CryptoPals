#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bytes_t.h"

int main() {
    Bytes_t data = {0};
    import_cstring( "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal", &data );
    Bytes_t xor_key = {0};
    import_cstring( "ICE", &xor_key );

    Bytes_t result = {0};
    multi_xor( data, xor_key, &result );

    const char* expected_hex = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";
    String_t result_hex = {0};
    format_hex( result, &result_hex );

    puts(expected_hex);
    puts((char*)result_hex.data);
    printf("equal: %s\n", strcmp((char*)result_hex.data, expected_hex) == 0 ? "true" : "false" );

    free(data.data);
    free(xor_key.data);
    free(result.data);
    free(result_hex.data);
    return 0;
}
