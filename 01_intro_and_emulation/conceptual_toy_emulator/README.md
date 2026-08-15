# Conceptual Toy Emulator: From Transistor to Counter in C++

This mini-project demonstrates the exact hierarchy of a digital computer inside a single, clean C++ file with **no external dependencies**.

---

## 🏛️ The Hierarchy Inside `toy_emulator.cpp`

```
 [ Level 4: 4-Bit Binary Counter ]
                 │ (Uses 4 D Flip-Flops & Half Adders)
                 ▼
 [ Level 3: Master-Slave D Flip-Flops & Adders ]
                 │ (Uses Latches and Universal Gates)
                 ▼
 [ Level 2: SR-Latches & Universal Gates (AND / OR / XOR) ]
                 │ (Uses CMOS NAND and NOT gates)
                 ▼
 [ Level 1: CMOS Logic Gates (NAND / NOT) ]
                 │ (Uses NMOS & PMOS Transistor switches)
                 ▼
 [ Level 0: Physical Transistor Switches (NMOS / PMOS) ]
```

---

## 🏃 How to Run

Compile and run with one command:
```bash
make run
```

Or manually:
```bash
g++ -O2 toy_emulator.cpp -o toy_emulator
./toy_emulator
```

---

## 🔍 Things to Notice in the Code:

1. **`nmos_transistor` & `pmos_transistor`**: These are simple conditional functions simulating physical conduction channels when voltage is applied to the gate.
2. **`cmos_nand`**: Implements parallel PMOS pull-up and series NMOS pull-down.
3. **`SRLatchNand`**: Demonstrates the feedback loop of cross-coupled gates creating 1-bit of persistent memory.
4. **`Counter4Bit`**: Shows how synchronous clocking advances state reliably without race conditions.
