This is our course website https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/index.html, if you want to learn more details.

## Project 1 Make a Toy Venus 
In Project 1, you will make a simple toy Venus, consisting of both assembler and emulator. If you are not yet familiar with Venus, try it out now. At the end of this project, you will understand the C programming language and the underlying execution of RISC-V assembly.

And for Venus which is a useful RISC-V simulator, you can get through this link, https://venus.cs61c.org/.
### Project 1.1 A RISC-V Assembler

Our assembler is a simple two-pass assembler. It will keep .text and .data segment apart. The linking process is not considered.

At a high level, the functionality of our assembler can be divided as follows.

1. Pass 1: Reads an input (.s) file. Comments are stripped. Constructs the .data segment in a data file, and the address of each label is recorded into a symbol table. Theses labels are validated here. The .text segment code will be recorded in a code file. This pass will output 3 files: data file, symbol table file, and an original code file.
2. Pass 2: Reads the symbol table file and original file. Then translates each instruction to basic code and machine code. Pseudo-instructions is translated here. Instruction syntax and arguments are validated at this step. This pass will output 2 files: basic code file and machine code file.
### Project 1.2 A RISC-V Emulator
Now we need to complete a simple emulator in C programming language to execute instructions. The emulator can be divided into the following components.

1. Launching and Running: This part, mainly in emulator.c, is responsible for creating a emulator and performing operations such as run(), step(), prev(), etc. We've done this part for you so you can ignore it!
2. Execution: This part, mainly in execute.c and execute_utils.c, is responsible for executing an instruction and finishing the corresponding operations on cpu and memory. You need to work on this component.
3. Memory: This part, mainly in mem.c, is responsible for simple conversion of memory addresses and manipulation of memory such as store() and load(). You need to work on this component.
4. Logs component: This part, mainly in logs.c, is responsible for record and save logs to go back in time. You need to work on this component.

## Project 2 Build a CPU from Zero
In project 2, you are required to build up a cpu by using Logisim with every built-in blocks you need in it. In the first part, you have to implement the ALU and Regfile first which are helpful in the latter section. After finishing the part 2, you will have a simple cpu which can be used to deal with some instructions providing by Project 1. Finally, you will have a deepen understanding of handware which you have learned in class.
### Project 2.1 ALU and Regfile
 1.  AlU : Your first task is to create an ALU that supports all the operations needed by the instructions in our ISA. Please note that we treat overflow as RISC-V does with unsigned instructions, meaning that we ignore overflow.

2. Reg file : As you learned in class, RISC-V architecture has 32 registers. However, in this project, You will only implement 9 of them (specified below) to save you some repetitive work. This means your rs1, rs2, and rd signals will still be 5-bit, but we will only test you on the specified registers.
Your RegFile should be able to write to or read from these registers specified in a given RISC-V instruction without affecting any other registers. There is one notable exception: your RegFile should NOT write to x0, even if an instruction try. Remember that the zero register should ALWAYS have the value 0x0. You should NOT gate the clock at any point in your RegFile: the clock signal should ALWAYS connect directly to the clock input of the registers without passing through ANY combinational logic.
### Project 2.2 CPU
In this part, we need to implement a cpu by constructing five stages, that is
1. Stage 1: Instruction Fetch
   The main thing we are concerned about in this stage is: how do we get the current instruction? From lecture, we know that instructions are stored in the instruction memory, and each of these instructions can be accessed through an address.


2. Stage 2: Instruction Decode
   Now that we have our instruction coming from the instruction input, we have break it down in the Instruction Decode step, according to the RISC-V instruction formats you have learned.
3. Stage 3: Execute
   The Execute stage, also known as the ALU stage, is where the computation of most instructions is performed. This is also where we will introduce the idea of using a Control Module.


4. Stage 4: Memory
   The memory stage is where the memory can be written to using store instructions and read from using load instructions. Because the addi instruction does not use memory, we will not spend too much time here.
5. Stage 5: Write back 
   The write back stage is where the results of the operation is saved back to the registers. Although not all instructions will write back to the register file (can you think of some which do not?), the addi instruction does.


## Project 3 From CA to LBM
In this project, we hope you can use all knowledge about computer architecture that your have learned in this course to optimize lattice Boltzmann methods (LBM). 

There are a lot of parts you can optimize and a lot of techniques you can use. And what we have used are as follows, 
1. Algorithm
2. Compiler
3. Multithreading
4. SIMD instructions
5. ……
## Project 4 Sokoban Game on Longan Nano
As CS110 is coming to an end, it's time to put the skills we've learned into practice.

In this project, you will code in C language and RISC-V assembly to implement a Sokoban game.
You will also use platformIO to cross-compile and generate a program for the Longan Nano development board.

And for demands, we have implemented the following requirements, 
1. (Required) Input Keys Debouncing
2. (Required) Displaying a Character with Image
3. (Required) Level Selection Scene
4. (Required) Game Play Scene
5. (Optional) Incremental Rendering
6. (Optional) Welcome Scene
7. (Optional) ScoreBoard Scene

In the end, for the pre video, you can have a overview in my website.
