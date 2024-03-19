#include <stdio.h>
#include "execute.h"
#include "execute_utils.h"
#include "mem.h"


/*******************************
 * Helper Functions
 *******************************/

/* Function handle with error machine code. You should skip the error 
   machine code after call this funcion
*/
void raise_machine_code_error(Instruction instruction){
    write_to_log("Invalid Machine Code: 0x%08x\n", instruction.bits);
}

/* Function handle with undefined ecall parameter.
*/
void raise_undefined_ecall_error(Register a0){
    write_to_log("Undefined Ecall Number: %d\n", a0);
}

/* Function to exit current program */
void exit_program(Processor *p) {
    if (p->stat==1) {
        /* set cpu state unwork */
        p->stat=0;
        write_to_log("Exited with error code 0.\n");
    }
    else {
        write_to_log("Error: program already exit.\n");
    }
}

/* 
    In this section you need to complete execute instruction. This should modify 
    the appropriate registers, make any necessary calls to memory, and updatge the program
    counter to refer You will find the functions in execute_utils.h very useful. So it's 
    better to finish execute_utils.c first. The following provides you with a general 
    framework. You can follow this framework or add your own functions, but please 
    don't change declaration or feature of execute(). Case execute() is the entry point
    to the execution phase. The execute_*() is specific implementations for each 
    instruction type. You had better read types.h carefully. Think about the usage 
    of the union defined in types.h
 */

void show_bit(int val){
    int i;
    for(i = 31;i >= 0;i--){
        printf("%d", (val >> i) & 1);
        if(i % 4 == 0) printf(" ");
    }
    puts("");
}

void execute(Instruction inst, Processor *p, Byte *memory)
{   
    /*printf("Execute Instruction %08x\n",inst.bits);
    show_bit(inst.bits);*/
    switch (inst.opcode.opcode)/*check type and call related specific function*/
    { 
    /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
    case 0x33:/*rtype 0x33 0110011=51*/
        execute_rtype(inst,p);/*call specific function*/
        break;
    case 0x13:/*itype 0x13=0010011=19*/
        execute_itype_except_load(inst,p);
        break;
    case 0x03:/*load 0x03=0000011=3*/
        execute_load(inst,p,memory);
        break;
    case 0x67:/*jalr 0x67=1100111=103*/
        execute_jalr(inst,p);
        break;
    case 0x6f:/*jal=0x6f=1101111=111*/
        execute_jal(inst,p);
        break;
    case 0x73:/*ecall=0x73=1110011=115*/
        execute_ecall(p,memory);
        break;
    case 0x63:/*branch=0x63=1100011=99*/
        execute_branch(inst,p);
        break;
    case 0x23:/*store=0x23=0100011=35*/
        execute_store(inst,p,memory);
        break;
    case 0x17:/*auipc=0x17=0010111=23*/
        execute_utype(inst,p);
        break;
    case 0x37:/*alui=0x37=0110111=55*/
        execute_utype(inst,p);
        break;
    default: /* undefined opcode */
        raise_machine_code_error(inst);
        p->PC += 4;
        break;
    }
    p->reg[0]=0;/*set x0=0*/
}

