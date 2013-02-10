RM := rm -rf

.PHONY: help all clean atmega328p atmega644 atmega32 atmega168 atmega1280 atmega128 atmega1284p atmega128rfa1 atmega2561 atmega2560 atmega640 atmega8

# Help
help:
	@echo 'make all - Will rebuild the library for all supported processors - takes a while'
	@echo 'make clean - Will delete the compiled libraries and all intermediate files from the build folder - may take a while'
	@echo 'make atmega328p - Will rebuild libWebbot-ATMega328P.a only'
	@echo 'make atmega644 - Will rebuild libWebbot-ATMega644.a only'
	@echo 'make atmega32 - Will rebuild libWebbot-ATMega32.a only'
	@echo 'make atmega168 - Will rebuild libWebbot-ATMega168.a only'
	@echo 'make atmega1280 - Will rebuild libWebbot-ATMega1280.a only'
	@echo 'make atmega128 - Will rebuild libWebbot-ATMega128.a only'
	@echo 'make atmega1284p - Will rebuild libWebbot-ATMega1284P.a only'
	@echo 'make atmega128rfa1 - Will rebuild libWebbot-ATMega128RFA1.a only'
	@echo 'make atmega2561 - Will rebuild libWebbot-ATMega2561.a only'
	@echo 'make atmega2560 - Will rebuild libWebbot-ATMega2560.a only'
	@echo 'make atmega640 - Will rebuild libWebbot-ATMega640.a only'
	@echo 'make atmega8 - Will rebuild libWebbot-ATMega8.a only'

# Rebuild for all processors
all: atmega328p atmega644 atmega32 atmega168 atmega1280 atmega128 atmega1284p atmega128rfa1 atmega2561 atmega2560 atmega640 atmega8

# Clean everything up
clean:
	-$(RM) build *.a *.lss

# Create library for atmega328p
atmega328p:
	@echo 'Making $@'
	${MAKE} MCU=atmega328p OUT=./build/$@ --makefile=./makeone libWebbot-ATMega328P.a

# Create library for atmega644
atmega644:
	@echo 'Making $@'
	${MAKE} MCU=atmega644 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega644.a

# Create library for atmega32
atmega32:
	@echo 'Making $@'
	${MAKE} MCU=atmega32 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega32.a

# Create library for atmega168
atmega168:
	@echo 'Making $@'
	${MAKE} MCU=atmega168 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega168.a

# Create library for atmega1280
atmega1280:
	@echo 'Making $@'
	${MAKE} MCU=atmega1280 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega1280.a

# Create library for atmega128
atmega128:
	@echo 'Making $@'
	${MAKE} MCU=atmega128 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega128.a

# Create library for atmega1284p
atmega1284p:
	@echo 'Making $@'
	${MAKE} MCU=atmega1284p OUT=./build/$@ --makefile=./makeone libWebbot-ATMega1284P.a

# Create library for atmega128rfa1
atmega128rfa1:
	@echo 'Making $@'
	${MAKE} MCU=atmega128rfa1 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega128RFA1.a

# Create library for atmega2561
atmega2561:
	@echo 'Making $@'
	${MAKE} MCU=atmega2561 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega2561.a

# Create library for atmega2560
atmega2560:
	@echo 'Making $@'
	${MAKE} MCU=atmega2560 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega2560.a

# Create library for atmega640
atmega640:
	@echo 'Making $@'
	${MAKE} MCU=atmega640 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega640.a

# Create library for atmega8
atmega8:
	@echo 'Making $@'
	${MAKE} MCU=atmega8 OUT=./build/$@ --makefile=./makeone libWebbot-ATMega8.a

