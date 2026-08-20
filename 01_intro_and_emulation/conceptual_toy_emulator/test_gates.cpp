#include <iostream>
#include "toy_emulator.hpp"

void testing_not_gate(){
    // 1. TESTING A NOT GATE
    std::cout << ".....TESTING THE NOT GATE....." << std::endl;
    std::cout << "Giving input for NOT gate as HIGH" << std::endl;
    LogicLevel not_out = cmos_not(HIGH); // Will return LOW
    std::cout << "NOT GATE: " << (not_out == LOW ? "0V (Success)" : "Fail") << "\n";
    std::cout << "Changing input for NOT gate to LOW" << std::endl;
    LogicLevel not_out_alternate = cmos_not(LOW); // Will return HIGH
    std::cout << "NOT GATE: " << (not_out_alternate == HIGH ? "5V (Success)" : "Fail") << "\n";
}

void testing_nand_gate(){
    // 2. TESTING AN AND  GATE
    std::cout << ".....TESTING THE NAND GATE....." << std::endl;
    // Case 1: (0, 0) -> HIGH (5V)
    LogicLevel nand_out00 = cmos_nand(LOW, LOW);
    std::cout << "NAND(0, 0): " << (nand_out00 == HIGH ? "5V (Success)" : "Fail") << "\n";
    // Case 2: (0, 1) -> HIGH (5V)
    LogicLevel nand_out01 = cmos_nand(LOW, HIGH);
    std::cout << "NAND(0, 1): " << (nand_out01 == HIGH ? "5V (Success)" : "Fail") << "\n";
    // Case 3: (1, 0) -> HIGH (5V)
    LogicLevel nand_out10 = cmos_nand(HIGH, LOW);
    std::cout << "NAND(1, 0): " << (nand_out10 == HIGH ? "5V (Success)" : "Fail") << "\n";
    // Case 4: (1, 1) -> LOW (0V) [Both series NMOS turn ON!]
    LogicLevel nand_out11 = cmos_nand(HIGH, HIGH);
    std::cout << "NAND(1, 1): " << (nand_out11 == LOW ? "0V (Success)" : "Fail") << "\n";
}


int main() {

    testing_not_gate();
    //testing_nand_gate();
    return 0;
}
