#include <iostream>
#include <verilated.h>
#include "Vmux2.h"
using namespace std;

int main(int argc, char** argv){
    Vmux2 top;

    top.sel = 0;
    top.b = 0;
    top.a = 1;
    top.eval();

    cout << "The select input is " << (int)top.sel << " The B wire value is " << (int)top.b << " The A wire value is " << (int)top.a <<  " The result is " << (int)top.y << endl;

    top.sel = 1;
    top.b = 0;
    top.a = 1;
    top.eval();

    cout << "The select input is " << (int)top.sel << " The B wire value is " << (int)top.b << " The A wire value is " << (int)top.a <<  " The result is " << (int)top.y << endl;

    return 0;
}
