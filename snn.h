#include "iostream"
// #include "weights.h"


#ifndef uint1_t
#include <ap_int.h>
typedef ap_uint<1> uint1_t;
#endif

#ifndef float24_t
#include <ap_fixed.h>
// typedef float float24_t;
typedef ap_fixed<24, 6, AP_RND, AP_SAT> float24_t;
#endif

void SNN_infer(uint1_t ecg[260], int *result);
