#include <iostream>
#include <verilated.h>
#include "Vinverter.h"
using namespace std;

int main(int argc, char** argv){
    cout << argc << " - Number of arguments(argc)" <<  endl;
    for (int i = 0; i < argc; i++) {
        cout << "argv[" << i << "] = " << argv[i] << "\n";
    }

    Vinverter top;

    // Test 1: Put 0V on the 'in' pin
    top.in = 0;
    top.eval();
    cout << "Inputs: 0 --> Output: " << (int)top.out << "(Expected: 1)\n";

    // Test 2: Put 5V on the 'in' pin
    top.in = 1;
    top.eval();
    cout << "Inputs: 1 --> Output: " << (int)top.out << "(Expected: 0)\n";

    return 0;
}
