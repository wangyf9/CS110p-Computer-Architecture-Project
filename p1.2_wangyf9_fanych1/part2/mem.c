#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mem.h"


/*******************************
 * Helper Functions
 *******************************/

/* Call this function in the following cases:
        1. Attempts to access a memory location that it is not allowed to access.
        2. Attempts to access a memory location in a way that is not allowed.
 */
static void raise_segmentation_fault(Address addr) {
    write_to_log("Error - segmentation fault at address %08x\n", addr);
}

/* Call this function if a data access is performed to an address 
   that is not aligned for the size of the access.
 */
static void raise_alignment_fault(Address addr) {
    write_to_log("Error - alignment fault at address %08x\n", addr);
}

/* This function is helpful to your alignment check.
 */
int check_alignment(Address address, Alignment alignment) {
    if (alignment == LENGTH_BYTE)
    {
        return 1;
    }
    /* HALF_WORD: The last bit is not used. */
    else if (alignment == LENGTH_HALF_WORD)
    {
        return address % 2 == 0;
    }
    /* WORD: The last two bits are not used. */
    else if (alignment == LENGTH_WORD)
    {
        return address % 4 == 0;
    }
    else {
        return 0;
    }
}

/* To store the data in the corresponding address in memory, 
   please note the following points:
        1. When check_align==1, check if the memory address is aligned
         with the data type. You need to use raise_alignment_fault().
        2. Think about when you need to use raise_segmentation_fault().
        3. Please perform an address conversion before actually store to memory.
 */

void store(Byte *memory, Address address, Alignment alignment, Word value,
           int check_align)
{
    /*initialize*/
    Address new_address = 0;
    /*address conversion??????*/
    /*in the stack*/
    if(address >= (DATA_BASE + (MEMORY_STACK - MEMORY_DATA)) && address < STACK_BASE){
            /*conversion*/
        new_address = STACK_BASE - address + MEMORY_STACK;
        if(MEMORY_SIZE <= new_address ){
            raise_segmentation_fault(address);
            return;
        }
    }
    /*in the data*/
    else if(address >= DATA_BASE && address < (DATA_BASE + (MEMORY_STACK - MEMORY_DATA))){
        /*conversion*/
        new_address = MEMORY_DATA + (address - DATA_BASE);
    }
    else{
        /*unmapped*/
        raise_segmentation_fault(address);
        return;
    }
    /*When check_align==1*/
    if(check_align == 1){
        /*check if the memory address is aligned with the data type*/
        if(!check_alignment(address,alignment)){
            
            /*use raise_alignment_fault()*/
            raise_alignment_fault(new_address);
            return;
        }
    }
    /*store to memory if BYTE*/
    if(alignment == LENGTH_BYTE){
        /*one byte*/
        memory[new_address] = value & 0xFF;
    }
    /*store to memory if HALFWORD*/
    else if(alignment == LENGTH_HALF_WORD){
        /*two byte*/
        memory[new_address] = value & 0xFF;
        /*right shift*/
        memory[new_address + 1] = (value >> 8) & 0xFF;
    }
    /*store to memory if WORD*/
    else if(alignment == LENGTH_WORD){
        /*four byte*/
        memory[new_address] = value & 0xFF;
        /*right shift*/
        memory[new_address + 1] = (value >> 8) & 0xFF;
        /*right shift*/
        memory[new_address + 2] = (value >> 16) & 0xFF;
        /*right shift*/
        memory[new_address + 3] = (value >> 24) & 0xFF;
    }

    /* YOUR CODE HERE */
    return;
}

/* To load the data in the corresponding address in memory, 
   please note the following points:
        1. When check_align==1, check if the memory address is aligned
         with the data type. You need to use raise_alignment_fault().
        2. Think about when you need to use raise_segmentation_fault().
        3. Please perform an address conversion before actually load from memory.
 */
Word load(Byte *memory, Address address, Alignment alignment, int check_align)
{
    /*initialize*/
    Address new_address = 0;
    Word result = 0;
    
    /*address conversion??????*/
    /*in the stack*/
    if(address >= (DATA_BASE + (MEMORY_STACK - MEMORY_DATA)) && address < STACK_BASE){
            /*conversion*/
        new_address = STACK_BASE - address + MEMORY_STACK;
        if(MEMORY_SIZE <= new_address ){
            raise_segmentation_fault(address);
            return 0;
        }
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
    else{
        /*unmapped*/
        raise_segmentation_fault(address);
        return 0;
    }

    /*When check_align==1*/
    if(check_align == 1){
        /*check if the memory address is aligned with the data type*/
        if(!check_alignment(address,alignment)){
            /*use raise_alignment_fault()*/
            raise_alignment_fault(new_address);
            return 0;
        }
    }
    /*load if BYTE*/
    if(alignment == LENGTH_BYTE){
        /*four byte*/
        result |= memory[new_address];
    }
    /*load if HALFWORD*/
    else if(alignment == LENGTH_HALF_WORD){
        /*four byte*/
        result |= memory[new_address];
        /*left shift*/
        result |= memory[new_address + 1] << 8;
    }
    /*load if WORD*/
    else if(alignment == LENGTH_WORD){
        /*four byte*/
        result |= memory[new_address];
        /*left shift*/
        result |= memory[new_address + 1] << 8;
        /*left shift*/
        result |= memory[new_address + 2] << 16;
        /*left shift*/
        result |= memory[new_address + 3] << 24;
    }
    /*return*/
    return result;
    /* YOUR CODE HERE */
}