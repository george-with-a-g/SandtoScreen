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
    // 2. TESTING A NAND  GATE
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

void testing_and_gate(){
    std::cout << ".....TESTING THE AND GATE....." << std::endl;
    LogicLevel and_out00 = cmos_not(cmos_nand(LOW, LOW));
    std::cout << "AND(0, 0): " << (and_out00 == LOW ? "0V (Success)" : "Fail") << "\n";
    LogicLevel and_out01 = cmos_not(cmos_nand(LOW, HIGH));
    std::cout << "AND(0, 1): " << (and_out01 == LOW ? "0V (Success)" : "Fail") << "\n";
    LogicLevel and_out10 = cmos_not(cmos_nand(HIGH, LOW));
    std::cout << "AND(1, 0): " << (and_out10 == LOW ? "0V (Success)" : "Fail") << "\n";
    LogicLevel and_out11 = cmos_not(cmos_nand(HIGH, HIGH));
    std::cout << "AND(1, 1): " << (and_out11 == HIGH ? "5V (Success)" : "Fail") << "\n";
}

void testing_or_gate(){
    std::cout << ".....TESTING THE OR GATE....." << std::endl;
    LogicLevel or_out00 = cmos_nand(cmos_not(LOW), cmos_not(LOW));
    std::cout << "OR(0, 0): " << (or_out00 == LOW ? "0V (Success)" : "Fail") << "\n";
    LogicLevel or_out01 = cmos_nand(cmos_not(LOW), cmos_not(HIGH));
    std::cout << "OR(0, 1): " << (or_out01 == HIGH ? "5V (Success)" : "Fail") << "\n";
    LogicLevel or_out10 = cmos_nand(cmos_not(HIGH), cmos_not(LOW));
    std::cout << "OR(1, 0): " << (or_out10 == HIGH ? "5V (Success)" : "Fail") << "\n";
    LogicLevel or_out11 = cmos_nand(cmos_not(HIGH), cmos_not(HIGH));
    std::cout << "OR(1, 1): " << (or_out11 == HIGH ? "5V (Success)" : "Fail") << "\n";
}

LogicLevel xor_gate_wrapper(LogicLevel a, LogicLevel b){
    LogicLevel nand_ab = cmos_nand(a, b);
    return cmos_nand(cmos_nand(a, nand_ab), cmos_nand(b, nand_ab));
}
void testing_xor_gate(){
    std::cout << ".....TESTING THE XOR GATE....." << std::endl;
    LogicLevel xor_out00 = xor_gate_wrapper(LOW, LOW);
    std::cout << "XOR(0, 0): " << (xor_out00 == LOW ? "0V (Success)" : "Fail") << "\n";
    LogicLevel xor_out01 = xor_gate_wrapper(LOW, HIGH);
    std::cout << "XOR(0, 1): " << (xor_out01 == HIGH ? "5V (Success)" : "Fail") << "\n";
    LogicLevel xor_out10 = xor_gate_wrapper(HIGH, LOW);
    std::cout << "XOR(1, 0): " << (xor_out10 == HIGH ? "5V (Success)" : "Fail") << "\n";
    LogicLevel xor_out11 = xor_gate_wrapper(HIGH, HIGH);
    std::cout << "XOR(1, 1): " << (xor_out11 == LOW ? "0V (Success)" : "Fail") << "\n";
}

class LUT2 {                                                                                                                                                                           
    private:                                                                                                                                                                               
        uint8_t table; // 4 bits of configuration memory (SRAM)                                                                                                                            
                                                                                                                                                                                           
    public:                                                                                                                                                                                
        // Program the LUT with a 4-bit truth table                                                                                                                                        
        void configure(uint8_t truth_table_bits) {                                                                                                                                         
            table = truth_table_bits & 0x0F; // Keep 4 bits                                                                                                                                
        }                                                                                                                                                                                  
                                                                                                                                                                                           
        // Evaluate based on 2 input wires (A and B act as memory address 0..3)                                                                                                            
        LogicLevel eval(LogicLevel a, LogicLevel b) const {                                                                                                                                
            int address = (a << 1) | b; // 0, 1, 2, or 3                                                                                                                                   
            return (table & (1 << address)) ? HIGH : LOW;                                                                                                                                  
        }                                                                                                                                                                                  
};

void testing_lut(){
    std::cout << ".....TESTING 2-INPUT FPGA LUT....." << std::endl;
    LUT2 lut;

    // 1. Program LUT as XOR Gate (0b0110)
    std::cout << "Programming the LUT as a XOR gate" << std::endl;
    lut.configure(0b0110);
    std::cout << "LUT as XOR(0, 0): " << (lut.eval(LOW, LOW) == LOW ? "0V (Success)" : "Fail") << "\n";
    std::cout << "LUT as XOR(0, 1): " << (lut.eval(LOW, HIGH) == HIGH ? "5V (Success)" : "Fail") << "\n";
    std::cout << "LUT as XOR(1, 0): " << (lut.eval(HIGH, LOW) == HIGH ? "5V (Success)" : "Fail") << "\n";
    std::cout << "LUT as XOR(1, 1): " << (lut.eval(HIGH, HIGH) == LOW ? "0V (Success)" : "Fail") << "\n";

    // 2. Program LUT as NOR Gate (0b0001)
    std::cout << "Programming the LUT as a NOR gate" << std::endl;
    lut.configure(0b0001);
    std::cout << "LUT as NOR(0, 0): " << (lut.eval(LOW, LOW) == HIGH ? "5V (Success)" : "Fail") << "\n";
    std::cout << "LUT as NOR(0, 1): " << (lut.eval(LOW, HIGH) == LOW ? "0V (Success)" : "Fail") << "\n";
    std::cout << "LUT as NOR(1, 0): " << (lut.eval(HIGH, LOW) == LOW ? "0V (Success)" : "Fail") << "\n";
    std::cout << "LUT as NOR(1, 1): " << (lut.eval(HIGH, HIGH) == LOW ? "0V (Success)" : "Fail") << "\n";

    // 3. Program LUT as AND Gate (0b1000)
    std::cout << "Programming the LUT as a AND gate" << std::endl;
    lut.configure(0b1000);
    std::cout << "LUT as AND(0, 0): " << (lut.eval(LOW, LOW) == LOW ? "0V (Success)" : "Fail") << "\n";
    std::cout << "LUT as AND(0, 1): " << (lut.eval(LOW, HIGH) == LOW ? "0V (Success)" : "Fail") << "\n";
    std::cout << "LUT as AND(1, 0): " << (lut.eval(HIGH, LOW) == LOW ? "0V (Success)" : "Fail") << "\n";
    std::cout << "LUT as AND(1, 1): " << (lut.eval(HIGH, HIGH) == HIGH ? "5V (Success)" : "Fail") << "\n";
}

int main() {
    //testing_not_gate();
    //testing_nand_gate();
    //testing_and_gate();
    //testing_or_gate();
    //testing_xor_gate();
    testing_lut();
    return 0;
}
