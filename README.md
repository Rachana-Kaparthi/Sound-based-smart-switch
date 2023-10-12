# Sound-based-smart-switch

## Table of Contents
- [RISCV GNU tool chain](#riscv-gnu-tool-chain)
- [Introduction to project](#introduction)
  - [Application Overview](#application-overview)
  - [Block diagram](#block-diagram)
  - [Flowchart](#flowchart)
  - [C code](#c-code)
  - [Assembly code conversion](#assembly-code-conversion)
    - [Unique instructions in assembly code](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/edit/main/README.md#unique-instructions-in-assembly-code)

## RISCV GNU tool chain

RISCV GNU tool chain is a C & C++ cross compiler. It has two modes: ELF/Newlib toolchain and Linux-ELF/glibc toolchain. We are using ELF/Newlib toolchain.

We are building a custom RISCV based application core for a specific application for 32 bit processor. 

Following are tools required to compile & execute the application:

1. RISCV GNU toolchain with dependent libraries as specified in [RISCV-GNU-Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain).

2. Spike simulator - Spike is a functional RISC-V ISA simulator that implements a functional model of one or more RISC-V harts. [RISCV-SPIKE](https://github.com/riscv-software-src/riscv-isa-sim.git).

### RISCV 32 bit compiler installation.

```
git clone https://github.com/riscv/riscv-gnu-toolchain --recursive
mkdir riscv32-toolchain
cd riscv-gnu-toolchain

// Adding this to path to the PATH variable
gedit ~/.bashrc
export PATH="/home/rachana/riscv32-toolchain/bin:$PATH" 
source ~/.bashrc

./configure --prefix=/home/rachana/riscv32-toolchain/ --with-arch=rv32i --with-abi=ilp32
sudo apt-get install libgmp-dev
make
```

Access the riscv32-unknown-elf-gcc inside bin folder of riscv32-toolchain folder in home folder of user as shown.
```
/home/rachana/riscv32-toolchain/bin/riscv32-unknown-elf-gcc --version
```

## Introduction 
### Application Overview
  I am trying to build a smart switch using sound sensor module. Initially we set the sensor threshold to a value so that any sound heard above the threshold is detected as high by the sensor and the same is sent to the processor. Processor checks the previous state of the switch and decides whether to turn the switch on or off based on its previous value. It turns the switch on if the previous value is low or turns the switch off if the previous value is high.I have set a time delay of 2 secs for two consecutive sensor read operations if sensor reads high for the first time so that the processor is not detecting the same clap twice before the sound is elapsed. I have used an indicator led so that user know when sound is being detected. The output of the switch can be connected to any home appliance such as light, fan etc using relays or motors at the output.
### Block Diagram  
![block-diagram-clap-switch](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/260a92d6-86a1-408d-a752-829760196ada)  

### Flowchart of the code
![clap_switch](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/f071487a-66b3-4e08-a2cd-db3a35716b89)  

### C code  
**Register architecture of x30 for GPIOs:**  

x30[0] - input data from sensor  

x30[1] - output to indicator  

x30[2] - output to switch 

Code snippet of Clap switch is shown below:  
```

#include <time.h>

#define DIFF_MAX 500000

int firstClap = 0;
int secondClap = 0;
int offClap1 = 0;
clock_t firstClaptime;
clock_t secondClaptime;
clock_t offClap1time;
void output(int value);
int sensor_data();

int sensor_data()
{
    int data;
    // Read sensor data into x30
    asm (
            "and %0, x30, 1"
            : "=r"(data)
        );
    return data;
}
void output(int value)
{
    unsigned int mask = 0xFFFFFFFD;
    int value1 = value*2;
   asm volatile(
        "and x30,x30, %1\n\t"
        "or x30,x30, %0\n\t"
        :
        :"r"(value1),"r"(mask)
        :"x30" //clobber list,indicating that x30 is modified
    );
}
int delay(int number_of_milli_seconds)
{
    // Converting time into milli_seconds

    int clocks = 1000 * number_of_milli_seconds;

    // Storing start time

    clock_t start_time = clock();

    // looping till required time is not achieved

    while (clock() < start_time + clocks)
        ;
}

void read()
{
    // sensor_input = digital_read(0);
    int sensor_input = sensor_data();
   // int output;
    if (sensor_input == 1)
    {
        if (firstClap == 0)
        {
            firstClap = 1;
            firstClaptime = clock();
        }
        else if (firstClap == 1 && secondClap == 0)
        {
            if (clock() - firstClaptime < DIFF_MAX)
            {
                secondClap = 1;
                output(1);
                // digitalWrite(3, output);
            }
            else
            {
                firstClap = 0;
            }
        }
        else if (firstClap == 1 && secondClap == 1)
        {
            if (offClap1 == 0)
            {
                offClap1 = 1;
                offClap1time = clock();
            }
            else if (offClap1 == 1)
            {
                if (clock() - offClap1time < DIFF_MAX)
                {
                    firstClap = 0;
                    secondClap = 0;
                    output(0); // digitalwrite(3,output);
                }
                offClap1 = 0;
            }
        }
        delay(200);
    }

    if (firstClap == 1 && secondClap == 0 && (clock() - firstClaptime > DIFF_MAX))
    {
        firstClap = 0;
    }
    if (offClap1 == 1 && (clock() - offClap1time > DIFF_MAX))
    {
        offClap1 = 0;
    }
}

int main()
{
    while (1)
    {
        read();
    }

    return (0);
}

```
C code can be compiled by running the below commands in terminal -  
```
gcc clap_switch.c
./a.out
```

Test file for the above C code can be seen [here](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/blob/main/testfiles/clap_switch_test_file.c).   
Below is the output seen upon execution of the test file using gcc compiler-  
![](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/blob/main/images/clap_switch_test_output.png)  


### Assembly code Conversion  

Compile the c program using RISCV-V GNU Toolchain and dump the assembly code into obj_dump.txt using the below commands.  

```
riscv32-unknown-elf-gcc -c -mabi=ilp32 -march=rv32im -ffreestanding -nostdlib -o ./clap_switch clap_switch.c
riscv32-unknown-elf-objdump -d -r clap_switch|less > assembly.txt
```
The written assembly.txt file can be seen [here](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/blob/main/assembly.txt).  


**Note** 
In the above c program, digital read and digital write functions are commented to show how the inputs and outputs are given. For now, we need only the logic which controls the clap_switch.  

#### Unique instructions in assembly code  

To get the number of instructions, run the python script file instruction_counter.py.  

Suppose your assembly code contains instructions like addi, lw, sw, and so on. Running the instruction_counter.py on this assembly.txt would yield: 

```
umber of different instructions: 20
List of unique instructions:
add
lw
ret
sub
and
bnez
li
jalr
mul
bgeu
mv
nop
bne
sw
j
lui
or
bltu
auipc
sll
```



