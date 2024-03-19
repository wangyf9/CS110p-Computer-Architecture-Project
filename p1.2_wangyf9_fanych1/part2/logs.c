#include <stdlib.h>
#include <string.h>
#include "logs.h"
#include "execute_utils.h"


/*******************************
 * Helper Functions
 *******************************/

/* Creates a new empty Logs and returns a pointer to that Logs. 
   If memory allocation fails, you should call allocation_failed().
 */
Logs *create_logs() {
    /*initialize*/
    Logs *new_log = NULL;
    /*malloc for space*/
    new_log = (Logs*)malloc(sizeof(Logs));
    /*check if NULL*/
    if(!new_log){
        allocation_failed();
        return NULL;
    }
    /*initialize*/
    new_log->cap = 10;
    new_log->size = 0;
    /*malloc for space*/
    new_log->nodes = (Logsnode*)malloc(new_log->cap * sizeof(Logsnode));
    /*check if NULL*/
    if(!new_log->nodes){
        free(new_log);
        allocation_failed();
        return NULL;
    }
    return new_log;
    /* YOUR CODE HERE */
}

/* Frees the given Logs and all associated memory. */
void free_logs(Logs *logs) {
    /*int i;*/
    /*check if NULL*/
    if(!logs){
        return;
    }
    /*check if NULL*/
     free(logs->nodes);
    /*free logs*/
    free(logs);
    /* YOUR CODE HERE */
}

/* Record the current status and save in a logs component.
   Think about what information needs to be recorded.
   Logs can resize to fit an arbitrary number of entries. 
   (so you should use dynamic memory allocation).
 */
void record(Instruction inst, Processor *cpu, Byte *mem, Logs *logs)
{
    /*parameters*/
    Logsnode *newnode;
    int newcap;
    Address address; 
    int i;
    
    
    /*check if NULL*/
    if(!cpu || !mem ||!logs){
        return;
    }
    /*check if oversize*/
    if(logs->size >= logs->cap){
        /*set new capacity*/
        newcap = 2 * logs->cap;
        /*realloc*/
        newnode = (Logsnode*)realloc(logs->nodes, newcap * sizeof(Logsnode));
        /*check if NULL*/
        if(newnode == NULL){
            allocation_failed();
            return;
        }
        /*put in the values*/
        logs->nodes = newnode;
        /*change the capacity*/
        logs->cap = newcap;
    }
    /*check if NULL*/
    if(!logs->nodes){
        allocation_failed();
        return;
    }
    
    /*size++*/
    logs->size ++;

    for(i = 0;i < 32;i++){
        (logs->nodes + (logs->size-1))->reg[i] = 0x00000000;
    }
    /*copy reg*/
    
    for(i = 0;i < 32;i++){
        (logs->nodes + (logs->size-1))->reg[i] = cpu->reg[i];
    }
    /*set status*/
    (logs->nodes + (logs->size-1))->status = cpu->stat;
    /*set inst*/
    (logs->nodes + (logs->size-1))->inst = inst;
    /*set PC*/
    (logs->nodes + (logs->size-1))->PC = cpu->PC;
    /*check if s Type*/
    if((inst.bits & 0x7F) == 0x23){
        address = cpu->reg[inst.stype.rs1] + get_store_offset(inst);

        /*in the text*/
        /*else if(address < DATA_BASE){
            new_address = MEMORY_TEXT + address - TEXT_BASE;
        }*/
        /*check if sb*/
        if(inst.stype.funct3 == 0x0){
            (logs->nodes + (logs->size-1))->Value = load(mem,address,LENGTH_BYTE,1);
        }
        /*check if sh*/
        else if(inst.stype.funct3 == 0x1){
            (logs->nodes + (logs->size-1))->Value = load(mem,address,LENGTH_HALF_WORD,1);
        }
        /*check if sw*/
        else if(inst.stype.funct3 == 0x2){
            (logs->nodes + (logs->size-1))->Value = load(mem,address,LENGTH_WORD,1);
        }
        /*none of above*/
        else{
            return;
        }
    }
    
    /*malloc for space*/
    /*
    (logs->nodes + (logs->size-1))->reg= malloc(32 * sizeof(Register));
    if((logs->nodes + (logs->size-1))->reg == NULL){
        allocation_failed();
        return;
    }*/
    
    /* YOUR CODE HERE */
}

/* Back to the previous state recorded in a logs component.
   if success, return 0.
   if fail, for example, no previous state, return -1.
 */
int undo(Processor *cpu, Byte *mem, Logs *logs) {
    Address address; 
    int i;
    /*check if NULL*/
    if(!cpu || !mem || !logs){
        return -1;
    }
    /*check if too small*/
    if(logs->size < 1){
        return -1;
    }
    /*size--*/
    logs->size --;
    /*copy the reg*/
    for(i = 0;i < 32;i++){
        cpu->reg[i] = (logs->nodes + logs->size)->reg[i];
    }
    
    /*copy the status*/
    cpu->stat = (logs->nodes + logs->size)->status;
    /*copy the PC*/
    cpu->PC = (logs->nodes + logs->size)->PC;
    /*for(i = 0;i < 32;i++){
        free((logs->nodes + logs->size)->reg + i);
    }*/
    
    /*check if s type*/
    if(((logs->nodes + logs->size)->inst.bits & 0x7F) == 0x23){
        address = (logs->nodes + logs->size)->reg[(logs->nodes + logs->size)->inst.stype.rs1] + get_store_offset((logs->nodes + logs->size)->inst);

        /*in the text*/
        /*else if(address < DATA_BASE){
            new_address = MEMORY_TEXT + address - TEXT_BASE;
        }*/
        /*check if sb*/
        if((logs->nodes + logs->size)->inst.stype.funct3 == 0x0){
            store(mem,address,LENGTH_BYTE,(logs->nodes + logs->size)->Value,1);
        }
        /*check if sh*/
        else if((logs->nodes + logs->size)->inst.stype.funct3 == 0x1){
            store(mem,address,LENGTH_HALF_WORD,(logs->nodes + logs->size)->Value,1);
        }
        /*check if sw*/
        else if((logs->nodes + logs->size)->inst.stype.funct3 == 0x2){
            store(mem,address,LENGTH_WORD,(logs->nodes + logs->size)->Value,1);
        }
        /*none of above*/
        else{
            return -1;
        }
    }
    return 0;
    /* YOUR CODE HERE */
}

/* Add function definition here if you need */