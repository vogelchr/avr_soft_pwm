DEVICE_CC = atmega328p
DEVICE_DUDE = m328p

PROGRAMMER_DUDE = -Pusb -c dragon_isp

AVRDUDE=avrdude
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
CC=avr-gcc
LD=avr-gcc

LDFLAGS=-Wall -g -mmcu=$(DEVICE_CC)
CPPFLAGS=
CFLAGS=-mmcu=$(DEVICE_CC) -Os -Wall -g -DF_CPU=16000000UL

MYNAME=avr-soft-pwm

OBJS=$(MYNAME).o serial.o avr-soft-pwm-irq.o

all : $(MYNAME).hex $(MYNAME).lst

$(MYNAME).bin : $(OBJS)

%.hex : %.bin
	$(OBJCOPY) -j .text -j .data -O ihex $^ $@ || (rm -f $@ ; false )

%.lst : %.bin
	$(OBJDUMP) -S $^ >$@ || (rm -f $@ ; false )

%.bin : %.o
	$(LD) $(LDFLAGS) -o $@ $^

ifneq ($(MAKECMDGOALS),clean)
include $(OBJS:.o=.d)
endif

%.d : %.c
	$(CC) -o $@ -MM $^

.PHONY : clean burn
burn : $(MYNAME).hex
	$(AVRDUDE) $(PROGRAMMER_DUDE) -p $(DEVICE_DUDE) -U flash:w:$^
clean :
	rm -f *.bak *~ *.bin *.hex *.lst *.o *.d