void execute_rtype(Instruction inst, Processor *p)
{   
    sDouble imm;/*as an intermediate variable*/
    /* YOUR CODE HERE */
    switch (inst.rtype.funct3)/*use func3 to do the first classification*/
    {
    case 0:/*0x0*/
        switch (inst.rtype.funct7)/*use func7 to do the second classification*/
        {
            case 0:/*add=0x00=0*/
                p->reg[inst.rtype.rd]=p->reg[inst.rtype.rs1]+p->reg[inst.rtype.rs2];
                break;
            case 1:/*mul=0x01=1 and it represents the lower 32 bits*/
                p->reg[inst.rtype.rd]=(((sWord)(p->reg[inst.rtype.rs1])*(sWord)(p->reg[inst.rtype.rs2]))&0xFFFFFFFF);
                break;
            case 32:/*sub=0x20=0100000*/
                p->reg[inst.rtype.rd]=p->reg[inst.rtype.rs1]-p->reg[inst.rtype.rs2];
                break;
            default:/*other cases*/
                raise_machine_code_error(inst);
                break;
        }
        break;
    case 1:/*0x1*/
        switch (inst.rtype.funct7)/*use func7 to do the second classification*/
        {
            case 0:/*sll=0x00=0*/
                p->reg[inst.rtype.rd]=p->reg[inst.rtype.rs1]<<p->reg[inst.rtype.rs2];
                break;
            case 1:/*mulh=0x01=1, which is higher  32  bits*/
                /*32+32=64*/
               /*printf("rs1========%d",)*/
                imm=(sDouble)((sDouble)(sWord)(p->reg[inst.rtype.rs1])*(sDouble)(sWord)(p->reg[inst.rtype.rs2]));
                p->reg[inst.rtype.rd]=(Word)(((Double)(imm>>32)) & 0xFFFFFFFF);
                break;
            default:/*other cases*/
                raise_machine_code_error(inst);
                break;
        }
        break;
    case 2:/*0x2*/
        switch (inst.rtype.funct7)/*use func7 to do the second classification*/
        {
            case 0:/*slt=0x00*/
                p->reg[inst.rtype.rd]=((sWord)(p->reg[inst.rtype.rs1])<(sWord)(p->reg[inst.rtype.rs2]))?1:0;
                break;
            default:/*other cases*/
                raise_machine_code_error(inst);
                break;
        }
        break;
    case 3:/*0x3*/
        switch (inst.rtype.funct7)/*use func7 to do the second classification*/
        {
            case 0:/*sltu=0x00*/
                p->reg[inst.rtype.rd]=((Word)(p->reg[inst.rtype.rs1])<(Word)(p->reg[inst.rtype.rs2]))?1:0;
                break;
            default:/*other cases*/
                raise_machine_code_error(inst);
                break;
        }
        break;
    case 4:/*0x4*/
        switch (inst.rtype.funct7)/*use func7 to do the second classification*/
        {
            case 0:/*xor=0x00*/
                p->reg[inst.rtype.rd]=p->reg[inst.rtype.rs1]^p->reg[inst.rtype.rs2];
                break;
            case 1:/*div=0x01*/
                p->reg[inst.rtype.rd]=((sWord)(p->reg[inst.rtype.rs1]))/((sWord)(p->reg[inst.rtype.rs2]));
                break;
            default:/*other cases*/
                raise_machine_code_error(inst);
                break;
        }
        break;
    case 5:/*0x5*/
        switch (inst.rtype.funct7)/*use func7 to do the second classification*/
        {
            case 0:/*srl=0x00, logic implement 0*/
                p->reg[inst.rtype.rd]=p->reg[inst.rtype.rs1]>>p->reg[inst.rtype.rs2];
                break;
            case 32:/*sra=0x20, arithmetic implement sign extension*/
                p->reg[inst.rtype.rd]=((sWord)(p->reg[inst.rtype.rs1])>>p->reg[inst.rtype.rs2]);
                break;
            default:/*other cases*/
                raise_machine_code_error(inst);
                break;
        }
        break;
    case 6:/*0x6*/
        switch (inst.rtype.funct7)/*use func7 to do the second classification*/
        {
            case 0:/*or=0x00*/
                p->reg[inst.rtype.rd]=p->reg[inst.rtype.rs1]|p->reg[inst.rtype.rs2];
                break;
            case 1:/*rem=0x01*/
                p->reg[inst.rtype.rd]=((sWord)(p->reg[inst.rtype.rs1]))%((sWord)(p->reg[inst.rtype.rs2]));
                break;
            default:/*other cases*/
                raise_machine_code_error(inst);
                break;
        }
        break;
    case 7:/*0x7*/
        switch (inst.rtype.funct7)/*use func7 to do the second classification*/
        {
            case 0:/*or=0x00*/
                p->reg[inst.rtype.rd]=p->reg[inst.rtype.rs1]&p->reg[inst.rtype.rs2];
                break;
            default:/*other cases*/
                raise_machine_code_error(inst);
                break;
        }
        break;
    default:
        raise_machine_code_error(inst);
        break;
    }
    p->PC+=4;/*next instruction*/
}

