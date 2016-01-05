# vim: noet:ts=4:sw=4:

SDCC = sdcc
#SDLD = sdld
SDLD = sdcc
LDFLAGS = -lstm8 -mstm8 --out-fmt-ihx --code-size 0x2000 --iram-size 0x400 -V -V --verbose
CFLAGS = -lstm8 -mstm8 --opt-code-speed --std-c99
#CFLAGS = -lstm8 -mstm8 --opt-code-size --std-c99 
# vim: noet:ts=4:sw=4:

SDCC = sdcc
#SDLD = sdld
SDLD = sdcc
LDFLAGS = -lstm8 -mstm8 --out-fmt-ihx --code-size 0x2000 --iram-size 0x400 -V -V --verbose
CFLAGS = -lstm8 -mstm8 --opt-code-speed --std-c99
#CFLAGS = -lstm8 -mstm8 --opt-code-size --std-c99 
# --peep-file peepstm8
# --fverbose-asm
OBJECTS = font_cp437_5x7.rel oled1306spi.rel util.rel timing.rel main.rel 
TARGET = fuelgauge.ihx

INTERMEDIATE = $(OBJECTS)
INTERMEDIATE += $(OBJECTS:.rel=.asm)
INTERMEDIATE += $(OBJECTS:.rel=.lk)
INTERMEDIATE += $(OBJECTS:.rel=.lst)
INTERMEDIATE += $(OBJECTS:.rel=.map) 
INTERMEDIATE += $(OBJECTS:.rel=.rst)
INTERMEDIATE += $(OBJECTS:.rel=.sym)
INTERMEDIATE += $(TARGET:.ihx=.lk)
INTERMEDIATE += $(TARGET:.ihx=.map)


.PHONY: all clean flash cleanobj r

all: $(TARGET)

r: clean  all

clean:
	rm -f $(TARGET) $(INTERMEDIATE)

cleanobj:
	rm -f $(INTERMEDIATE)

flash: $(TARGET)
	stm8flash -cstlinkv2 -pstm8s003f3 -w $(TARGET)

$(TARGET): $(OBJECTS)
	$(SDLD) $(LDFLAGS) -o $@ $^
	@grep -B 2 "^INITIALIZER" $(TARGET:.ihx=.map)

%.rel: %.c
	$(SDCC) -c $(CFLAGS) $^

