#!/usr/bin/env python3
"""
asm.py — 32-Bit ARM7 (ARMv4T) Assembler in Pure Python
Translates human-readable ARM assembly into raw 32-bit machine code binaries and hex files.

Usage:
    python3 asm.py input.s -o output.bin
    python3 asm.py input.s -o output.hex --format hex
"""

import sys
import re
import struct
import argparse

# ============================================================================
# 1. ARM32 ENCODING TABLES & CONSTANTS
# ============================================================================

CONDITIONS = {
    'EQ': 0x0,  # Z == 1 (Equal)
    'NE': 0x1,  # Z == 0 (Not Equal)
    'CS': 0x2, 'HS': 0x2,  # C == 1 (Carry Set / Unsigned Higher or Same)
    'CC': 0x3, 'LO': 0x3,  # C == 0 (Carry Clear / Unsigned Lower)
    'MI': 0x4,  # N == 1 (Minus / Negative)
    'PL': 0x5,  # N == 0 (Plus / Positive or Zero)
    'VS': 0x6,  # V == 1 (Overflow)
    'VC': 0x7,  # V == 0 (No Overflow)
    'HI': 0x8,  # C == 1 and Z == 0 (Unsigned Higher)
    'LS': 0x9,  # C == 0 or Z == 1 (Unsigned Lower or Same)
    'GE': 0xA,  # N == V (Signed Greater than or Equal)
    'LT': 0xB,  # N != V (Signed Less Than)
    'GT': 0xC,  # Z == 0 and N == V (Signed Greater Than)
    'LE': 0xD,  # Z == 1 or N != V (Signed Less than or Equal)
    'AL': 0xE,  # Always (Default)
    '':   0xE   # Unconditional
}

DATA_OPCODES = {
    'AND': 0x0,
    'EOR': 0x1,
    'SUB': 0x2,
    'RSB': 0x3,
    'ADD': 0x4,
    'ADC': 0x5,
    'SBC': 0x6,
    'RSC': 0x7,
    'TST': 0x8,
    'TEQ': 0x9,
    'CMP': 0xA,
    'CMN': 0xB,
    'ORR': 0xC,
    'MOV': 0xD,
    'BIC': 0xE,
    'MVN': 0xF
}

REGISTERS = {
    f'R{i}': i for i in range(16)
}
REGISTERS.update({
    'SP': 13,
    'LR': 14,
    'PC': 15
})


# ============================================================================
# 2. HELPER FUNCTIONS FOR BIT MANIPULATION
# ============================================================================

def parse_reg(reg_str):
    """Parses register string like 'R0', 'SP', 'PC' to integer index (0..15)."""
    reg_clean = reg_str.strip().upper()
    if reg_clean not in REGISTERS:
        raise ValueError(f"Invalid register name: '{reg_str}'")
    return REGISTERS[reg_clean]


def parse_immediate(imm_str):
    """Parses immediate numbers like '#42', '#0x2A', or '-8'."""
    clean = imm_str.strip().lstrip('#')
    if clean.startswith(('0x', '0X')):
        return int(clean, 16)
    elif clean.startswith(('0b', '0B')):
        return int(clean, 2)
    else:
        return int(clean, 10)


def encode_arm_immediate(val):
    """
    Encodes an immediate value into ARM's 8-bit constant + 4-bit rotate field.
    Returns (encoded_12_bits) or raises ValueError if not representable.
    """
    val = val & 0xFFFFFFFF
    for r in range(0, 16):
        rot = r * 2
        # Reverse rotate
        unrotated = ((val << rot) & 0xFFFFFFFF) | (val >> (32 - rot) if rot > 0 else 0)
        if (unrotated & 0xFFFFFF00) == 0:
            return (r << 8) | (unrotated & 0xFF)
    raise ValueError(f"Immediate value {val} (0x{val:X}) cannot be encoded in 8-bit rotated format.")


# ============================================================================
# 3. TWO-PASS ASSEMBLER ENGINE
# ============================================================================

