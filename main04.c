#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Bytes_t.h"
#include "statistics.h"

int main() {
    FILE* datafile = fopen("challenge-4.txt", "r");
    if ( !datafile ) { return 0; }

    size_t bufflen = 0;
    char* buffer = NULL;

    uint8_t min_key = 0x00;
    double min_score = HUGE_VAL;
    char* min_plaintext = NULL;
    Bytes_t min_cyphertext = {0};

    String_t input_cstr = {0};
    Bytes_t input = {0};
    Bytes_t plain = {0};

    String_t hexout = {0};

    while ( !feof(datafile) ) {
        ssize_t read = getline( &buffer, &bufflen, datafile );
        if ( read >= 1 && buffer[read - 1] == '\n' ) { buffer[read - 1] = '\0'; } // remove trailing LF

        import_cstring( buffer, &input_cstr );
        import_hex( input_cstr, &input );

        if ( !min_plaintext ) { // all lines in challenge 4 are the exact same length
            clone( input, &min_cyphertext );
            min_plaintext = malloc( sizeof(*min_plaintext)*(input.length + 1) );
            min_plaintext[input.length] = '\0';
        }

        for ( uint8_t key = 1; key != 0; ++key ) { // 1..255, overflows to 0 and stops
            single_xor( input, key, &plain );
            double score = score_english_ascii( plain );
            if ( score < min_score ) {
                clone( input, &min_cyphertext );
                memcpy( min_plaintext, plain.data, plain.length );
                min_score = score;
                min_key = key;
            }
        }
    }

    format_hex( min_cyphertext, &hexout );
    printf("[%lf] 0x%x : [%s]\nCyphertext: %s\n", min_score, min_key, min_plaintext, hexout.data);

    fclose(datafile);

    free(hexout.data);
    free(plain.data);
    free(buffer);
    free(min_cyphertext.data);
    free(min_plaintext);
    free(input.data);
    free(input_cstr.data);
    return 0;
}
