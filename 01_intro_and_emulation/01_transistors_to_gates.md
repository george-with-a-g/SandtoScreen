# 01. From Switches to Logic Gates: How Computers Think

> **Goal:** Understand how physical electricity turns into logic ("YES", "NO", "AND", "OR") without drowning in engineering jargon.

---

## 📖 Jargon Buster (Read this first!)

Whenever you see a fancy word in hardware, here is what it actually means:

| Jargon Term | What it actually means in plain English |
| :--- | :--- |
| **`1` (HIGH / VDD)** | **Electricity is ON** (like a light bulb turned on, typically 5 Volts or 1.2 Volts). |
| **`0` (LOW / GND / Ground)** | **Electricity is OFF** (0 Volts, wire connected to the ground/earth). |
| **Transistor** | A **microscopic switch** operated by electricity instead of your finger. |
| **Logic Gate** | A simple **decision maker** made of switches (e.g., "If A AND B are on, turn on the light"). |
| **Truth Table** | A simple **cheat-sheet table** showing what the output will be for every possible input. |
| **Series** | Switches connected **one after another in a single line**. (Both must be closed for electricity to pass). |
| **Parallel** | Switches connected **side-by-side on alternative paths**. (Either one can let electricity pass). |

---

## 1. What is a 1 and a 0?

Computers do not understand numbers, letters, or videos. Computers are just electrical circuits.

Inside a wire, there can only be two states:
- **Voltage is flowing** -> We call this **`1`** (or **HIGH** / **TRUE**).
- **No voltage (0 Volts)** -> We call this **`0`** (or **LOW** / **FALSE**).

That's the entire foundation of digital computers! Everything else is built on top of this.

---

## 2. What is a Transistor? (The Automatic Switch)

Think of a regular light switch on your bedroom wall:
- You flip it **UP** with your finger -> the light turns **ON**.
- You flip it **DOWN** -> the light turns **OFF**.

A **Transistor** is the exact same light switch, but **you don't use your finger**.
Instead, you apply a tiny voltage to a control wire (called the **Gate**):

```
       Control Wire (Gate)
              │
              ▼  (Put electricity here...)
       ───[ SWITCH ]───
              ▲
              │
    (...and it connects the main wire, letting electricity flow!)
```

There are two common kinds of transistor switches:
1. **Normal Switch (NMOS):** Turns **ON** when you send it a `1` (voltage). Turns **OFF** when you give it `0`.
2. **Reverse Switch (PMOS):** Turns **ON** when you give it a `0` (no voltage). Turns **OFF** when you give it `1`.

By combining these two simple switches, we can build every **Logic Gate** in the world.

---

## 3. The 6 Basic Logic Gates (Explained with Everyday Examples)

A **Logic Gate** takes one or two input wires (`0` or `1`) and produces an output wire (`0` or `1`) based on a simple rule.

---

### Gate 1: The NOT Gate (The Inverter / "Opposite Day")
* **Rule:** Gives the exact opposite of whatever you give it.
* **Real-world example:** "If it is raining, I do NOT go outside."

```
Input A ───[ NOT ]─── Output
```

#### Truth Table:
| Input A | Output |
| :---: | :---: |
| `0` (OFF) | **`1` (ON)** |
| `1` (ON) | **`0` (OFF)** |

---

### Gate 2: The AND Gate (The Strict Parent)
* **Rule:** Output is `1` **ONLY IF BOTH** Input A **AND** Input B are `1`.
* **Real-world example:** "You can play video games ONLY IF you finished homework **AND** cleaned your room."

```
Input A ──┐
          ├──[ AND ]─── Output
Input B ──┘
```

#### Truth Table:
| Input A | Input B | Output |
| :---: | :---: | :---: |
| `0` | `0` | **`0`** |
| `0` | `1` | **`0`** |
| `1` | `0` | **`0`** |
| `1` | `1` | **`1`** (Only when both are 1) |

---

### Gate 3: The OR Gate (The Friendly Host)
* **Rule:** Output is `1` if **EITHER** Input A **OR** Input B is `1` (or both).
* **Real-world example:** "You can enter the club if you have a VIP Pass **OR** you bought a Ticket."

```
Input A ──┐
          ├──[ OR ]─── Output
Input B ──┘
```

#### Truth Table:
| Input A | Input B | Output |
| :---: | :---: | :---: |
| `0` | `0` | **`0`** |
| `0` | `1` | **`1`** |
| `1` | `0` | **`1`** |
| `1` | `1` | **`1`** |

---

### Gate 4: The NAND Gate (NOT-AND: The Universal Building Block)
* **Rule:** It is simply an **AND** gate followed by a **NOT** gate. It outputs `0` **only** if both inputs are `1`. Otherwise, it outputs `1`.
* **Real-world example:** "A bank safe alarm stays QUIET (`1`), UNLESS both robbers step on the pressure plates at the same time (`0`)."

