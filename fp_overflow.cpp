#include <iostream>
#include <bitset>
#include <cstring>
#include <cmath>
#include <iomanip>

using namespace std;

union FloatBits {
    float f;
    uint32_t bits;
};

// function to print IEEE 754 representation using bitset
void printIEEE(float value, const string& label) {
    FloatBits fb;
    fb.f = value;
    
    // extract components using bitset
    bitset<32> bits(fb.bits);
    
    cout << label << ": " << fixed << setprecision(8) << value << endl;
    cout << "0b ";
    
    // print sign bit (bit 31)
    cout << bits[31] << " ";
    
    // print exponent bits (bits 30-23)
    for (int i = 30; i >= 23; i--) {
        cout << bits[i];
        if (i == 27) cout << " "; // add space after 4 bits
    }
    cout << " ";
    
    // print mantissa bits (bits 22-0) with spaces every 4 bits
    for (int i = 22; i >= 0; i--) {
        cout << bits[i];
        if (i % 4 == 0 && i > 0) cout << " ";
    }
    cout << endl;
}

// extract unbiased exponent using bitset
int getExponent(float value) {
    FloatBits fb;
    fb.f = value;
    bitset<32> bits(fb.bits);
    
    // extract exponent bits (bits 30-23) and convert to int
    int exponent = 0;
    for (int i = 30; i >= 23; i--) {
        exponent = (exponent << 1) | bits[i];
    }
    
    // subtract bias (127 for single precision)
    return exponent - 127;
}

// check if overflow will occur
bool willOverflow(float bound, float increment) {
    int boundExp = getExponent(bound);
    int incExp = getExponent(increment);
    
    // overflow occurs when exponent difference >= 24
    // (23 mantissa bits + 1 for implicit leading bit)
    return (boundExp - incExp) >= 24;
}

// calculate threshold value
float calculateThreshold(float bound, float increment) {
    int boundExp = getExponent(bound);
    int incExp = getExponent(increment);
    
    int expDiff = boundExp - incExp;
    
    if (expDiff >= 24) {
        // the threshold is the value where the increment becomes insignificant
        // it will have a mantissa of all zeros
        return ldexp(1.0f, expDiff) * increment;
    }
    
    return 0.0f;
}

int main(int argc, char* argv[]) {
    // check for exactly 2 arguments (program name + 2 values)
    if (argc < 3) {
        cerr << "Error: Please provide exactly two floating-point values." << endl;
        return 1;
    }
    else if (argc > 3) {
        cerr << "Error: Please provide exactly two floating-point values." << endl;
        return 1;
    }
    
    // parse input values
    float bound = atof(argv[1]);
    float increment = atof(argv[2]);
    
    // print IEEE representations
    printIEEE(bound, "Bound value");
    printIEEE(increment, "Increment value");
    
    // check for overflow and display results
    if (willOverflow(bound, increment)) {
        cout << "Warning: Possible overflow!" << endl;
        cout << "Overflow threshold:" << endl;
        
        float threshold = calculateThreshold(bound, increment);
        
        // print threshold in floating point notation
        cout << fixed << setprecision(8) << threshold << endl;
        
        // print threshold in bit representation
        FloatBits fb;
        fb.f = threshold;
        bitset<32> bits(fb.bits);
        
        cout << "0b ";
        cout << bits[31] << " ";
        for (int i = 30; i >= 23; i--) {
            cout << bits[i];
            if (i == 27) cout << " ";
        }
        cout << " ";
        for (int i = 22; i >= 0; i--) {
            cout << bits[i];
            if (i % 4 == 0 && i > 0) cout << " ";
        }
        cout << endl;
    }
    else {
        cout << "No overflow!" << endl;
    }
    
    return 0;
}
