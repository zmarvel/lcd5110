

OPENOCD ?= /usr/bin/openocd/
OPENOCD_SCRIPTS ?= /usr/share/openocd/scripts

openocd: $(ELF)
	$(OPENOCD) -s $(OPENOCD_SCRIPTS) \
	    -f interface/stlink-v2-1.cfg \
	    -f target/stm32l4x.cfg \
	    -c "telnet_port 4444" \
	    -c "gdb_port 3333"

download: $(ELF)
	$(OPENOCD) -s $(OPENOCD_SCRIPTS) \
	    -f interface/stlink-v2-1.cfg \
	    -f target/stm32l4x.cfg \
	    -c "init" \
	    -c "targets" \
	    -c "reset halt" \
	    -c "program $< verify" \
	    -c "reset run" \
	    -c "exit"