void execute_itype_except_load(Instruction inst, Processor *p)
{
    /* YOUR CODE HERE */
    /*create two variables in order to simplify the following work*/
    int imm=get_imm_operand(inst);/*for srai and srli and slli the range of them is 0:31, just five bits*/
    Word real_imm_for_spe=imm&0x1F;/*get real imm for three special cases,because 0-31 all positive*/
    Word function7_for_i=(imm>>5)&0x7F;/*get function7, 7bits*/
    switch (inst.itype.funct3)/*first check by func3*/
    {
    case 0:/*0x0 addi +*/
        p->reg[inst.itype.rd]=p->reg[inst.itype.rs1]+get_imm_operand(inst);
        break; 
    case 1:/*0x1 slli left logic, so it doesnot matter*/
        if(function7_for_i!=0x00){
            raise_machine_code_error(inst);
            break;
        }
        p->reg[inst.itype.rd]=p->reg[inst.itype.rs1]<<get_imm_operand(inst);
        break;
    case 2:/*0x2 slti it is used for signed number*/
        p->reg[inst.itype.rd]=((sWord)p->reg[inst.itype.rs1]<(sWord)get_imm_operand(inst))?1:0;
        break;
    case 3:/*0x3 sltiu it is used for unsigned number*/
        p->reg[inst.itype.rd]=((Word)p->reg[inst.itype.rs1]<(Word)get_imm_operand(inst))?1:0;
        break;
    case 4:/*0x4 xori ^*/
        p->reg[inst.itype.rd]=p->reg[inst.itype.rs1]^get_imm_operand(inst);
        break;
    case 5:/*0x5*/
        switch (function7_for_i)/*second check*/
        {
        case 0:/* srli right logic*/
            p->reg[inst.itype.rd]=p->reg[inst.itype.rs1]>>get_imm_operand(inst);
            break;
        case 32:/*srai right arithmetic*/
            p->reg[inst.itype.rd]=(((sWord)(p->reg[inst.itype.rs1]))>>real_imm_for_spe);
            break;
        default:
            raise_machine_code_error(inst);
            break;
        }
        break;
    case 6:/*0x6 ori |*/
        p->reg[inst.itype.rd]=p->reg[inst.itype.rs1]|get_imm_operand(inst);
        break;
    case 7:/*0x7 and &*/
        p->reg[inst.itype.rd]=p->reg[inst.itype.rs1]&get_imm_operand(inst);
        break;
    default:
        raise_machine_code_error(inst);
        break;
    }
    p->PC+=4;/*next instruction*/
}

/* You may need to use exit_program() */
void execute_ecall(Processor *p, Byte *memory)
{
    /* YOUR CODE HERE */
    Address new_address;
    Address address = p->reg[11];
    switch (p->reg[10])/*determine different cases by the value of a0*/
    {
    case 1:
        write_to_log("%d",p->reg[11]);/*print the value of a1 as an integer*/
        break;
    case 4:/*print the string at address a1*/
        /*address conversion??????*/
        /*in the stack*/
        if(address >= (STACK_BASE - (MEMORY_SIZE - MEMORY_STACK)) && address <= STACK_BASE){
            /*conversion*/
            new_address = MEMORY_STACK + (address - (STACK_BASE - (MEMORY_SIZE - MEMORY_STACK)));
        }
        /*in the data*/
        else if(address >= DATA_BASE && address < (DATA_BASE + (MEMORY_STACK - MEMORY_DATA))){
            /*conversion*/
            new_address = MEMORY_DATA + (address - DATA_BASE);
        }
        /*in the text*/
        else if(address < (TEXT_BASE + (MEMORY_DATA - MEMORY_TEXT))){
            /*conversion*/
            new_address = MEMORY_TEXT + (address  - TEXT_BASE);
        }
        /*printf("p->reg[11] = [%x]\n",p->reg[11]);*/
        write_to_log("%s",(char*)&(memory[new_address]));/**/
        /*printf("%d\n",*(memory + new_address));*/
        break;
    case 10:/*exit or end of code indicator*/
        exit_program(p);
        break;
    case 11:/*print value of a1 as a char*/
        write_to_log("%c",p->reg[11]);
        break;
    default:/*other a0*/
        raise_undefined_ecall_error(p->reg[10]);
        break;
    }
    p->PC+=4;/*next instruction*/
}

