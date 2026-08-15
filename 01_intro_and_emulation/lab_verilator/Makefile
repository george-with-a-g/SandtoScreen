# Verilator Makefile for 4-bit Counter

TOP_MODULE = counter
VERILOG_SRC = counter.v
CPP_TB = sim_main.cpp
VCD_FILE = counter.vcd

OBJ_DIR = obj_dir
SIM_BIN = $(OBJ_DIR)/V$(TOP_MODULE)

all: $(SIM_BIN)

$(SIM_BIN): $(VERILOG_SRC) $(CPP_TB)
	@echo "[VERILATOR] Compiling Verilog to C++ Model..."
	verilator --cc $(VERILOG_SRC) --exe $(CPP_TB) --trace --build -o V$(TOP_MODULE) -Mdir $(OBJ_DIR)

run: $(SIM_BIN)
	@echo "[SIM] Running Simulation..."
	./$(SIM_BIN)

wave: $(VCD_FILE)
	@echo "[GTKWAVE] Opening waveform trace..."
	gtkwave $(VCD_FILE) &

clean:
	rm -rf $(OBJ_DIR) $(VCD_FILE)

.PHONY: all run wave clean
