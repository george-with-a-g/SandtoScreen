#include <iostream>
#include <verilated.h>
#include "Vhalf_adder.h"
using namespace std;

int main(int argc, char** argv){
    Vhalf_adder top;

    // Test 1: Put OV on the a, b pins
    top.a = 0;
    top.b = 0;
    top.eval();

    cout << "Inputs are (0,0) --> Output sum is: " << (int)top.sum << "(Expected: 0)\n";
    cout << "Inputs are (0,0) --> Output carry is: " << (int)top.carry << "(Expected: 0)\n";

    // Test 2: Put OV on the a, 5V on the b pins
    top.a = 0;
    top.b = 1;
    top.eval();

    cout << "Inputs are (0,1) --> Output sum is: " << (int)top.sum << "(Expected: 1)\n";
    cout << "Inputs are (0,1) --> Output carry is: " << (int)top.carry << "(Expected: 0)\n";

    // Test 3: Put 5V on the a, 0V on the b pins
    top.a = 1;
    top.b = 0;
    top.eval();

    cout << "Inputs are (1,0) --> Output sum is: " << (int)top.sum << "(Expected: 1)\n";
    cout << "Inputs are (1,0) --> Output carry is: " << (int)top.carry << "(Expected: 0)\n";

    // Test 4: Put 5V on the a, 5V on the b pins
    top.a = 1;
    top.b = 1;
    top.eval();

    cout << "Inputs are (1,1) --> Output sum is: " << (int)top.sum << "(Expected: 0)\n";
    cout << "Inputs are (1,1) --> Output carry is: " << (int)top.carry << "(Expected: 1)\n";
    return 0;
}
