#include <stdio.h>
#include <stdlib.h>
#include "execute_utils.h"


/* You may find implementing this function helpful */

/* Signed extension of a number to 32 bits. 
   size is the number of valid bits of field.
   e.g. field=0x8a, size=8, return 0xFFFFFF8a.
        field=0x7a, size=8, return 0x0000007a.
 */
int bitSigner(unsigned int field, unsigned int size)
{
    /* YOUR CODE HERE */
    /*implement signed extension*/
    unsigned extension = (field & (1 << (size - 1))) ? ~((1 << (size - 1)) - 1) : 0;
    return field | extension;
}

/* Return the imm from instruction, for details, please refer to I type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_imm_operand(Instruction instruction)
{   
    /* YOUR CODE HERE */
    /*because we need refer to the i type*/
    /*and i type we need is [11:0]*/
    int imm_operand = bitSigner(instruction.itype.imm,12);
    /*return the answer*/
    return imm_operand;
}
/* Return the offset from instruction, for details, please refer to SB type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_branch_offset(Instruction instruction)
{   
    /* YOUR CODE HERE */
    /*in this function we need refer to the sb type and it has imm5 and imm7*/
    int sum = 0;/*return value*/
    int imm_1 = instruction.sbtype.imm5 >> 1;/*imm[4:1]*/
    int imm_2 = instruction.sbtype.imm5 - (imm_1 << 1);/*imm[11]*/
    int imm_3 = instruction.sbtype.imm7 >> 6;/*imm[12]*/
    int imm_4 = instruction.sbtype.imm7 - (imm_3 << 6);/*imm[10:5]*/
    /*because we have ignored original 0th bit*/
    sum += (imm_1<<1)+(imm_2<<11)+(imm_3<<12)+(imm_4<<5);
    /*return to finish*/
    return bitSigner(sum,13);
}

/* Return the offset from instruction, for details, please refer to UJ type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_jump_offset(Instruction instruction)
{
    /* YOUR CODE HERE */
    /*uj type just have one imm*/
    int sum=0;
    /*divide it into two part so that we can calculate it more easily*/
    int imm_1_sum = instruction.ujtype.imm >> 9;/*imm[20]+imm[10:1]*/
    int imm_2_sum = instruction.ujtype.imm - (imm_1_sum << 9);/*imm[11]+imm[19:12]*/
    int imm_1_1 = imm_1_sum>>10;/*imm[20]*/
    int imm_1_2 = imm_1_sum-(imm_1_1<<10);/*imm[10:1]*/
    int imm_2_1 = imm_2_sum>>8;/*imm[11]*/
    int imm_2_2 = imm_2_sum-(imm_2_1<<8);/*imm[19:12]*/
    /*sum*/
    sum = sum + (imm_1_1 << 20)+(imm_1_2 << 1) + (imm_2_1 << 11) + (imm_2_2 << 12);
    /*return to finish*/
    return bitSigner(sum,21);
}

/* Return the offset from instruction, for details, please refer to S type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_store_offset(Instruction instruction)
{
    /* YOUR CODE HERE */
    /*because refer to s type, it also has two imm*/
    int sum = instruction.stype.imm5 + (instruction.stype.imm7 << 5);
    /*return to finish*/
    return bitSigner(sum,12);

}
