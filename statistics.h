#pragma once

#include "Bytes_t.h"

// attempts to measure the "legitimate"-ness of a plaintext blob for a particlar encoding
// uses a chi-squared analysis (lower score is better)
double score_english_ascii( const Bytes_t text );
