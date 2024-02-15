#include "statistics.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h> // file stuff

double score_english_ascii( const Bytes_t text ) {
    static double freqs[256] = {0};
    static bool init = false;
    if ( !init ) {
        init = true;

        // TODO: refactor this; make frequency customizable?
        FILE* freqs_file = fopen("english_ascii_freq.txt", "r");
        if ( !freqs_file ) { return 0; }
        size_t bufflen = 0;
        char* buffer = NULL;

        while ( !feof(freqs_file) ) {
            getline( &buffer, &bufflen, freqs_file );
            size_t i;
            double f;
            sscanf( buffer, "%zu:%lf", &i, &f );
            freqs[i] = f;
        }
        free(buffer);
        fclose(freqs_file);
        // TODO: assert that `freqs` sum to 1.0 +- epsilon
    }

    uint32_t counts[256] = {0};
    for ( size_t i = 0; i < text.length; ++i ) {
        counts[text.data[i]]++;
    }

    double score = 0.0;
    for ( size_t i = 0; i < 256; ++i ) {
        if ( counts[i] != 0 || freqs[i] != 0.0 ) {
            double dif = (double)(counts[i])/text.length - freqs[i];
            score += (dif*dif)/freqs[i];
        }
    }
    return score;
}

uint32_t hamming_distance( const Bytes_t a, const Bytes_t b ) {
    if ( a.length != b.length ) { return 0xFFFFFFFF; } // -1

    uint32_t dist = 0;
    for ( size_t i = 0; i < a.length; ++i ) {
        dist += __builtin_popcount( a.data[i] ^ b.data[i] );
    }
    return dist;
}
