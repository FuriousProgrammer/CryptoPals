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
    Bytes_t* min_cyphertext = NULL;

    String_t* input_cstr = NULL;
    Bytes_t* input = NULL;
    Bytes_t* plain = NULL;

    while ( !feof(datafile) ) {
        size_t read = getline( &buffer, &bufflen, datafile );
        if ( read >= 1 ) { buffer[read - 1] = '\0'; } // remove trailing LF

        String_t* tmp = import_cstring( buffer, &input_cstr );
        if ( !tmp ) { return 0; }
        tmp = import_hex( input_cstr, &input );
        if ( !tmp ) { return 0; }

        if ( !min_plaintext ) { // all lines in challenge 4 are the exact same length
            Bytes_t* tmp = clone( input, &min_cyphertext );
            if ( !tmp ) { return 0; }
            min_plaintext = malloc( sizeof(*min_plaintext)*(input->length + 1) );
            if ( !min_plaintext ) { return 0; }
            min_plaintext[input->length] = '\0';
        }

        for ( uint8_t key = 1; key != 0; ++key ) { // 1..255, overflows to 0 and stops
            tmp = single_xor( input, key, &plain );
            if ( !tmp ) { return 0; }
            double score = score_english_ascii( plain );
            if ( score < min_score ) {
                Bytes_t* tmp = clone( input, &min_cyphertext );
                if ( !tmp ) { return 0; }
                memcpy( min_plaintext, plain->data, plain->length );
                min_score = score;
                min_key = key;
            }
        }
    }

    String_t* hexout = format_hex( min_cyphertext, NULL );

    printf("[%lf] 0x%x : [%s]\nCyphertext: %s\n", min_score, min_key, min_plaintext, hexout->data);

    fclose(datafile);

    free(hexout);
    free(plain);
    free(buffer);
    free(min_cyphertext);
    free(min_plaintext);
    free(input);
    free(input_cstr);
    return 0;
}
