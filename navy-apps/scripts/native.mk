LD = $(CXX)

### Run an application with $(ISA)=native

NAVY_HOME = $(NEMU_HOME)/../navy-apps

env:
	$(MAKE) -C $(NAVY_HOME)/libs/libos ISA=native

run: app env
	@LD_PRELOAD=$(NAVY_HOME)/libs/libos/build/native.so $(APP) $(mainargs)

gdb: app env
	@LD_PRELOAD=$(NAVY_HOME)/libs/libos/build/native.so gdb --args $(APP) $(mainargs)

.PHONY: env run gdb
