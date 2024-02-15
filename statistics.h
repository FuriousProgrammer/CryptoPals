#pragma once
#include <stdint.h>
#include "Bytes_t.h"

// attempts to measure the "legitimate"-ness of a plaintext blob for a particlar encoding
// uses a chi-squared analysis (lower score is better)
double score_english_ascii( const Bytes_t text );

// computes the hamming distance between 2 byte arrays of the same length
// returns 0xFFFFFFFF (aka -1) if the strings are different lengths
uint32_t hamming_distance( const Bytes_t a, const Bytes_t b );
