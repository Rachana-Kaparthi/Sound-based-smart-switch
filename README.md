# Sound-based-smart-switch

## Table of Contents
- [RISCV GNU tool chain](#riscv-gnu-tool-chain)
- [Introduction to project](#introduction)
  - [Application Overview](#application-overview)
  - [Block diagram](#block-diagram)
  - [Flowchart](#flowchart)
  - [C code](#c-code)
  - [Assembly code conversion](#assembly-code-conversion)
    - [Unique instructions in assembly code](#unique-instructions-in-assembly-code)
  - [C code for debugging using Spike](#c-code-for-debugging-using-spike)
  - [Functional Simulations](#functional-simulation)
  - [GLC Simulations](#gls-simulation)
    - [Netlist-1: For testing UART functionality](#netlist-1-for-testing-uart-functionality)
    - [Netlist-2: For performing GLS Simulations bypassing UART](#netlist-2-for-performing-gls-simulations-bypassing-uart)
    - [Detailed view of ID_instruction](#detailed-view-of-id_instruction)
  - [Physical Design Using Openlane](#physical-design-using-openlane)
    - [Design steps using Openlane](#design-steps-using-openlane)
    	- [Invoking Openlane](#invoking-openlane)
        - [Synthesis](#synthesis)
        - [Floorplan](#floorplan)
        - [Placement](#placement)
        - [CTS](#CTS)
        - [Routing](#Routing)

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

### Sound Sensor module  
![image](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/7cfc76bf-e497-454a-bddf-49163323c745)
DAOKI High Sensitivity Sound Microphone Sensor Detection Module  

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

int main()
{
    int bulb = 0;
    while (1)
    {
        
        unsigned int mask=0xFFFFFFFE;
        int data;
        // Read sensor data into x30
        asm volatile(
                "andi %0, x30, 0x0001\n\t"
                : "=r"(data)
                :
                :
            );
        
       // printf("Input_data is %d\n",data);
        int sensor_input = data;
       
        if (sensor_input == 1)
        {

            mask = 0xFFFFFFFD; // output to x30[1]
            int value1 = 2;
            asm volatile(
                "and x30,x30, %1\n\t"
                "or x30,x30, %0\n\t"
                :
                :"r"(value1),"r"(mask)
                :"x30" //clobber list,indicating that x30 is modified
            );

            
            if(bulb == 1)
            {
                bulb = 0;

                mask = 0xFFFFFFFB;
                int value2 = 0;
                asm volatile(
                    "and x30,x30, %1\n\t"
                    "or x30,x30, %0\n\t"
                    :
                    :"r"(value2),"r"(mask)
                    :"x30"//clobber list,indicating that x30 is modified
                );
            }
            else
            {
                bulb = 1;
               

                mask = 0xFFFFFFFB;
                int value2 = 4;
                asm volatile(
                    "and x30,x30, %1\n\t"
                    "or x30,x30, %0\n\t"
                    :
                    :"r"(value2),"r"(mask)
                    :"x30"//clobber list,indicating that x30 is modified
                );


            }
            // delay(500);// waiting for almost 1 sec before sensing the input so that the present sound subsides
            int i,j;
            for (i = 0; i < 10; i++) {
                 for (j = 0; j < 10; j++) {
                // Adding a loop inside to create some delay as sound may last for some time
                }

            }
           

            mask = 0xFFFFFFFD; // output to x30[1]
            value1 = 0;
            asm volatile(
                "and x30,x30, %1\n\t"
                "or x30,x30, %0\n\t"
                :
                :"r"(value1),"r"(mask)
                :"x30" //clobber list,indicating that x30 is modified
            );

        }
       
    }

    return (0);
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

**Assembly code:**  

```
out32:     file format elf32-littleriscv


Disassembly of section .text:

00010054 <main>:
   10054:	fc010113          	addi	sp,sp,-64
   10058:	02812e23          	sw	s0,60(sp)
   1005c:	04010413          	addi	s0,sp,64
   10060:	fe042623          	sw	zero,-20(s0)
   10064:	ffe00793          	li	a5,-2
   10068:	fef42023          	sw	a5,-32(s0)
   1006c:	001f7793          	andi	a5,t5,1
   10070:	fcf42e23          	sw	a5,-36(s0)
   10074:	fdc42783          	lw	a5,-36(s0)
   10078:	fcf42c23          	sw	a5,-40(s0)
   1007c:	fd842703          	lw	a4,-40(s0)
   10080:	00100793          	li	a5,1
   10084:	fef710e3          	bne	a4,a5,10064 <main+0x10>
   10088:	ffd00793          	li	a5,-3
   1008c:	fef42023          	sw	a5,-32(s0)
   10090:	00200793          	li	a5,2
   10094:	fcf42a23          	sw	a5,-44(s0)
   10098:	fd442783          	lw	a5,-44(s0)
   1009c:	fe042703          	lw	a4,-32(s0)
   100a0:	00ef7f33          	and	t5,t5,a4
   100a4:	00ff6f33          	or	t5,t5,a5
   100a8:	fec42703          	lw	a4,-20(s0)
   100ac:	00100793          	li	a5,1
   100b0:	02f71463          	bne	a4,a5,100d8 <main+0x84>
   100b4:	fe042623          	sw	zero,-20(s0)
   100b8:	ffb00793          	li	a5,-5
   100bc:	fef42023          	sw	a5,-32(s0)
   100c0:	fc042823          	sw	zero,-48(s0)
   100c4:	fd042783          	lw	a5,-48(s0)
   100c8:	fe042703          	lw	a4,-32(s0)
   100cc:	00ef7f33          	and	t5,t5,a4
   100d0:	00ff6f33          	or	t5,t5,a5
   100d4:	02c0006f          	j	10100 <main+0xac>
   100d8:	00100793          	li	a5,1
   100dc:	fef42623          	sw	a5,-20(s0)
   100e0:	ffb00793          	li	a5,-5
   100e4:	fef42023          	sw	a5,-32(s0)
   100e8:	00400793          	li	a5,4
   100ec:	fcf42623          	sw	a5,-52(s0)
   100f0:	fcc42783          	lw	a5,-52(s0)
   100f4:	fe042703          	lw	a4,-32(s0)
   100f8:	00ef7f33          	and	t5,t5,a4
   100fc:	00ff6f33          	or	t5,t5,a5
   10100:	fe042423          	sw	zero,-24(s0)
   10104:	0300006f          	j	10134 <main+0xe0>
   10108:	fe042223          	sw	zero,-28(s0)
   1010c:	0100006f          	j	1011c <main+0xc8>
   10110:	fe442783          	lw	a5,-28(s0)
   10114:	00178793          	addi	a5,a5,1
   10118:	fef42223          	sw	a5,-28(s0)
   1011c:	fe442703          	lw	a4,-28(s0)
   10120:	00900793          	li	a5,9
   10124:	fee7d6e3          	bge	a5,a4,10110 <main+0xbc>
   10128:	fe842783          	lw	a5,-24(s0)
   1012c:	00178793          	addi	a5,a5,1
   10130:	fef42423          	sw	a5,-24(s0)
   10134:	fe842703          	lw	a4,-24(s0)
   10138:	00900793          	li	a5,9
   1013c:	fce7d6e3          	bge	a5,a4,10108 <main+0xb4>
   10140:	ffd00793          	li	a5,-3
   10144:	fef42023          	sw	a5,-32(s0)
   10148:	fc042a23          	sw	zero,-44(s0)
   1014c:	fd442783          	lw	a5,-44(s0)
   10150:	fe042703          	lw	a4,-32(s0)
   10154:	00ef7f33          	and	t5,t5,a4
   10158:	00ff6f33          	or	t5,t5,a5
   1015c:	f09ff06f          	j	10064 <main+0x10>

```

**Note** 
In the above c program, digital read and digital write functions are commented to show how the inputs and outputs are given. For now, we need only the logic which controls the clap_switch.  

#### Unique instructions in assembly code  

To get the number of instructions, run the python script file instruction_counter.py.  

Suppose your assembly code contains instructions like addi, lw, sw, and so on. Running the instruction_counter.py on this assembly.txt would yield: 

```
Number of different instructions: 10
List of unique instructions:
li
or
bne
bge
and
j
addi
sw
lw
andi
```

### C code for debugging using Spike  

```
#include<stdio.h>

void delay(int seconds);

int main()
{
    int bulb = 0;
    //debugging
   
    int indicator = 0;
    int i = 0;
    int j = 0;
    int sensor_inp= 0;
    int register_value = 0;

    //debugging
    while (j< 30)
    {
        if(i == 5) sensor_inp = 1;
        // sensor_input = digital_read(0);
        //-------------------------------------------------------
        //debugging
        printf("Entering sensor_read\n");
        //debugging

        //debugging
        unsigned int mask=0xFFFFFFFE;
        
        asm volatile(
            "and x30,x30,%0\n\t"
            "or x30, x30, %1\n\t"
                :
                :"r"(mask),"r"(sensor_inp)
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
        int sensor_input = data;
        //--------------------------------------------------------
    
        if (sensor_input == 1)
        {
            //digitalWrite(indicator, HIGH);
            //output_indicator(1); // sound is not detected when this led is high
            //-----------------------------------------------------------------

            printf("Entering output_indicator\n");

            mask = 0xFFFFFFFD; // output to x30[1]
            int value1 = 2;
            asm volatile(
                "and x30,x30, %1\n\t"
                "or x30,x30, %0\n\t"
                :
                :"r"(value1),"r"(mask)
                :"x30" //clobber list,indicating that x30 is modified
            );

            //debugging
            // Read sensor data into x30
            asm volatile(
                    "andi %0, x30, 0x0002"
                    : "=r"(data)
                );
            if(data == 2) indicator = 1;
            if(data == 0) indicator = 0;
        
            printf("Setting Indicator value to %d\n",indicator);

            //-----------------------------------------------------------------
            
            if(bulb == 1)
            {
                bulb = 0;
                // output_bulb(0);//digitalWrite(bulb, LOW);
                //-----------------------------------------------------------

                printf("Entering output_bulb\n");

                mask = 0xFFFFFFFB;
                int value2 = 0;
                asm volatile(
                    "and x30,x30, %1\n\t"
                    "or x30,x30, %0\n\t"
                    :
                    :"r"(value2),"r"(mask)
                    :"x30"//clobber list,indicating that x30 is modified
                );

                //debugging
                int data;
                // int bulb;
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

                //------------------------------------------------------------
            }
            else
            {
                bulb = 1;
                // output_bulb(1);//digitalWrite(bulb, HIGH);
                //-----------------------------------------------------------

                printf("Entering output_bulb\n");

                mask = 0xFFFFFFFB;
                int value2 = 4;
                asm volatile(
                    "and x30,x30, %1\n\t"
                    "or x30,x30, %0\n\t"
                    :
                    :"r"(value2),"r"(mask)
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

                //------------------------------------------------------------
            }
            
            int i,j;
            for (i = 0; i < 10; i++) {
                 for (j = 0; j < 10; j++) {
                // Adding a loop inside to create some delay as sound may last for some time
                }

            }
            // output_indicator(0); // sound is only detected when this led goes low
            //-----------------------------------------------------------------

            printf("Entering output_indicator\n");

            mask = 0xFFFFFFFD; // output to x30[1]
            value1 = 0;
            asm volatile(
                "and x30,x30, %1\n\t"
                "or x30,x30, %0\n\t"
                :
                :"r"(value1),"r"(mask)
                :"x30" //clobber list,indicating that x30 is modified
            );

            //debugging
            // Read sensor data into x30
            asm volatile(
                    "andi %0, x30, 0x0002"
                    : "=r"(data)
                );
            if(data == 2) indicator = 1;
            if(data == 0) indicator = 0;
        
            printf("Setting Indicator value to %d\n",indicator);

            //-----------------------------------------------------------------
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

### GLS Simulation  

In this section, we will be performing Gate level Simulation by creating two different netlist - one is specific to our ASIC application using UART mechanism for data transfer and second netlist is created for testing by bypassing the UART mechanism.   

#### Netlist-1: For testing UART functionality:  
Make sure you have the below files for generating netlist1 -  
- ``` sky130_fd_sc_hd__tt_025C_1v80_256 ```
- ``` processor.v ```
- ``` sky130_fd_sc_hd.v ```
- ``` primitives.v ```
  
Before performing Yosys, make sure you comment out below mentioned sram module definations from your ```processor.v``` file which was used earlier for functional testing:
- module sky130_sram_1kbyte_1rw1r_32x256_8_inst
- module sky130_sram_1kbyte_1rw1r_32x256_8_data
Also, make sure your writing_inst_done = 0 in the code as shown below
![writing_inst_done_0](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/50a29d9a-bb7c-46c5-ac5c-55ff59e47903)
Now run the below Yosys commands :
```
yosys // to invoke yosys in your folder
read_liberty -lib sky130_fd_sc_hd__tt_025C_1v80_256.lib
read_verilog processor.v
synth -top wrapper
dfflibmap -liberty sky130_fd_sc_hd__tt_025C_1v80_256.lib
abc -liberty sky130_fd_sc_hd__tt_025C_1v80_256.lib
write_verilog synth_test_asic.v
```
A verilog file gets created in you folder with the name ```synth_test_asic.v```. Now make sure you have the definition of sky130_sram_1kbyte_1rw1r_32x256_8 in a file in your folder, for this create a file named  ```sky130_sram_1kbyte_1rw1r_32x256_8_uart.v```  with the below contents:  

```
module sky130_sram_1kbyte_1rw1r_32x256_8(
`ifdef USE_POWER_PINS
    vccd1,
    vssd1,
`endif
// Port 0: RW
    clk0,csb0,web0,wmask0,addr0,din0,dout0,
// Port 1: R
    clk1,csb1,addr1,dout1
  );

  parameter NUM_WMASKS = 4 ;
  parameter DATA_WIDTH = 32 ;
  parameter ADDR_WIDTH = 8 ;
  parameter RAM_DEPTH = 1 << ADDR_WIDTH;
  // FIXME: This delay is arbitrary.
  parameter DELAY = 0 ;
  parameter VERBOSE = 0 ; //Set to 0 to only display warnings
  parameter T_HOLD = 0 ; //Delay to hold dout value after posedge. Value is arbitrary

`ifdef USE_POWER_PINS
    inout vccd1;
    inout vssd1;
`endif
  input  clk0; // clock
  input   csb0; // active low chip select
  input  web0; // active low write control
  input [NUM_WMASKS-1:0]   wmask0; // write mask
  input [ADDR_WIDTH-1:0]  addr0;
  input [DATA_WIDTH-1:0]  din0;
  output [DATA_WIDTH-1:0] dout0;
  input  clk1; // clock
  input   csb1; // active low chip select
  input [ADDR_WIDTH-1:0]  addr1;
  output [DATA_WIDTH-1:0] dout1;

  reg  csb0_reg;
  reg  web0_reg;
  reg [NUM_WMASKS-1:0]   wmask0_reg;
  reg [ADDR_WIDTH-1:0]  addr0_reg;
  reg [DATA_WIDTH-1:0]  din0_reg;
  reg [DATA_WIDTH-1:0]  dout0;

  // All inputs are registers
  always @(posedge clk0)
  begin
    csb0_reg = csb0;
    web0_reg = web0;
    wmask0_reg = wmask0;
    addr0_reg = addr0;
    din0_reg = din0;
    //#(T_HOLD) dout0 = 32'bx;
    if ( !csb0_reg && web0_reg && VERBOSE ) 
      $display($time," Reading %m addr0=%b dout0=%b",addr0_reg,mem[addr0_reg]);
    if ( !csb0_reg && !web0_reg && VERBOSE )
      $display($time," Writing %m addr0=%b din0=%b wmask0=%b",addr0_reg,din0_reg,wmask0_reg);
  end

  reg  csb1_reg;
  reg [ADDR_WIDTH-1:0]  addr1_reg;
  reg [DATA_WIDTH-1:0]  dout1;

  // All inputs are registers
  always @(posedge clk1)
  begin
    csb1_reg = csb1;
    addr1_reg = addr1;
    if (!csb0 && !web0 && !csb1 && (addr0 == addr1))
         $display($time," WARNING: Writing and reading addr0=%b and addr1=%b simultaneously!",addr0,addr1);
    #(T_HOLD) dout1 = 32'bx;
    if ( !csb1_reg && VERBOSE ) 
      $display($time," Reading %m addr1=%b dout1=%b",addr1_reg,mem[addr1_reg]);
  end

reg [DATA_WIDTH-1:0]    mem [0:RAM_DEPTH-1];

  // Memory Write Block Port 0
  // Write Operation : When web0 = 0, csb0 = 0
  always @ (negedge clk0)
  begin : MEM_WRITE0
    if ( !csb0_reg && !web0_reg ) begin
        if (wmask0_reg[0])
                mem[addr0_reg][7:0] = din0_reg[7:0];
        if (wmask0_reg[1])
                mem[addr0_reg][15:8] = din0_reg[15:8];
        if (wmask0_reg[2])
                mem[addr0_reg][23:16] = din0_reg[23:16];
        if (wmask0_reg[3])
                mem[addr0_reg][31:24] = din0_reg[31:24];
    end
  end

  // Memory Read Block Port 0
  // Read Operation : When web0 = 1, csb0 = 0
  always @ (negedge clk0)
  begin : MEM_READ0
    if (!csb0_reg && web0_reg)
       dout0 <= #(DELAY) mem[addr0_reg];
  end

  // Memory Read Block Port 1
  // Read Operation : When web1 = 1, csb1 = 0
  always @ (negedge clk1)
  begin : MEM_READ1
    if (!csb1_reg)
       dout1 <= #(DELAY) mem[addr1_reg];
  end

endmodule
```
Now run the below iverilog commands in the terminal:  
```
iverilog -o test_uart testbench.v synth_test_asic.v sky130_sram_1kbyte_1rw1r_32x256_8_uart.v sky130_fd_sc_hd.v primitives.v
./test_uart
gtkwave waveform_uart.vcd
```
Below is the output in the gtkwave:  
![gls_gtkwave_uart](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/5f7c1820-7213-47e3-bcf5-c001f66bf3a9)
In this case, writing_inst_done will be 1 after the whole instructions are sent via uart and we can see writing_inst_done was initialliy 0 and becomes 1 after certain time.  
ID_instruction starts fetching the instructions only after writing_inst_done becomes 1 and output in this case is shown below:  
![gls_gtkwave_uart1](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/6effac0c-1eab-4464-940e-6d712506f561)  
So, this confirms that UART data transfer is working as expected.  

#### Netlist-2: For performing GLS Simulations bypassing UART  
Though the above method confirms proper output, UART transfers take a lot of time for generating the VCD file. So, here is a quicker way to perform simulation bypassing UART:  
Make sure you have the below files for generating netlist2 -  
- ``` sky130_fd_sc_hd__tt_025C_1v80_256 ```
- ``` processor.v ```
- ``` sky130_fd_sc_hd.v ```
- ``` primitives.v ```
Before performing Yosys, make sure writing_inst_done is set to 1 in ```processor.v``` file.
![writing_inst_done_1](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/e0ab9674-c614-44f9-add8-680247c7e5c0)
Now run the below Yosys commands :
```
yosys // to invoke yosys in your folder
read_liberty -lib sky130_fd_sc_hd__tt_025C_1v80_256.lib
read_verilog processor.v
synth -top wrapper
dfflibmap -liberty sky130_fd_sc_hd__tt_025C_1v80_256.lib
abc -liberty sky130_fd_sc_hd__tt_025C_1v80_256.lib
write_verilog synth_test_processor.v
```
A verilog file gets created in you folder with the name ```synth_test_processor.v```. Now change the name of the module sky130_sram_1kbyte_1rw1r_32x256_8 to sky130_sram_1kbyte_1rw1r_32x256_8_data and sky130_sram_1kbyte_1rw1r_32x256_8 to sky130_sram_1kbyte_1rw1r_32x256_8_inst in the generated synth_test_processor.v file.  
Secondly change the contents of testbench file similar to this file [testbench_bypassuart.v](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/blob/main/testbench_bypassuart.v)
Next make sure you have the definition of sky130_sram_1kbyte_1rw1r_32x256_8_data and sky130_sram_1kbyte_1rw1r_32x256_8_inst modules in a file in your folder, for this create a file named  ```sky130_sram_1kbyte_1rw1r_32x256_8.v```  with the below contents:   

```
// OpenRAM SRAM model
// Words: 512
// Word size: 32
// Write size: 8

module sky130_sram_1kbyte_1rw1r_32x256_8_inst(
`ifdef USE_POWER_PINS
    vccd1,
    vssd1,
`endif
// Port 0: RW
    clk0,csb0,web0,wmask0,addr0,din0,dout0,
// Port 1: R
    clk1,csb1,addr1,dout1
  );

  parameter NUM_WMASKS = 4 ;
  parameter DATA_WIDTH = 32 ;
  parameter ADDR_WIDTH = 8 ;
  parameter RAM_DEPTH = 1 << ADDR_WIDTH;
  // FIXME: This delay is arbitrary.
  parameter DELAY = 0 ;
  parameter VERBOSE = 0 ; //Set to 0 to only display warnings
  parameter T_HOLD = 0 ; //Delay to hold dout value after posedge. Value is arbitrary

`ifdef USE_POWER_PINS
    inout vccd1;
    inout vssd1;
`endif
  input  clk0; // clock
  input   csb0; // active low chip select
  input  web0; // active low write control
  input [NUM_WMASKS-1:0]   wmask0; // write mask
  input [ADDR_WIDTH-1:0]  addr0;
  input [DATA_WIDTH-1:0]  din0;
  output [DATA_WIDTH-1:0] dout0;
  input  clk1; // clock
  input   csb1; // active low chip select
  input [ADDR_WIDTH-1:0]  addr1;
  output [DATA_WIDTH-1:0] dout1;

  reg  csb0_reg;
  reg  web0_reg;
  reg [NUM_WMASKS-1:0]   wmask0_reg;
  reg [ADDR_WIDTH-1:0]  addr0_reg;
  reg [DATA_WIDTH-1:0]  din0_reg;
  reg [DATA_WIDTH-1:0]  dout0;

  // All inputs are registers
  always @(posedge clk0)
  begin
    csb0_reg = csb0;
    web0_reg = web0;
    wmask0_reg = wmask0;
    addr0_reg = addr0;
    din0_reg = din0;
    #(T_HOLD) dout0 = 32'bx;
    if ( !csb0_reg && web0_reg && VERBOSE ) 
      $display($time," Reading %m addr0=%b dout0=%b",addr0_reg,mem[addr0_reg]);
    if ( !csb0_reg && !web0_reg && VERBOSE )
      $display($time," Writing %m addr0=%b din0=%b wmask0=%b",addr0_reg,din0_reg,wmask0_reg);
  end

  reg  csb1_reg;
  reg [ADDR_WIDTH-1:0]  addr1_reg;
  reg [DATA_WIDTH-1:0]  dout1;

  // All inputs are registers
  always @(posedge clk1)
  begin
    csb1_reg = csb1;
    addr1_reg = addr1;
    if (!csb0 && !web0 && !csb1 && (addr0 == addr1))
         $display($time," WARNING: Writing and reading addr0=%b and addr1=%b simultaneously!",addr0,addr1);
    #(T_HOLD) dout1 = 32'bx;
    if ( !csb1_reg && VERBOSE ) 
      $display($time," Reading %m addr1=%b dout1=%b",addr1_reg,mem[addr1_reg]);
  end

reg [DATA_WIDTH-1:0]    mem [0:RAM_DEPTH-1];

  initial
    begin

        mem[0] = 32'h00000000;
	mem[1] = 32'h00000000;
	mem[2] = 32'hfd010113;
	mem[3] = 32'h02812623;
	mem[4] = 32'h03010413;
	mem[5] = 32'hfe042623;
	mem[6] = 32'hffe00793;
	mem[7] = 32'hfef42223;
	mem[8] = 32'h001f7793;
	mem[9] = 32'hfef42023;
	mem[10] = 32'hfe042783;
	mem[11] = 32'hfcf42e23;
	mem[12] = 32'hfdc42703;
	mem[13] = 32'h00100793;
	mem[14] = 32'hfef710e3;
	mem[15] = 32'hffd00793;
	mem[16] = 32'hfef42223;
	mem[17] = 32'h00200793;
	mem[18] = 32'hfcf42c23;
	mem[19] = 32'hfd842783;
	mem[20] = 32'hfe442703;
	mem[21] = 32'h00ef7f33;
	mem[22] = 32'h00ff6f33;
	mem[23] = 32'hfec42703;
	mem[24] = 32'h00100793;
	mem[25] = 32'h02f71463;
	mem[26] = 32'hfe042623;
	mem[27] = 32'hffb00793;
	mem[28] = 32'hfef42223;
	mem[29] = 32'hfc042a23;
	mem[30] = 32'hfd442783;
	mem[31] = 32'hfe442703;
	mem[32] = 32'h00ef7f33;
	mem[33] = 32'h00ff6f33;
	mem[34] = 32'h02c0006f;
	mem[35] = 32'h00100793;
	mem[36] = 32'hfef42623;
	mem[37] = 32'hffb00793;
	mem[38] = 32'hfef42223;
	mem[39] = 32'h00400793;
	mem[40] = 32'hfcf42823;
	mem[41] = 32'hfd042783;
	mem[42] = 32'hfe442703;
	mem[43] = 32'h00ef7f33;
	mem[44] = 32'h00ff6f33;
	mem[45] = 32'hfe042423;
	mem[46] = 32'h0100006f;
	mem[47] = 32'hfe842783;
	mem[48] = 32'h00178793;
	mem[49] = 32'hfef42423;
	mem[50] = 32'hfe842703;
	mem[51] = 32'h00900793;
	mem[52] = 32'hfee7d6e3;
	mem[53] = 32'hffd00793;
	mem[54] = 32'hfef42223;
	mem[55] = 32'hfc042c23;
	mem[56] = 32'hfd842783;
	mem[57] = 32'hfe442703;
	mem[58] = 32'h00ef7f33;
	mem[59] = 32'h00ff6f33;
	mem[60] = 32'hf29ff06f;
	mem[61] = 32'hffffffff;
	mem[62] = 32'hffffffff;

  end

  // Memory Write Block Port 0
  // Write Operation : When web0 = 0, csb0 = 0
  always @ (negedge clk0)
  begin : MEM_WRITE0
    if ( !csb0_reg && !web0_reg ) begin
        if (wmask0_reg[0])
                mem[addr0_reg][7:0] = din0_reg[7:0];
        if (wmask0_reg[1])
                mem[addr0_reg][15:8] = din0_reg[15:8];
        if (wmask0_reg[2])
                mem[addr0_reg][23:16] = din0_reg[23:16];
        if (wmask0_reg[3])
                mem[addr0_reg][31:24] = din0_reg[31:24];
    end
  end

  // Memory Read Block Port 0
  // Read Operation : When web0 = 1, csb0 = 0
  always @ (negedge clk0)
  begin : MEM_READ0
    if (!csb0_reg && web0_reg)
       dout0 <= #(DELAY) mem[addr0_reg];
  end

  // Memory Read Block Port 1
  // Read Operation : When web1 = 1, csb1 = 0
  always @ (negedge clk1)
  begin : MEM_READ1
    if (!csb1_reg)
       dout1 <= #(DELAY) mem[addr1_reg];
  end

endmodule


// OpenRAM SRAM model
// Words: 512
// Word size: 32
// Write size: 8

module sky130_sram_1kbyte_1rw1r_32x256_8_data(
`ifdef USE_POWER_PINS
    vccd1,
    vssd1,
`endif
// Port 0: RW
    clk0,csb0,web0,wmask0,addr0,din0,dout0,
// Port 1: R
    clk1,csb1,addr1,dout1
  );

  parameter NUM_WMASKS = 4 ;
  parameter DATA_WIDTH = 32 ;
  parameter ADDR_WIDTH = 8 ;
  parameter RAM_DEPTH = 1 << ADDR_WIDTH;
  // FIXME: This delay is arbitrary.
  parameter DELAY = 0 ;
  parameter VERBOSE = 0 ; //Set to 0 to only display warnings
  parameter T_HOLD = 0 ; //Delay to hold dout value after posedge. Value is arbitrary

`ifdef USE_POWER_PINS
    inout vccd1;
    inout vssd1;
`endif
  input  clk0; // clock
  input   csb0; // active low chip select
  input  web0; // active low write control
  input [NUM_WMASKS-1:0]   wmask0; // write mask
  input [ADDR_WIDTH-1:0]  addr0;
  input [DATA_WIDTH-1:0]  din0;
  output [DATA_WIDTH-1:0] dout0;
  input  clk1; // clock
  input   csb1; // active low chip select
  input [ADDR_WIDTH-1:0]  addr1;
  output [DATA_WIDTH-1:0] dout1;

  reg  csb0_reg;
  reg  web0_reg;
  reg [NUM_WMASKS-1:0]   wmask0_reg;
  reg [ADDR_WIDTH-1:0]  addr0_reg;
  reg [DATA_WIDTH-1:0]  din0_reg;
  reg [DATA_WIDTH-1:0]  dout0;

  // All inputs are registers
  always @(posedge clk0)
  begin
    csb0_reg = csb0;
    web0_reg = web0;
    wmask0_reg = wmask0;
    addr0_reg = addr0;
    din0_reg = din0;
    #(T_HOLD) dout0 = 32'bx;
    if ( !csb0_reg && web0_reg && VERBOSE ) 
      $display($time," Reading %m addr0=%b dout0=%b",addr0_reg,mem[addr0_reg]);
    if ( !csb0_reg && !web0_reg && VERBOSE )
      $display($time," Writing %m addr0=%b din0=%b wmask0=%b",addr0_reg,din0_reg,wmask0_reg);
  end

  reg  csb1_reg;
  reg [ADDR_WIDTH-1:0]  addr1_reg;
  reg [DATA_WIDTH-1:0]  dout1;

  // All inputs are registers
  always @(posedge clk1)
  begin
    csb1_reg = csb1;
    addr1_reg = addr1;
    if (!csb0 && !web0 && !csb1 && (addr0 == addr1))
         $display($time," WARNING: Writing and reading addr0=%b and addr1=%b simultaneously!",addr0,addr1);
    #(T_HOLD) dout1 = 32'bx;
    if ( !csb1_reg && VERBOSE ) 
      $display($time," Reading %m addr1=%b dout1=%b",addr1_reg,mem[addr1_reg]);
  end

reg [DATA_WIDTH-1:0]    mem [0:RAM_DEPTH-1];

  // Memory Write Block Port 0
  // Write Operation : When web0 = 0, csb0 = 0
  always @ (negedge clk0)
  begin : MEM_WRITE0
    if ( !csb0_reg && !web0_reg ) begin
        if (wmask0_reg[0])
                mem[addr0_reg][7:0] = din0_reg[7:0];
        if (wmask0_reg[1])
                mem[addr0_reg][15:8] = din0_reg[15:8];
        if (wmask0_reg[2])
                mem[addr0_reg][23:16] = din0_reg[23:16];
        if (wmask0_reg[3])
                mem[addr0_reg][31:24] = din0_reg[31:24];
    end
  end

  // Memory Read Block Port 0
  // Read Operation : When web0 = 1, csb0 = 0
  always @ (negedge clk0)
  begin : MEM_READ0
    if (!csb0_reg && web0_reg)
       dout0 <= #(DELAY) mem[addr0_reg];
  end

  // Memory Read Block Port 1
  // Read Operation : When web1 = 1, csb1 = 0
  always @ (negedge clk1)
  begin : MEM_READ1
    if (!csb1_reg)
       dout1 <= #(DELAY) mem[addr1_reg];
  end

endmodule
```
Now, run the below iverilog commands in the terminal:

```
iverilog -o test testbench_bypassuart.v synth_test_processor.v sky130_sram_1kbyte_1rw1r_32x256_8.v sky130_fd_sc_hd.v primitives.v
./test
gtkwave waveform.vcd
```

Here is output in the gtkwave :

![gls_gtkwave](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/86cdb230-73e0-4c9f-ad90-9a54dcfc90d4)

It can be clearly seen that, in this case writing_inst_done is set to 1 from the very beginning of the simulation and output is seen as expected similar to that of the functional simulation.  

#### Detailed view of ID_instruction

As already seen in the [assembly code](#assembly-code-conversion) conversion section,  

```10064:	ffe00793          	li	a5,-2  ```  

sensor_input is read by the processor when the ID_instruction is ffe00793 and the same can be observed in the gtkwave output waveform at multiple instances as shown below 

![ID_inst1](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/caf529a2-6205-452d-bb94-7da29daa4c82 "Figure 1")   
![ID_inst2](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/bdefa8a9-9187-4237-98f2-d0777a1d4241 "Figure 2")   

In figure 2, it can be seen that as long as sensor_input is 0[case A, case B], processor compares it with 1, breaks on not equal to 1 and returns to sensor read operation which is   

``` 10084:	fef710e3          	bne	a4,a5,10064 <main+0x10> ```   

According to assembly code, x30 register is modified with the indicator output when processor excecutes the below instruction and the same is shown in gtkwave 
``` 100a4:	00ff6f33          	or	t5,t5,a5 ```  

![ID_inst3](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/31ae60e3-0c91-48c0-8cf4-5b2aca81e51c "Figure 3")    

Next, x30 is modified with bulb output when processor executes ``` 100d0:	00ff6f33          	or	t5,t5,a5 ``` and same is observed in the gtkwave    

![ID_inst4](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/568e2dbf-8b8b-4704-bd73-eed131739c66 "Figure 4")   

Finally,indicator is turned off after certain delay when processor executes ``` 10154:	00ef7f33          	and	t5,t5,a4 ```,   
ideally output should change after this instruction ```10158:	00ff6f33          	or	t5,t5,a5```   
but indicator value that is being sent is 0 here, so bitwise or of any number is the number itself. Hence output is observed a little early as shown below   

![ID_inst5](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/ba7e3251-a365-42ec-af70-0a4857761843 "Figure 5")

In this way, inputs and outputs are observed in gtkwave and verified the order of instructions taking place in accordance with the assembly code generated using GCC compiler.    

## Physical Design Using Openlane
OpenLane is an automated RTL to GDSII flow based on several components including OpenROAD, Yosys, Magic, Netgen, CVC, SPEF-Extractor, KLayout and a number of custom scripts for design exploration and optimization. It also provides a number of custom scripts for design exploration and optimization.  

Detailed steps on installation of required tools can be seen [here](https://github.com/Rachana-Kaparthi/ADVANCED-PHYSICAL-DESIGN-USING-OPENLANE-SKY130/edit/main/README.md#day-1---inception-of-open-source-edaopenlane-and-sky130-pdk)

The Place and Route (PNR) flow is a crucial step in the physical design process of creating integrated circuits. It involves placing the synthesized logic elements onto the chip and routing the interconnections between these elements. The Place and Route flow is a key stage in turning a high-level hardware description into a physical design that can be fabricated.    

Here's an overview of the typical PNR flow:

*Synthesis:*

Convert your RTL code into a gate-level netlist using synthesis tools such as Yosys. This step generates a logical representation of your design using standard cells from a library.  

*Floor Planning:*

Define the physical layout of the chip, including the placement of functional blocks, I/O pads, and power grid distribution. This step helps determine the chip's overall size and shape.  

*Power Planning:*

Implement the power distribution network to provide stable power to all parts of the design while minimizing voltage drop. Tools like OpenSTA can be used for static timing analysis to ensure proper power distribution.  

*Placement:*

Place the synthesized logical cells onto the chip's floorplan. Tools like RePLace or Graywolf can be used for placement.  
Placement is usually done in two steps:  

- Global Placement
- Detailed Placement

*Clock Tree Synthesis (CTS):*

Generate a clock distribution network that ensures clock signals reach all parts of the design with minimal skew. Typically, OpenLane's TritonCTS is used for this purpose.It ususally takes the shape of a tree.  

*Routing:*

Create the physical interconnections (metal layers) between the placed cells while adhering to design rules. This step is performed using a router like FastRoute or TritonRoute.  
Metal Layer form a routing grid which is huge, hence we use divide and conquer methodology for routing grid.  
Global Routing: Generates routing grids  
Detailed Routing: Uses the routing guides to implement the actual wiring  

*Sign Off:*  

This includes physical and Timing verifications.  
- *Physical verifications:*
  - *Design Rule Checking (DRC):*
    - Verify that the chip layout adheres to the manufacturing process's design rules. DRC tools like Magic or KLayout are commonly used for this purpose.
  - *Layout vs. Schematic (LVS) Check:*
    - Ensure that the final layout matches the original schematic. LVS tools like Netgen or Calibre are used to compare the netlist extracted from the layout with the synthesized netlist.
- *Timing verifications:*
  - *Static Timing Analysis (STA):*
    - Analyze the timing characteristics of your design to ensure that all setup and hold time requirements are met. OpenSTA is commonly used in the OpenLane flow for STA.
### Design steps using Openlane  

Before starting of with the design make sure you have all the required libraries, lef files needed for the design. Required files for this design can be seen [here](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/tree/main/clap_switch)  
After cloning the openlane related files from github, go to designs folder and create a folder with the name clap_switch inside which place all the required extra lef, lib files and verilog files under src folder inside clap_switch. Now, place congif.json file inside clap+switch folder and outside src.  Structure should look like below:  
```
clap_switch
|__ config.json
|__ macro_placement.cfg
|__ src
|   |__ processor.v
|   |__ sky130_fd_sc_hd.v
|   |__ sky130_sram_1kbyte_1rw1r_32x256_8.gds
|   |__ sky130_sram_1kbyte_1rw1r_32x256_8.lef  
|   |__ sky130_sram_1kbyte_1rw1r_32x256_8.v
|   |__ sky130_sram_1kbyte_1rw1r_32x256_8_TT_1p8V_25C.lib
|   |__ other lib files and lef files 
```
#### Invoking Openlane  
Use the below commands to invoke Openlane in the linux terminal  
```
cd ~/OpenLane
make mount
./flow.tcl -interactive
package require openlane 0.9
prep -design clap_switch
```  
![openlane_launch](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/8cdf9f43-5316-4534-af17-c1b5751f282c)

#### Synthesis  

The main aim of synthesis in the Place and Route (PNR) flow is to transform a high-level hardware description, typically represented in RTL (Register Transfer Level), into a gate-level netlist that can be physically implemented on an integrated circuit (IC).  

Here are the primary goals and tasks of synthesis in the PNR flow:

Logic Translation:

Synthesis translates the RTL description of the digital design into a gate-level netlist. The RTL code describes the functional behavior of the design using high-level constructs, while the gate-level netlist represents the design in terms of logic gates and flip-flops.  

Technology Mapping:  

During synthesis, the tool selects specific gates and flip-flops from the target technology library to implement the logical functions described in the RTL. This process involves mapping the abstract RTL constructs to the available library cells.

```
run_synthesis
```
![run_synthesis](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/9c87bb99-c249-4b45-ab27-faf26504518d)    

**Statistics after Synthesis:**  
![synthesis_chip_are](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/1d6009d9-606d-494e-8ad7-99aaa3572ac1)


#### Floorplan  

The main aim of the floorplanning stage in the Place and Route (PNR) flow is to define the physical organization of the integrated circuit (IC) on the chip, allocating space for different functional blocks and guiding the subsequent placement and routing stages.  
Here are the main goals and objectives of floorplanning in the PNR flow:

Area Allocation:    

Floorplanning involves dividing the chip area into different regions to allocate space for various functional blocks, such as the core logic, I/O pads, clock distribution network, and memory. This step helps ensure efficient use of available silicon real estate.  

Aspect Ratio Determination:  

The floorplan defines the aspect ratio of the chip, specifying the width and height of the chip. The aspect ratio is important for meeting the overall design goals and constraints.  

I/O Placement:  

The locations of input and output pads (I/O pads) are determined during floorplanning. These pads need to be strategically placed for ease of connectivity with external devices and to minimize signal delays.  

Hierarchy Consideration:  

For large and complex designs, floorplanning takes into account the hierarchical structure of the design. It may involve partitioning the design into smaller blocks, each with its own floorplan, and then integrating these blocks into the overall chip floorplan.  

```
run_floorplan
```
![run_floorplan](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/cd1e6607-ade2-4a1d-a847-a158b54ec7ea)
- Post the floorplan run, a .def file will have been created within the results/floorplan directory.
- We may review floorplan files by checking the floorplan.tcl.
To view the floorplan, Magic is invoked after moving to the results/floorplan directory,then use the following command:
```
magic -T /home/rachana/open_pdks/sky130/magic/sky130.tech lef read ../../tmp/merged.max.lef def read wrapper.def &
```
![magic_floorplan](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/c42b640d-ef80-4e69-a7ec-e8eff274fc11)

**Die Area and Core area reports post floorplan:**  
![die_area_report_fp](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/2eb2a0db-fca9-4f94-b366-a70bf910ddb6)  
![core_area_report](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/110a0960-d34b-4808-8faf-a729e14a9f5e)


#### Placement  

The main goal of placement is to determine the physical locations of synthesized logic elements (cells) on the IC chip.It involves deciding where each logic element should be placed on the chip, considering factors such as area utilization, timing constraints, and power consumption.  

Key Considerations:  

- Area Utilization: Placement aims to efficiently use the available silicon area on the chip.
- Timing Constraints: It influences the physical distances between logic elements, impacting signal propagation delays.
- Power Consumption: Placement decisions can affect power consumption, and optimization strategies may be applied.
- Hierarchy: For complex designs, hierarchical considerations may influence placement decisions.

```
run_placement
```
Post placement, the design can be viewed on magic within results/placement directory. Run the following command in that directory:  
```
magic -T /home/rachana/open_pdks/sky130/magic/sky130.tech lef read ../../tmp/merged.max.lef def read wrapper.def &
```
![magic_placement](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/fa61d674-0dd3-4192-a12e-97118437db5d)  

#### CTS  
Clock tree synteshsis is used to create the clock distribution network that is used to deliver the clock to all sequential elements. The main goal is to create a network with minimal skew across the chip. H-trees are a common network topology that is used to achieve this goal.

The purpose of building a clock tree is enable the clock input to reach every element and to ensure a zero clock skew. H-tree is a common methodology followed in CTS.  

```
run_cts
```
![run_cts](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/34a53442-b4df-4b67-989a-40ce16080015)    

**Timing reports:**  
![setup_hold_cts](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/4cca9204-a1a9-4b7b-8770-eb487771b611)  

**Area Report:**  
![image](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/0d48e139-878c-466d-8af3-977ce4691462)  

**Skew report:**  
![cts_skew_report](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/bda2b4e5-14f2-4df3-b724-2d290b4e9657)  

**Power Report:**  
![cts_power_report](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/ad9e2834-f981-413e-932b-f0d632adfa46)


#### Routing  
The main goal of routing is to establish physical connections (interconnections or wires) between the placed logic elements on the IC chip.It involves creating the metal traces that connect the outputs of one logic element to the inputs of another, facilitating the flow of signals.  

Key Considerations:  

- Wirelength: Routing aims to minimize the total wirelength of interconnections, reducing resistance and capacitance.
- Timing Closure: Routing plays a critical role in achieving timing closure by ensuring that signal paths meet the specified timing constraints.
- Congestion Management: Routing algorithms manage congestion to avoid issues associated with high-density interconnections in specific regions.
- Signal Integrity: Routing aims to preserve signal integrity by minimizing signal distortions and noise.
  
```
run_routing
```
![run_routing](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/8c872397-0476-4842-8d39-ea00df57b473)   

Layout in magic tool post routing: The design can be viewed on magic within results/routing directory. Run the following command in that directory:  
```
magic -T /home/rachana/open_pdks/sky130/magic/sky130.tech lef read ../../tmp/merged.max.lef def read wrapper.def &
```
![magic_routing](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/a7a22b3e-fdac-4a38-b0e6-bf57aee976d9)
![routing_zoomed_in](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/c33f24c8-8a4c-4b52-93fa-483d568c5966)  
**Area of the design**  
![area_of_design](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/99d33bee-6396-4606-b545-be6dc2ed0a62)   

**Timing Report post Routing**  
![routing_timing_report](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/d205bed9-2aac-4ef1-9519-5b85725aa607)   

**Area Report post Routing**  
![routing_area_report](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/b80b7b7d-b129-4ee4-af62-7bff00b62e4f)  

**Skew Report post Routing**  
![routing_skew_report](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/1e7bdd50-f98a-4731-b1cd-5c58f95bc998)  

**Power Report post Routing**  
![routing_power_report](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/8da5f6a5-e527-45cf-ab1d-5b180c0584c5)

**Reporting zero DRC Violations**  
![drc_violations_routing](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/ec5f52fd-429a-4c78-b6ae-6da5d04beaee)

**Performance Calculation**  
We have given Clock period of 50ns in config.json file , setup slack we got after routing is 13.29ns  
```
                              1
Max Performance =  -----------------------------
                     clock period - slack(setup)
Max Performance = 27.24 Mhz
```

#### Magic 
```
run_magic
run_magic_spice_export
run_magic_drc
run_netgen
run_magic_antenna_check
```

![run_magic](https://github.com/Rachana-Kaparthi/Sound-based-smart-switch/assets/140998470/9441da0a-db3c-4c4a-8868-b486ebab7751)

### VLSI INTERACTIVE OPENLANE FLOW

```

cd OpenLane/ 
make mount 
{ If Error occurs use the below commands in OpenLane directory:
sudo chown $USER /var/run/docker.sock 
PYTHON_BIN=python3 make mount
}

./flow.tcl -interactive
package require openlane 0.9
prep -design picorv32a
run_synthesis
run_floorplan
run_placement
run_cts
gen_pdn
run_routing
run_magic
run_magic_spice_export
run_magic_drc
run_netgen
run_magic_antenna_check

```


## Acknowledgement   

- Kunal Ghosh, VSD Corp. Pvt. Ltd.
- Mayank Kabra (Founder, Chipcron Pvt. Ltd.)
- Bhargav, Colleague, IIIT B
- Alwin Shaju,Colleague, IIIT B
- chatgpt

## References
1. https://openlane.readthedocs.io/en/latest/
2. https://luplab.gitlab.io/rvcodecjs/
3. https://github.com/bhargav-vlsi/RISCV-Display-controller
4. https://github.com/SakethGajawada/RISCV_GNU
5. https://github.com/riscv-collab/riscv-gnu-toolchain
6. https://github.com/riscv-software-src/riscv-isa-sim
7. https://github.com/alwinshaju08/IIITB_Waterlevel_detector
8. https://www.vsdiat.com













