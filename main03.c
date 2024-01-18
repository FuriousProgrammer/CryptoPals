#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Bytes_t.h"
#include "statistics.h"

int main() {
    const char* const input_hex = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    String_t* txt = import_cstring( input_hex, NULL );
    if ( !txt ) { return 0; }

    Bytes_t* input = import_hex( txt, NULL );
    if ( !input ) { return 0; }

    Bytes_t* plain = NULL;

    uint8_t min_key = 0x00;
    double min_score = score_english_ascii( input );
    char* min_phrase = malloc( sizeof(*min_phrase)*(input->length + 1) );
    min_phrase[input->length] = '\0';
    memcpy( min_phrase, input->data, input->length );

    for ( uint8_t key = 1; key != 0; ++key ) { // 1..255, overflows to 0 and stops
        Bytes_t* tmp = single_xor( input, key, &plain );
        if ( !tmp ) { return 0; }
        double score = score_english_ascii( plain );
        if ( score < min_score ) {
            memcpy( min_phrase, plain->data, plain->length );
            min_score = score;
            min_key = key;
        }
    }

    printf("[%lf] 0x%x : [%s]\n", min_score, min_key, min_phrase);

    free(plain);
    free(min_phrase);
    free(input);
    free(txt);
    return 0;
}
