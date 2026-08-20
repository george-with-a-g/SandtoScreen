#include <iostream>
#include "toy_emulator.hpp"

int main() {
    // 1. TESTING A NOT GATE
    std::cout << ".....TESTING THE NOT GATE....." << std::endl;
    std::cout << "Giving input for NOT gate as HIGH" << std::endl;
    LogicLevel not_out = cmos_not(HIGH); // Will return LOW
    std::cout << "NOT GATE: " << (not_out == LOW ? "0V (Success)" : "Fail") << "\n";
    std::cout << "Changing input for NOT gate to LOW" << std::endl;
    LogicLevel not_out_alternate = cmos_not(LOW); // Will return HIGH
    std::cout << "NOT GATE: " << (not_out_alternate == HIGH ? "5V (Success)" : "Fail") << "\n";

    std::cout << ".....TESTING THE AND GATE....." << std::endl;
    /*
    // 2. Test CMOS NAND Gate
    LogicLevel nand_out = cmos_nand(HIGH, HIGH);
    std::cout << "NAND(1, 1): " << nand_out << " (Expected: 0)\n";

    // 3. Test Half Adder
    AdderResult res = half_adder(HIGH, HIGH);
    std::cout << "Half Adder 1 + 1 -> Sum: " << res.sum 
              << ", Carry: " << res.carry_out << " (Expected: Sum 0, Carry 1)\n";
    */

    return 0;
}
