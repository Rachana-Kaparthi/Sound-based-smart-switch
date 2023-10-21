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
  I am trying to build a smart switch using sound sensor module. Initially we set the sensor threshold by rotating the screw in the sensor module so that any sound heard above the threshold is detected as high by the sensor and the same is sent to the processor. Processor checks the previous state of the switch and decides whether to turn the switch on or off based on its previous value. It turns the switch on if the previous value is low or turns the switch off if the previous value is high.I have set a time delay of 2 secs for two consecutive sensor read operations if sensor reads high for the first time so that the processor is not detecting the same clap twice before the sound is elapsed. I have used an indicator led so that user know when sound is being detected. The output of the switch can be connected to any home appliance such as light, fan etc using relays or motors at the output.
### Block Diagram  
![block-diagram-single-clap-switch](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/d6584d81-4c49-42c6-830b-5ebf6ba6ca84)

### Flowchart of the code
![single_clap_switch](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/c1cf3748-82fa-47d9-bb62-0dc62e0c97f2)

### C code  
**Register architecture of x30 for GPIOs:**  

x30[0] - input data from sensor  

x30[1] - output to indicator  

x30[2] - output to switch 

Code snippet of Clap switch is shown below:  
```
void output_indicator(int value);
void output_bulb(int value);
int sensor_data();
void delay(int seconds);

int main()
{
    int bulb = 0;
    while (1)
    {
        // sensor_input = digital_read(0);
        int sensor_input = sensor_data();
    
        if (sensor_input == 1)
        {
            //digitalWrite(indicator, HIGH);
            output_indicator(1); // sound is not detected when this led is high
            if(bulb == 1)
            {
                bulb = 0;
                output_bulb(0);//digitalWrite(bulb, LOW);
            }
            else
            {
                bulb = 1;
                output_bulb(1);//digitalWrite(bulb, HIGH);
            }
            delay(500);// waiting for almost 1 sec before sensing the input so that the present sound subsides
            output_indicator(0); // sound is only detected when this led goes low
            //digitalWrite(indicator, LOW);
        }
    }

    return (0);
}

int sensor_data()
{
    int data;
    // Read sensor data into x30
    asm volatile(
            "and %0, x30, 1"
            : "=r"(data)
            :
            :
        );
    return data;
}
void output_indicator(int value)
{
    unsigned int mask = 0xFFFFFFFD; // output to x30[1]
    int value1 = value*2;
    asm volatile(
        "and x30,x30, %1\n\t"
        "or x30,x30, %0\n\t"
        :
        :"r"(value1),"r"(mask)
        :"x30" //clobber list,indicating that x30 is modified
    );
}
void output_bulb(int value)
{
    unsigned int mask = 0xFFFFFFFB;
    int value1 = value*4;
    asm volatile(
        "and x30,x30, %1\n\t"
        "or x30,x30, %0\n\t"
        :
        :"r"(value1),"r"(mask)
        :"x30" //clobber list,indicating that x30 is modified
    );
}

void delay(int seconds) {
    int i, j;
    for (i = 0; i < seconds; i++) {
        for (j = 0; j < 1000000; j++) {
            // Adding a loop inside to create some delay as sound may last for some time
        }
    }
}

```

Test file for the above C code can be seen [here](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/blob/main/clap_switch_test.c).   

C code can be compiled by running the below commands in terminal -  
```
gcc clap_switch.c
./a.out
```

Below is the output seen upon execution of the test file using gcc compiler-  
![](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/blob/main/images/single_clap_switch_test_output.png)  


### Assembly code Conversion  

Compile the c program using RISCV-V GNU Toolchain and dump the assembly code into obj_dump.txt using the below commands.  

```
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -ffreestanding -nostdlib -o out clap_switch.c
riscv64-unknown-elf-objdump -d -r out > assembly.txt
```
The written assembly.txt file can be seen [here](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/blob/main/assembly.txt).  


**Note** 
In the above c program, digital read and digital write functions are commented to show how the inputs and outputs are given. For now, we need only the logic which controls the clap_switch.  

#### Unique instructions in assembly code  

To get the number of instructions, run the python script file instruction_counter.py.  

Suppose your assembly code contains instructions like addi, lw, sw, and so on. Running the instruction_counter.py on this assembly.txt would yield: 

```
Number of different instructions: 17
List of unique instructions:
j
bne
jal
nop
and
bge
li
sw
mv
addi
slli
lw
ret
or
andi
blt
lui
```



