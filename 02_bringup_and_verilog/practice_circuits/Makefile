VERILATOR ?= verilator

test_inverter:
	$(VERILATOR) -Wall -CFLAGS "-DTEST_INVERTER" -cc inverter.v --exe test_practice.cpp
	make -C obj_dir -f Vinverter.mk Vinverter
	./obj_dir/Vinverter

test_mux:
	$(VERILATOR) -Wall -CFLAGS "-DTEST_MUX2" -cc mux2.v --exe test_practice.cpp
	make -C obj_dir -f Vmux2.mk Vmux2
	./obj_dir/Vmux2

test_dff:
	$(VERILATOR) -Wall -CFLAGS "-DTEST_DFF" -cc dff.v --exe test_practice.cpp
	make -C obj_dir -f Vdff.mk Vdff
	./obj_dir/Vdff

test_adder:
	$(VERILATOR) -Wall -CFLAGS "-DTEST_ADDER" -cc half_adder.v --exe test_practice.cpp
	make -C obj_dir -f Vhalf_adder.mk Vhalf_adder
	./obj_dir/Vhalf_adder

test_all: test_inverter test_mux test_dff test_adder

clean:
	rm -rf obj_dir

.PHONY: test_inverter test_mux test_dff test_adder test_all clean
