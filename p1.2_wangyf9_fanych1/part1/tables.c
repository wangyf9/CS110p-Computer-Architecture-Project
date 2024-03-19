/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia 
   to be part of the project in Feb 2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tables.h"
/*include*/
const int SYMBOLTBL_NON_UNIQUE = 0;
const int SYMBOLTBL_UNIQUE_NAME = 1;
/*static data*/
/*******************************
 * Helper Functions
 *******************************/

void addr_alignment_incorrect()
{   
    /*false*/
    write_to_log("Error: address is not a multiple of 4.\n");
}

void name_already_exists(const char *name)
{   
    /*false*/
    write_to_log("Error: name '%s' already exists in table.\n", name);
}

void write_sym(FILE *output, uint32_t addr, const char *name)
{   
    /*write*/
    fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containg 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time.
   If memory allocation fails, you should call allocation_failed().
   Mode will be either SYMBOLTBL_NON_UNIQUE or SYMBOLTBL_UNIQUE_NAME. You will
   need to store this value for use during add_to_table().
 */
SymbolTable *create_table(int mode)
{
    /*create null symboltable*/
    SymbolTable *symboltable = NULL;
    /*apply for memory space*/
    symboltable = malloc(sizeof(SymbolTable));
    /*apply failed*/
    if(!symboltable){
        allocation_failed();
    }
    /*initialize*/
    symboltable->cap = 10;
    symboltable->len = 0;
    symboltable->mode = mode;  
    /*apply for memory space for table*/
    symboltable->tbl = malloc(symboltable->cap * sizeof(Symbol));
    /*check*/
    if(!symboltable->tbl){
        /*false free symboltable and call the function*/
        free(symboltable);
        allocation_failed();
    }   
    /*symboltable->tbl->name = calloc(1,sizeof(char));
    if(!symboltable->tbl->name){
        allocation_failed();
    }
    symboltable->tbl->addr = 0;*/
    /*finished*/
    return symboltable;
    /* YOUR CODE HERE */
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable *table)
{   
    /*create new variables*/
    uint32_t i;
    /*check input*/
    if(!table){
        /*null, no other operations*/
        return;
    }
    /*check input.table*/
    if(table->tbl != NULL){
        /*free tbl.name*/
        for(i = 0;i < table->len;i++){
            if(table->tbl[i].name != NULL){
                free((table->tbl + i)->name);
            }
        }
        /*free tbl*/
        free(table->tbl);
    }
    /*direct free table because there are not tbl*/
    free(table);
    /* YOUR CODE HERE */
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE. 
   1. ADDR is given as the byte offset from the first instruction. 
   2. The SymbolTable must be able to resize itself as more elements are added. 

   3. Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   4. If ADDR is not word-aligned and check_align==1, you should call 
   addr_alignment_incorrect() and return -1. 

   5. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists 
   in the table, you should call name_already_exists() and return -1. 

   6.If memory allocation fails, you should call allocation_failed(). 

   Otherwise, you should store the symbol name and address and return 0. 
 */
int add_to_table(SymbolTable *table, const char *name, uint32_t addr, int check_align)
{   /*create new variables*/
    uint32_t i;
    uint32_t newcap;
    Symbol *newtbl;
    /*char *newname ;
    if(!table || !name){
        return -1;
    }
    newname = malloc(strlen(name)+1);
    if(newname == NULL){
        allocation_failed();
        return -1;
    }
    strcpy(newname, name);*/
    /*check addr false*/
    if(addr % 4 != 0 && check_align == 1){
        /*return false*/
        addr_alignment_incorrect();
        return -1;
    }
    /*check whether unique*/
    if(table->mode == SYMBOLTBL_UNIQUE_NAME){
        for(i = 0;i < table->len ;i++){
            /*check whether exist*/
            if(strcmp((table->tbl + i)->name,name)==0 ){
                /*check whether already exists*/
                name_already_exists(name);
                /*return false*/
                return -1;
            }
        }
    }
    /*enlarge capcity*/
    if(table->len >= table->cap){
        newcap = 2 * table->cap;
        /*apply for more memory space*/
        newtbl = realloc(table->tbl,newcap * sizeof(Symbol));
        /*apply fail*/
        if(newtbl == NULL){
            allocation_failed();
            /*return false*/
            return -1;
        }
        /*assign*/
        table->tbl = newtbl;
        table->cap = newcap;
    }
    /*check null*/
    if(!(table->tbl)){
        /*return false */
        allocation_failed();
        return -1;
    }
    /*add tbl into table*/
    table->len ++;
    (table->tbl + (table->len-1))->name = malloc(strlen(name) + 1);
    /*check*/
    if((table->tbl + (table->len-1))->name == NULL){
        /*return false*/
        allocation_failed();
        return -1;
    }
    /*copy*/
    strcpy((table->tbl + (table->len-1))->name, name);
    /*change*/
    (table->tbl + (table->len-1))->addr = addr;
    /*finished*/
    return 0;
    /* YOUR CODE HERE */
}

/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable *table, const char *name)
{   /*create new variables*/
    uint32_t i;
    /*check input*/
    if(!table || !name){
        /*return false*/
        return -1;
    }
    for(i = 0;i < table->len;i++){
        /*check where is what we want*/
        if(strcmp((table->tbl + i)->name,name)==0){
            /*get the address we want*/
            return (table->tbl + i)->addr;
        }
    }
    /*finished, other cases all are false*/
    return -1;
    /* YOUR CODE HERE */
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_sym() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable *table, FILE *output)
{   
    /*create new variables*/
    uint32_t i;
    /*check input*/
    if(!table || !output){
        /*return false*/
        return;
    }
    for(i = 0;i < table->len;i++){
        /*write*/
        write_sym(output, (table->tbl + i)->addr, (table->tbl + i)->name);
    }
    /* YOUR CODE HERE */
}

/* DO NOT MODIFY THIS LINE BELOW */
/* ===================================================================== */
/* Read the symbol table file written by the `write_table` function, and
   restore the symbol table.
 */
SymbolTable *create_table_from_file(int mode, FILE *file)
{   
    /*create a new symbol table*/
    SymbolTable *symtbl = create_table(mode);
    /*create temporary store array*/
    char buf[1024];
    /*read every line in file*/
    while (fgets(buf, sizeof(buf), file))
    {   
        /*create new variables*/
        char *name, *addr_str, *end_ptr;
        uint32_t addr;
        /*get what we want except \t*/
        addr_str = strtok(buf, "\t");
        /* create table based on these tokens */
        /*SysConvert*/
        addr = strtol(addr_str, &end_ptr, 0);
        name = strtok(NULL, "\n");
        /* add the name to table, one by one */
        add_to_table(symtbl, name, addr, 0);
    }
    return symtbl;
}