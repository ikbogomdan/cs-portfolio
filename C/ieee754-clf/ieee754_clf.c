#include <stdint.h>
#include <string.h>

#include "ieee754_clf.h"

float_class_t classify(double x) { 
    uint64_t t; 
    uint8_t sign_ = 0;
    uint8_t exp_ = 0;
    uint8_t mant_ = 0;
    
    memcpy(&t, &x, sizeof(x));  

    { 
        if (t & (1ULL << 63)) {
            ++sign_;
        }
    }
    { 
        for (int i = 62; i > 51; --i) { 
            if (t & (1ULL << i)) {
                ++exp_;
            }
        }
    }
    { 
        for (int i = 51; i >= 0; --i) { 
            if (t & (1ULL << i)) {
                ++mant_;
            }
        }
    }

    if (exp_ == 0 && mant_ == 0) {
        if (sign_ == 1) {
            return MinusZero;
        }
        return Zero;
    }

    if (exp_ == 11 && mant_ == 0) {
        if (sign_ == 1) {
            return MinusInf;
        }
        return Inf;
    }

    if (exp_ != 0 && exp_ != 11) {
        if (sign_ == 1) {
            return MinusRegular;
        }
        return Regular;
    }

    if (exp_ == 0 && mant_ != 0) {
        if (sign_ == 1) {
            return MinusDenormal;
        }
        return Denormal;
    }

    return NaN;
}