void execute_branch(Instruction inst, Processor *p)
{
    
    /* YOUR CODE HERE */
    int imm=get_branch_offset(inst);
    int rs1=p->reg[inst.sbtype.rs1];
    int rs2=p->reg[inst.sbtype.rs2];
    switch (inst.sbtype.funct3)/*first check*/
    {
    case 0:/*0x0 beq*/
        if(p->reg[inst.sbtype.rs1]==p->reg[inst.sbtype.rs2]){/*satisfy jump*/
            p->PC += imm;
        }
        else{/*else read next instruction*/
            p->PC += 4;
        }
        break;
    case 1:/*0x1 bne*/
        if(p->reg[inst.sbtype.rs1]!=p->reg[inst.sbtype.rs2]){/*satisfy jump*/
            p->PC+=imm;
        }
        else{/*else read next instruction*/
            p->PC+=4;
        }
        break;
    case 4:/*0x4 blt*/
        if((sWord)rs1<(sWord)rs2){/*satisfy jump*/
            p->PC+=imm;
        }
        else{/*else read next instruction*/
            p->PC+=4;
        }
        break;
    case 5:/*0x5 bge*/
        if((sWord)rs1>=(sWord)rs2){/*satisfy jump*/
            p->PC+=imm;
        }
        else{/*else read next instruction*/
            p->PC+=4;
        }
        break;
    case 6:/*0x6 bltu*/
        if((Word)(rs1)<(Word)(rs2)){/*satisfy jump*/
            p->PC+=imm;
        }
        else{/*else read next instruction*/
            p->PC+=4;
        }
        break;
    case 7:/*0x7 bgeu*/
        if((Word)(rs1)>=(Word)(rs2)){/*satisfy jump*/
            p->PC+=imm;
        }
        else{/*else read next instruction*/
            p->PC+=4;
        }
        break;
    default:/*other cases*/
        raise_machine_code_error(inst);
        p->PC+=4;/*else read next instruction*/
        break;
    }   
}

void execute_load(Instruction inst, Processor *p, Byte *memory)
{
    switch (inst.itype.funct3)
    {
    /*lb*/
    case 0:
        /*R[rd] ← SignExt(Mem(R[rs1] + offset, byte))*/
        p->reg[inst.itype.rd] = bitSigner(load(memory, (p->reg[inst.itype.rs1] + get_imm_operand(inst)), LENGTH_BYTE, 1), 8);
        /*PC+4*/
        p->PC += 4;
        break;
    /*lh*/
    case 1:
        /*R[rd] ← SignExt(Mem(R[rs1] + offset, half))*/
        p->reg[inst.itype.rd] = bitSigner(load(memory, (p->reg[inst.itype.rs1] + get_imm_operand(inst)), LENGTH_HALF_WORD, 1), 16);
        /*PC+4*/
        p->PC += 4;
        break;
    /*lw*/
    case 2:
        /*R[rd] ← Mem(R[rs1] + offset, word)*/
        p->reg[inst.itype.rd] = load(memory, (p->reg[inst.itype.rs1] + get_imm_operand(inst)), LENGTH_WORD, 1);
        /*PC+4*/
        p->PC += 4;
        break;
    /*lbu*/
    case 4:
        /*R[rd] ← U(Mem(R[rs1] + offset, byte))*/
        p->reg[inst.itype.rd] = (Word)load(memory, (p->reg[inst.itype.rs1] + get_imm_operand(inst)), LENGTH_BYTE, 1);
        /*PC+4*/
        p->PC += 4;
        break;
    /*lhu*/
    case 5:
        /*R[rd] ← U(Mem(R[rs1] + offset, half))*/
        p->reg[inst.itype.rd] = (Word)load(memory, (p->reg[inst.itype.rs1] + get_imm_operand(inst)), LENGTH_HALF_WORD, 1);
        /*PC+4*/
        p->PC += 4;
        break;
    default:
        /*invalid machine code*/
        raise_machine_code_error(inst);
        /*PC+4*/
        p->PC += 4;
        break;
    }
    /* YOUR CODE HERE */
}

