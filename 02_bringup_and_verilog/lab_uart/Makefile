VERILATOR ?= verilator
GTKWAVE ?= gtkwave

VERILATOR_FLAGS = -Wall --trace -cc uart_top.v uart_tx.v uart_rx.v --top-module uart_top --exe sim_main.cpp

all: build

build:
	$(VERILATOR) $(VERILATOR_FLAGS)
	make -C obj_dir -f Vuart_top.mk Vuart_top

run: build
	./obj_dir/Vuart_top

view: run
	$(GTKWAVE) uart.vcd

clean:
	rm -rf obj_dir uart.vcd

.PHONY: all build run view clean
