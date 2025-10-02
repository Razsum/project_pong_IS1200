# DTEKV I2C

A small library to comunicate with i2c devices over GPIO pins.
This is intended for use with the DTEKV board of the Computer Hardware Engineering (IS1200) course at KTH.

## Usage Guide

#### Clone repository in to lib folder

```sh
mkdir lib
cd lib
git clone https://github.com/Olivoz/dtekv-i2c-lib.git
```

#### Update Makefile

```diff
diff --git a/Makefile b/Makefile
--- a/Makefile
+++ b/Makefile
@@ -1,11 +1,11 @@
 SRC_DIR ?= ./
 OBJ_DIR ?= ./
-SOURCES ?= $(shell find $(SRC_DIR) -name '*.c' -or -name '*.S')
+SOURCES ?= $(shell find $(SRC_DIR) -not -path './lib/*' -name '*.c' -or -name '*.S') $(shell find ./lib/**/src -name '*.c' -or -name '*.S')
 OBJECTS ?= $(addsuffix .o, $(basename $(notdir $(SOURCES))))
 LINKER ?= $(SRC_DIR)/dtekv-script.lds

 TOOLCHAIN ?= riscv32-unknown-elf-
-CFLAGS ?= -Wall -nostdlib -O3 -mabi=ilp32 -march=rv32imzicsr -fno-builtin
+CFLAGS ?= -Wall -nostdlib -O3 -mabi=ilp32 -march=rv32imzicsr -fno-builtin -I./lib/dtekv-i2c-lib/include


 build: clean main.bin
```

#### Configure GPIO pins

By default this library uses GPIO pin 0 and 1.
You can change this in the `include/dtekv-i2c-lib/dtekv-i2c-lib.h` header.
There are also some other modifiable values in `include/dtekv-mpu6050-lib/dtekv-mpu6050-lib.h`, such as the MPU6050 I2C Address.
