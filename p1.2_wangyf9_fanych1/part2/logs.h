#ifndef LOGS_H
#define LOGS_H

#include "emulator.h"
#include "mem.h"
#include <stdio.h>

/* Complete the following definition of Logs and implement the following
   functions. You are free to add structs or functions, but you must keep
   structure Logs. And you cannot change a function that has already been given.
 */

 /* Add structure definition here if you need */
typedef struct
{
  Register PC;/*program counter*/
  Register reg[32];/*32 registers*/
  int status;/*status*/
  Word Value;
  Instruction inst;/*instruction*/
} Logsnode;


typedef struct
{
  Logsnode *nodes;/*node*/
  int size;/*number of node*/
  int cap;/*capacity*/
    /* YOUR CODE HERE */
} Logs;




/* IMPLEMENT ME - see documentation in logs.c */
Logs *create_logs();

/* IMPLEMENT ME - see documentation in logs.c */
void free_logs(Logs *logs);

/* IMPLEMENT ME - see documentation in logs.c */
void record(Instruction inst, Processor *cpu, Byte *mem, Logs *logs);

/* IMPLEMENT ME - see documentation in logs.c */
int undo(Processor *cpu, Byte *mem, Logs *logs);

/* Add function declaration here if you need */

#endif