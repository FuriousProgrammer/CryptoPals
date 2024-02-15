#include <stdio.h>
#include <stdlib.h>

#include "Bytes_t.h"
#include "statistics.h"

int main() {
    String_t a = {0};
    String_t b = {0};

    import_cstring( "this is a test", &a );
    import_cstring( "wokka wokka!!!", &b );
    printf("Hamming distance between [%s] and [%s]:\nExpected: 37, Actual: %d\n\n", a.data, b.data, hamming_distance(a, b));

    free(a.data);
    free(b.data);
}