void execute_store(Instruction inst, Processor *p, Byte *memory)
{
    /*initialize*/
    Address address = 0;
    switch (inst.stype.funct3)
    {
    /*sb*/
    case 0:
        /*address = R[rs1] + offset*/
        address = p->reg[inst.stype.rs1] + get_store_offset(inst);
        /*Mem(R[rs1] + offset) ← R[rs2][7:0]*/
        /*[7:0] will be solved in store()*/
        store(memory,address,LENGTH_BYTE,p->reg[inst.stype.rs2],1);/*what is the checkalign???*/
        /*PC + 4*/
        p->PC += 4;
        break;
    /*sh*/
    case 1:
        /*address = R[rs1] + offset*/
        address = p->reg[inst.stype.rs1] + get_store_offset(inst);
        /*Mem(R[rs1] + offset) ← R[rs2][15:0]*/
        /*[15:0] will be solved in store()*/
        store(memory,address,LENGTH_HALF_WORD,p->reg[inst.stype.rs2],1);/*what is the checkalign???*/
        /*PC + 4*/
        p->PC += 4;
        break;
    /*sw*/
    case 2:
        /*address = R[rs1] + offset*/
        address = p->reg[inst.stype.rs1] + get_store_offset(inst);
        /*Mem(R[rs1] + offset) ← R[rs2]*/
        store(memory,address,LENGTH_WORD,p->reg[inst.stype.rs2],1);/*what is the checkalign???*/
        /*PC + 4*/
        p->PC += 4;
        break;
    default:
        /*invalid machine code*/
        raise_machine_code_error(inst);
        /*PC+4*/
        p->PC += 4;
        break;
    }
    /* YOUR CODE HERE */
}

void execute_jal(Instruction inst, Processor *p)
{
    /*R[rd] ← PC + 4*/
    p->reg[inst.ujtype.rd] = p->PC +4;
    /*PC ← PC + {imm, 1b'0}*/
    p->PC += get_jump_offset(inst);
    /* YOUR CODE HERE */
}

void execute_jalr(Instruction inst, Processor *p)
{
    /*check funct3*/
    if(inst.itype.funct3 == 0){
        /*R[rd] ← PC + 4*/
        p->reg[inst.itype.rd] = p->PC + 4;
        /*PC ← R[rs1] + imm*/
        p->PC = p->reg[inst.itype.rs1] + get_imm_operand(inst);
    }
    else{
        raise_machine_code_error(inst);
        p->PC += 4;
    }
    /* YOUR CODE HERE */
}

void execute_utype(Instruction inst, Processor *p)
{
    switch (inst.utype.opcode)
    {
    /*auipc*/
    case 23:
        /*R[rd] ← PC + {offset, 12b'0}*/
        p->reg[inst.utype.rd] = p->PC + ((inst.utype.imm) << 12);
        /*PC+4*/
        p->PC += 4;
        break;
    /*lui*/
    case 55:
        /*R[rd] ← {offset, 12b'0}*/
        p->reg[inst.utype.rd] = (inst.utype.imm) << 12;
        /*PC+4*/
        p->PC += 4;
        break;
    default:
        /*invalid machine code*/
        raise_machine_code_error(inst);
        /*PC+4*/
        p->PC += 4;
        break;
    }
    /* YOUR CODE HERE */
}