class ARMAssembler:
    def __init__(self):
        self.labels = {}
        self.instructions = []

    def pass_one(self, lines):
        """Pass 1: Strip comments, collect labels, calculate instruction addresses."""
        pc = 0
        cleaned_lines = []

        for line_num, raw_line in enumerate(lines, 1):
            trimmed = raw_line.strip()
            # If line starts with #, @, //, or ;, it is a full comment line
            if trimmed.startswith(('#', '@', '//', ';')):
                continue

            # Strip inline comments (starting with @, //, or ;)
            line = re.sub(r'(@|//|;).*$', '', raw_line).strip()
            if not line:
                continue

            # Check for label definitions (e.g. 'loop_start:')
            while ':' in line:
                label_part, line = line.split(':', 1)
                label_name = label_part.strip()
                if label_name:
                    if label_name in self.labels:
                        raise ValueError(f"Duplicate label '{label_name}' on line {line_num}")
                    self.labels[label_name] = pc
                line = line.strip()

            if line:
                cleaned_lines.append((line_num, pc, line))
                # Directives like .word or standard 32-bit instructions take 4 bytes
                pc += 4

        return cleaned_lines

    def parse_mnemonic(self, mnemonic_raw):
        """Extracts base opcode, condition suffix, and 'S' flag (e.g. 'ADDEQS' -> ('ADD', 'EQ', True))."""
        m = mnemonic_raw.upper()

        # Check for Branch with Link ('BL') vs Branch ('B')
        if m.startswith('BL') and len(m) <= 4:
            cond = m[2:]
            return 'BL', cond, False
        elif m.startswith('B') and len(m) <= 3 and m not in ('BIC', 'BICS'):
            cond = m[1:]
            return 'B', cond, False

        # Check Data Processing and Memory instructions
        for base in sorted(list(DATA_OPCODES.keys()) + ['LDR', 'STR', 'LDRB', 'STRB'], key=len, reverse=True):
            if m.startswith(base):
                rest = m[len(base):]
                s_flag = False
                if rest.endswith('S') and base in DATA_OPCODES and base not in ('CMP', 'CMN', 'TST', 'TEQ'):
                    s_flag = True
                    rest = rest[:-1]
                cond = rest
                if cond in CONDITIONS:
                    return base, cond, s_flag

        raise ValueError(f"Unknown mnemonic: '{mnemonic_raw}'")

    def assemble_instruction(self, line_num, pc, line):
        """Assembles a single instruction string into a 32-bit integer."""
        parts = re.split(r'[\s,]+', line.strip())
        mnemonic_raw = parts[0]
        args = parts[1:]

        # Handle .word directive
        if mnemonic_raw.lower() == '.word':
            val_str = args[0]
            if val_str in self.labels:
                return self.labels[val_str]
            return parse_immediate(val_str)

        base_op, cond_str, s_flag = self.parse_mnemonic(mnemonic_raw)
        cond = CONDITIONS.get(cond_str, 0xE)

        # --------------------------------------------------------------------
        # 1. BRANCH INSTRUCTIONS (B, BL)
        # --------------------------------------------------------------------
        if base_op in ('B', 'BL'):
            target_str = args[0]
            if target_str in self.labels:
                target_addr = self.labels[target_str]
                # ARM pipeline offset: PC is 8 bytes ahead of current instruction!
                offset = (target_addr - (pc + 8)) >> 2
            else:
                offset = parse_immediate(target_str) >> 2

            offset_24 = offset & 0x00FFFFFF
            link_bit = 1 if base_op == 'BL' else 0
            word = (cond << 28) | (0b101 << 25) | (link_bit << 24) | offset_24
            return word

        # --------------------------------------------------------------------
        # 2. DATA PROCESSING INSTRUCTIONS (ADD, SUB, MOV, CMP, etc.)
        # --------------------------------------------------------------------
        if base_op in DATA_OPCODES:
            opcode = DATA_OPCODES[base_op]
            s_bit = 1 if (s_flag or base_op in ('CMP', 'CMN', 'TST', 'TEQ')) else 0

            if base_op in ('MOV', 'MVN'):
                rd = parse_reg(args[0])
                rn = 0
                op2_str = args[1]
            elif base_op in ('CMP', 'CMN', 'TST', 'TEQ'):
                rd = 0
                rn = parse_reg(args[0])
                op2_str = args[1]
            else: # ADD, SUB, AND, ORR, etc.
                rd = parse_reg(args[0])
                rn = parse_reg(args[1])
                op2_str = args[2] if len(args) > 2 else args[1]

            # Operand 2 parsing
            if op2_str.startswith('#'):
                imm_val = parse_immediate(op2_str)
                op2_bits = encode_arm_immediate(imm_val)
                i_bit = 1
            else:
                rm = parse_reg(op2_str)
                op2_bits = rm & 0xF
                i_bit = 0

            word = (cond << 28) | (0b00 << 26) | (i_bit << 25) | (opcode << 21) | \
                   (s_bit << 20) | (rn << 16) | (rd << 12) | op2_bits
            return word

        # --------------------------------------------------------------------
        # 3. SINGLE DATA TRANSFER (LDR, STR, LDRB, STRB)
        # --------------------------------------------------------------------
        if base_op in ('LDR', 'STR', 'LDRB', 'STRB'):
            rd = parse_reg(args[0])
            # Parse memory reference like [Rn] or [Rn, #offset]
            mem_str = " ".join(args[1:])
            m = re.match(r'\[\s*([A-Za-z0-9]+)(?:\s*,\s*#?([-\w]+))?\s*\]', mem_str)
            if not m:
                raise ValueError(f"Invalid memory addressing format on line {line_num}: '{mem_str}'")

            rn = parse_reg(m.group(1))
            offset_val = parse_immediate(m.group(2)) if m.group(2) else 0

            u_bit = 1 if offset_val >= 0 else 0
            offset_12 = abs(offset_val) & 0xFFF
            l_bit = 1 if base_op.startswith('LDR') else 0
            b_bit = 1 if 'B' in base_op else 0
            p_bit = 1  # Pre-indexed
            w_bit = 0  # No writeback

            word = (cond << 28) | (0b01 << 26) | (0 << 25) | (p_bit << 24) | \
                   (u_bit << 23) | (b_bit << 22) | (w_bit << 21) | (l_bit << 20) | \
                   (rn << 16) | (rd << 12) | offset_12
            return word

        raise ValueError(f"Unsupported instruction on line {line_num}: '{line}'")

    def assemble(self, source_text):
        """Assembles source code text and returns list of 32-bit integer words."""
        lines = source_text.splitlines()
        cleaned_lines = self.pass_one(lines)
        machine_words = []

        for line_num, pc, line in cleaned_lines:
            try:
                word = self.assemble_instruction(line_num, pc, line)
                machine_words.append(word)
            except Exception as e:
                print(f"❌ Error on line {line_num}: {line}\n   --> {e}", file=sys.stderr)
                sys.exit(1)

        return machine_words


