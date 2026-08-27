VERILATOR ?= verilator
GTKWAVE ?= gtkwave

VERILATOR_FLAGS = -Wall --trace -cc blinky.v --exe sim_main.cpp

all: build

build:
	$(VERILATOR) $(VERILATOR_FLAGS)
	make -C obj_dir -f Vblinky.mk Vblinky

run: build
	./obj_dir/Vblinky

view: run
	$(GTKWAVE) blinky.vcd

clean:
	rm -rf obj_dir blinky.vcd

.PHONY: all build run view clean
