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
![image](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/e1c2f41b-73a4-4a43-a69b-450d4dc9e2fe)


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
    
        if (sensor_input)
        {
            //digitalWrite(indicator, HIGH);
            output_indicator(1); // sound is not detected when this led is high
            if(bulb)
            {
                bulb = 0;
                output_bulb(0);//digitalWrite(bulb, LOW);
            }
            else
            {
                bulb = 1;
                output_bulb(1);//digitalWrite(bulb, HIGH);
            }
            delay(10);// waiting for almost 1 sec before sensing the input so that the present sound subsides
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
    int i;
    for (i = 0; i < seconds; i++) {
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

**Assembly code:**  

```

output:     file format elf32-littleriscv


Disassembly of section .text:

00010054 <main>:
   10054:	fe010113          	addi	sp,sp,-32
   10058:	00112e23          	sw	ra,28(sp)
   1005c:	00812c23          	sw	s0,24(sp)
   10060:	02010413          	addi	s0,sp,32
   10064:	fe042623          	sw	zero,-20(s0)
   10068:	054000ef          	jal	ra,100bc <sensor_data>
   1006c:	fea42423          	sw	a0,-24(s0)
   10070:	fe842783          	lw	a5,-24(s0)
   10074:	fe078ae3          	beqz	a5,10068 <main+0x14>
   10078:	00100513          	li	a0,1
   1007c:	068000ef          	jal	ra,100e4 <output_indicator>
   10080:	fec42783          	lw	a5,-20(s0)
   10084:	00078a63          	beqz	a5,10098 <main+0x44>
   10088:	fe042623          	sw	zero,-20(s0)
   1008c:	00000513          	li	a0,0
   10090:	098000ef          	jal	ra,10128 <output_bulb>
   10094:	0140006f          	j	100a8 <main+0x54>
   10098:	00100793          	li	a5,1
   1009c:	fef42623          	sw	a5,-20(s0)
   100a0:	00100513          	li	a0,1
   100a4:	084000ef          	jal	ra,10128 <output_bulb>
   100a8:	00a00513          	li	a0,10
   100ac:	0c0000ef          	jal	ra,1016c <delay>
   100b0:	00000513          	li	a0,0
   100b4:	030000ef          	jal	ra,100e4 <output_indicator>
   100b8:	fb1ff06f          	j	10068 <main+0x14>

000100bc <sensor_data>:
   100bc:	fe010113          	addi	sp,sp,-32
   100c0:	00812e23          	sw	s0,28(sp)
   100c4:	02010413          	addi	s0,sp,32
   100c8:	001f7793          	andi	a5,t5,1
   100cc:	fef42623          	sw	a5,-20(s0)
   100d0:	fec42783          	lw	a5,-20(s0)
   100d4:	00078513          	mv	a0,a5
   100d8:	01c12403          	lw	s0,28(sp)
   100dc:	02010113          	addi	sp,sp,32
   100e0:	00008067          	ret

000100e4 <output_indicator>:
   100e4:	fd010113          	addi	sp,sp,-48
   100e8:	02812623          	sw	s0,44(sp)
   100ec:	03010413          	addi	s0,sp,48
   100f0:	fca42e23          	sw	a0,-36(s0)
   100f4:	ffd00793          	li	a5,-3
   100f8:	fef42623          	sw	a5,-20(s0)
   100fc:	fdc42783          	lw	a5,-36(s0)
   10100:	00179793          	slli	a5,a5,0x1
   10104:	fef42423          	sw	a5,-24(s0)
   10108:	fe842783          	lw	a5,-24(s0)
   1010c:	fec42703          	lw	a4,-20(s0)
   10110:	00ef7f33          	and	t5,t5,a4
   10114:	00ff6f33          	or	t5,t5,a5
   10118:	00000013          	nop
   1011c:	02c12403          	lw	s0,44(sp)
   10120:	03010113          	addi	sp,sp,48
   10124:	00008067          	ret

00010128 <output_bulb>:
   10128:	fd010113          	addi	sp,sp,-48
   1012c:	02812623          	sw	s0,44(sp)
   10130:	03010413          	addi	s0,sp,48
   10134:	fca42e23          	sw	a0,-36(s0)
   10138:	ffb00793          	li	a5,-5
   1013c:	fef42623          	sw	a5,-20(s0)
   10140:	fdc42783          	lw	a5,-36(s0)
   10144:	00279793          	slli	a5,a5,0x2
   10148:	fef42423          	sw	a5,-24(s0)
   1014c:	fe842783          	lw	a5,-24(s0)
   10150:	fec42703          	lw	a4,-20(s0)
   10154:	00ef7f33          	and	t5,t5,a4
   10158:	00ff6f33          	or	t5,t5,a5
   1015c:	00000013          	nop
   10160:	02c12403          	lw	s0,44(sp)
   10164:	03010113          	addi	sp,sp,48
   10168:	00008067          	ret

0001016c <delay>:
   1016c:	fd010113          	addi	sp,sp,-48
   10170:	02812623          	sw	s0,44(sp)
   10174:	03010413          	addi	s0,sp,48
   10178:	fca42e23          	sw	a0,-36(s0)
   1017c:	fe042623          	sw	zero,-20(s0)
   10180:	0100006f          	j	10190 <delay+0x24>
   10184:	fec42783          	lw	a5,-20(s0)
   10188:	00178793          	addi	a5,a5,1
   1018c:	fef42623          	sw	a5,-20(s0)
   10190:	fec42703          	lw	a4,-20(s0)
   10194:	fdc42783          	lw	a5,-36(s0)
   10198:	fef746e3          	blt	a4,a5,10184 <delay+0x18>
   1019c:	00000013          	nop
   101a0:	02c12403          	lw	s0,44(sp)
   101a4:	03010113          	addi	sp,sp,48
   101a8:	00008067          	ret

```



**Note** 
In the above c program, digital read and digital write functions are commented to show how the inputs and outputs are given. For now, we need only the logic which controls the clap_switch.  

#### Unique instructions in assembly code  

To get the number of instructions, run the python script file instruction_counter.py.  

Suppose your assembly code contains instructions like addi, lw, sw, and so on. Running the instruction_counter.py on this assembly.txt would yield: 

```
Number of different instructions: 16
List of unique instructions:

or
jal
addi
beqz
li
sw
lw
mv
andi
ret
and
nop
blt
slli
j
bne
```

#### C code for Spike debug  
```
#include<stdio.h>

void output_indicator(int value);
void output_bulb(int value);
int sensor_data(int value);
void delay(int seconds);

int main()
{
    int bulb = 0;
    //debugging
   
    int indicator = 0;
    int i = 0;
    int j = 0;
    int sensor_inp= 0;

    //debugging
    while (j< 30)
    {
        if(i == 5) sensor_inp = 1;
        // sensor_input = digital_read(0);
        int sensor_input = sensor_data(sensor_inp);
        //register_value();
    
        if (sensor_input)
        {
            //digitalWrite(indicator, HIGH);
            output_indicator(1); // sound is not detected when this led is high
            
            if(bulb)
            {
                bulb = 0;
                output_bulb(0);//digitalWrite(bulb, LOW);
               
            }
            else
            {
                bulb = 1;
                output_bulb(1);//digitalWrite(bulb, HIGH);
              
            }
            delay(200);// waiting for almost 1 sec before sensing the input so that the present sound subsides
            output_indicator(0); // sound is only detected when this led goes low
            //register_value();
            //digitalWrite(indicator, LOW);
        }
        //debugging
        if(i == 5) { sensor_inp = 0; i = 0;}
        i++;
        j++;
        //debugging
    }

    return (0);
}

int sensor_data(int value)
{
    //debugging
    printf("Entering sensor_read\n");
    //debugging

    //debugging
	int mask=0xFFFFFFFE;
	
	asm volatile(
		"and x30,x30,%0\n\t"
    "or x30, x30, %1\n\t"
	    	:
	    	:"r"(mask),"r"(value)
	    	:"x30"
	    	);
	//debugging  

    int data;
    // Read sensor data into x30
    asm volatile(
            "andi %0, x30, 0x0001\n\t"
            : "=r"(data)
            :
            :
        );
    
    printf("Input_data is %d\n",data);
    return data;
}
void output_indicator(int value)
{
    printf("Entering output_indicator\n");

    unsigned int mask = 0xFFFFFFFD; // output to x30[1]
    int value1 = value*2;
    asm volatile(
        "and x30,x30, %1\n\t"
        "or x30,x30, %0\n\t"
        :
        :"r"(value1),"r"(mask)
        :"x30" //clobber list,indicating that x30 is modified
    );

    //debugging
     int data;
     int indicator;
    // Read sensor data into x30
    asm volatile(
            "andi %0, x30, 0x0002"
            : "=r"(data)
        );
    if(data == 2) indicator = 1;
    if(data == 0) indicator = 0;
    //debugging   
   
    printf("Setting Indicator value to %d\n",indicator);
    
}
void output_bulb(int value)
{
    printf("Entering output_bulb\n");

    unsigned int mask = 0xFFFFFFFB;
    int value1 = value*4;
    asm volatile(
        "and x30,x30, %1\n\t"
        "or x30,x30, %0\n\t"
        :
        :"r"(value1),"r"(mask)
        :"x30"//clobber list,indicating that x30 is modified
    );

    //debugging
     int data;
     int bulb;
    // Read sensor data into x30
    asm volatile(
            "andi %0, x30, 0x0004"
            : "=r"(data)
            :
            :
        );
    if(data == 4) bulb = 1;
    if(data == 0) bulb = 0;

    printf("Setting bulb value to %d\n",bulb);
   
}

void delay(int seconds) {
    printf("Entering delay to wait for some time before turning off the indicator\n");
    int i, j;
    for (i = 0; i < seconds; i++) {
        for (j = 0; j < 1000000; j++) {
            // Adding a loop inside to create some delay as sound may last for some time
        }
    }
}


```
Commands for debugging:
```
riscv64-unknown-elf-gcc -march=rv64i -mabi=lp64 -ffreestanding -o out single_clap_switch_copy.c 
spike pk out
```

**Output in the terminal**  
![spike_debug](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/8c9f70eb-500a-42e6-8072-8c05ef5052a4)    

Intially when sensor input is 1, indicator value and bulb values are set to 1 and after a finite delay indicator value is set to 0.
Next time when sensor input is 1, indicator value is initially set to 1, bulb value is set to 0(as previous value of bulb is 1) and after a finite delay indicator value is set to 0.  

### Functional Simulation    
After debugging the C code using Spike, the C code is converted into an optimised RTL logic which consists of your code related specific instruction set, here it is [processor.v](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/blob/main/processor.v) and functionality simulation is done using this [testbench](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/blob/main/testbench.v).  

Commands to run Verilog files and view them in gtkwave:  
```
iverilog -o test processor.v testbench.v
./test
gtkwave waveform.vcd
```
![verilog_simulation](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/a2645dd8-4b2f-481c-a36b-082e07c259fe)   

**Case 1:** Input is 1 for the 1st time i.e. sound is heard for the first time.  
Expectation - As soon as sound is heard for the first time, bulb should be turned on.  

![functional_simulation1](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/766f5fe3-3a85-4cd2-a63b-0f6e70cbf28d)  
Observations:  
Initially output value is set to 0, so as long as there is no input output continues to be 0.  
When Input becomes 1, first indicator value changes to 1 indicating the user that processor does not sense the input until the indicator goes low so that user knows when can he give his second clap to turn the bulb off, then bulb value becomes 1 as its previous state is 0. Now processor waits for certain amount of time before reading the input again and as soon as the wait is over, indicator goes low indicating the user processor is sensing the sound again.

**Case 2:** Input is 0 i.e. sound is elapsed  
Expectation - Bulb should continue to retain its previous stage as long as sound is not heard.  

![functional_simulation2](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/68257b44-0a1e-4cfc-88f0-32373d7084c5)  
Observation:  
Previously bulb value was 1 and it continued to stay in its previous state as long as input is zero i.e. sound is not heard.

**Case 3:** Input is 1 for the second time i.e. sound is heard for the second time  
Expectation - Since the bulb is on previously, it should be turned off as it hears the sound for the second time.  

![functional_simulation5](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/356cfc2f-eeed-4119-af32-593d82d78183) 
Observations:  
Soon after the input is 1, indicator becomes 1 and bulb value is 0 as it was 1 in its previous stage and the indicator becomes 0 after a delay.

Zooming into the above picture to see a dip in the input :   

![functional_simulation3](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/334d1780-705a-4f3c-bda8-7e9917141773)  
Input is brought down to 0 for a very short time and then made high intentionally in this case to verify if the processor is reading the data before the specified delay. But it can be seen that output is constant irrespective of the input before the indicator goes low.


**Case 4:** Input is 0 i.e. sound is elapsed  
Expectation - Since there is no sound heard, bulb should continue to stay in its previous state until it hears sound again.  

![functional_simulation4](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/87b7dbd2-3f5c-4148-abd9-a6c93aaf3400)   
Observation:  
In this case,input is finally 0 and bulb value continues to be high till the end.

In this way, all the required cases are tested and verified in functional simulation using Gtkwave and processor is working as expected in all the cases.  













