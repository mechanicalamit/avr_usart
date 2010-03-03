# Minimal Makefile for avr-gcc

PROJECT=avr_usart_base
SOURCES=avr_usart_base.c
PROGGER=ba1fb
MMCU=attiny2313

CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
OPT=s
 
CFLAGS=-mmcu=$(MMCU) -Wall -O$(OPT)
 
$(PROJECT).hex: $(PROJECT).out
	$(OBJCOPY) -j .text -O ihex $(PROJECT).out $(PROJECT).hex
	$(SIZE) $(PROJECT).out
 
$(PROJECT).out: $(SOURCES)
	$(CC) $(CFLAGS) -I./ -o $(PROJECT).out $(SOURCES)

disasm:$(PROJECT).out
	avr-objdump -S $(PROJECT).out

asm:$(SOURCES)
	$(CC) $(CFLAGS) -I./ -S $(SOURCES)
 
program: $(PROJECT).hex
	avrdude -p $(MMCU) -c ba1fb -U flash:w:$(PROJECT).hex
clean:
	rm -f $(PROJECT).out
	rm -f $(PROJECT).hex