```
Input A ──┐
          ├──[ AND ]──o─── Output (The little circle 'o' means NOT)
Input B ──┘
```

#### Truth Table:
| Input A | Input B | AND Result | NAND Output (Inverted) |
| :---: | :---: | :---: | :---: |
| `0` | `0` | `0` | **`1`** |
| `0` | `1` | `0` | **`1`** |
| `1` | `0` | `0` | **`1`** |
| `1` | `1` | `1` | **`0`** |

> **🌟 Why NAND is famous:** NAND is called the **Universal Gate**. You can build every other gate (NOT, AND, OR, XOR, CPU adders, memory) using **only NAND gates**!

---

### Gate 5: The NOR Gate (NOT-OR)
* **Rule:** An **OR** gate followed by a **NOT** gate. Outputs `1` only if **NEITHER** input is active (`0` and `0`).

#### Truth Table:
| Input A | Input B | OR Result | NOR Output (Inverted) |
| :---: | :---: | :---: | :---: |
| `0` | `0` | `0` | **`1`** |
| `0` | `1` | `1` | **`0`** |
| `1` | `0` | `1` | **`0`** |
| `1` | `1` | `1` | **`0`** |

---

### Gate 6: The XOR Gate (Exclusive OR / "Pick One")
* **Rule:** Output is `1` if the inputs are **DIFFERENT** (one is `1`, the other is `0`). If both are the same, output is `0`.
* **Real-world example:** A restaurant meal combo: "You can choose Soup **OR** Salad, but you cannot have both."
* **Why it matters:** XOR is the heart of **Binary Addition** (1 + 0 = 1, but 1 + 1 = 0 with a carry)!

```
Input A ──┐
          ├──[ XOR ]─── Output
Input B ──┘
```

#### Truth Table:
| Input A | Input B | Output |
| :---: | :---: | :---: |
| `0` | `0` | **`0`** (Same) |
| `0` | `1` | **`1`** (Different) |
| `1` | `0` | **`1`** (Different) |
| `1` | `1` | **`0`** (Same) |

---

## 4. How Physical Switches Form Logic

Let's look at how switches physically create these rules with a battery and a light bulb:

### A. The AND Connection (Series)
To light the bulb, electricity must flow through Switch A **AND** Switch B:

```
Battery (+) ────[ Switch A ]────[ Switch B ]────( Light Bulb )──── Battery (-)
```
- If Switch A is open: Bulb is OFF.
- If Switch B is open: Bulb is OFF.
- Only when **BOTH A AND B are closed** does the bulb turn ON!

### B. The OR Connection (Parallel)
Electricity has two possible paths. It can flow through Switch A **OR** Switch B:

```
                  ┌───[ Switch A ]───┐
Battery (+) ──────┤                  ├────( Light Bulb )──── Battery (-)
                  └───[ Switch B ]───┘
```
- If Switch A is closed: Bulb turns ON.
- If Switch B is closed: Bulb turns ON.
- If both are closed: Bulb is ON.
- Only if **both are open** is the bulb OFF.

---

## 5. How Microchips Do It: The CMOS Inverter (NOT Gate)

In real computer chips, we don't have mechanical switches. We have microscopic transistors.

To make a **NOT Gate**:
1. Place a **Reverse Switch (PMOS)** connected to Power on top.
2. Place a **Normal Switch (NMOS)** connected to Ground on bottom.

```
                  Power (1 / HIGH)
                         │
                  ┌──────┴──────┐
                  │ Top Switch  │ (Turns ON when Input is 0)
                  └──────┬──────┘
                         │
     Input Wire ─────────┼────────────── Output Wire
                         │
                  ┌──────┴──────┐
                  │Bottom Switch│ (Turns ON when Input is 1)
                  └──────┬──────┘
                         │
                  Ground (0 / LOW)
```

### Trace the path:
1. **When Input = 0:**
   - Top switch turns **ON**, bottom switch turns **OFF**.
   - Output connects to Power -> **Output = 1**.
2. **When Input = 1:**
   - Top switch turns **OFF**, bottom switch turns **ON**.
   - Output connects to Ground -> **Output = 0**.

Notice that at no point are both switches on at the same time. This prevents wasting electricity!

---

## 🎯 Summary Checklist

Before moving to the next file, make sure you can answer these:
- [ ] A `1` means high voltage (ON), and `0` means zero voltage (OFF).
- [ ] A transistor is an electronic switch controlled by voltage.
- [ ] **NOT** inverts: 0 -> 1, 1 -> 0.
- [ ] **AND** requires all inputs to be `1`.
- [ ] **OR** requires at least one input to be `1`.
- [ ] **XOR** is `1` only when inputs are different.
- [ ] **NAND** is the opposite of AND and can build any circuit.

---

👉 Next Step: Read **[`02_fpgas_and_luts.md`](./02_fpgas_and_luts.md)** to see how FPGAs configure these gates without rewiring silicon!
