# Makefile for 32-Bit ARM7 CPU Core Verilator Simulation

VERILATOR = verilator
VERILATOR_FLAGS = -Wall -Wno-UNUSED -Wno-UNDRIVEN --trace --cc
SRCS = cpu_top.v cpu_core.v alu.v decoder.v register_file.v memory_bus.v

.PHONY: all build test_add test_fib clean

all: build

build:
	$(VERILATOR) $(VERILATOR_FLAGS) $(SRCS) --exe sim_main.cpp --build -o Vcpu_top

test_add: build
	python3 ../assembler/asm.py ../assembler/examples/add.s -o ../assembler/examples/add.hex -f hex
	./obj_dir/Vcpu_top ../assembler/examples/add.hex

test_fib: build
	python3 ../assembler/asm.py ../assembler/examples/fibonacci.s -o ../assembler/examples/fibonacci.hex -f hex
	./obj_dir/Vcpu_top ../assembler/examples/fibonacci.hex

clean:
	rm -rf obj_dir cpu_trace.vcd