# ============================================================================
# 4. COMMAND LINE INTERFACE
# ============================================================================

def main():
    parser = argparse.ArgumentParser(description="32-Bit ARM7 (ARMv4T) Python Assembler")
    parser.add_argument("input", help="Source assembly file (.s / .asm)")
    parser.add_argument("-o", "--output", default="a.bin", help="Output file path (default: a.bin)")
    parser.add_argument("-f", "--format", choices=["bin", "hex"], default="bin",
                        help="Output format: 'bin' (raw binary) or 'hex' (Verilog $readmemh hex text)")

    args = parser.parse_args()

    with open(args.input, "r") as f:
        source_code = f.read()

    assembler = ARMAssembler()
    words = assembler.assemble(source_code)

    print(f"🔧 Assembled {len(words)} instructions ({len(words)*4} bytes) from '{args.input}'.")

    if args.format == "bin":
        with open(args.output, "wb") as f:
            for w in words:
                f.write(struct.pack("<I", w)) # Little-endian 32-bit word
        print(f"💾 Saved binary output to '{args.output}'.")
    else:
        with open(args.output, "w") as f:
            for w in words:
                f.write(f"{w:08X}\n")
        print(f"📄 Saved Verilog hex output to '{args.output}'.")


if __name__ == "__main__":
    main()
