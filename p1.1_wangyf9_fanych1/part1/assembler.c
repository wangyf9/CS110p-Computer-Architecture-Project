/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia 
   to be part of the project in Feb 2023.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*include*/
#include "assembler.h"
#include "tables.h"
#include "translate.h"
#include "translate_utils.h"
/*include*/
#define MAX_ARGS 3
#define BUF_SIZE 1024
const char *IGNORE_CHARS = " \f\n\r\t\v,()";

/*******************************
 * Helper Functions
 *******************************/

/* you should not be calling this function yourself. */
static void raise_label_error(uint32_t input_line, const char *label)
{
    write_to_log("Error - invalid label at line %d: %s\n", input_line, label);
}

/* call this function if more than MAX_ARGS arguments are found while parsing
   arguments.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.

   EXTRA_ARG should contain the first extra argument encountered.
 */
static void raise_extra_argument_error(uint32_t input_line,
                                       const char *extra_arg)
{
    write_to_log("Error - extra argument at line %d: %s\n", input_line,
                 extra_arg);
}

/* You should call this function if write_original_code() or translate_inst()
   returns 0.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.
 */
static void raise_instruction_error(uint32_t input_line, const char *name,
                                    char **args, int num_args)
{

    write_to_log("Error - invalid instruction at line %d: ", input_line);
    log_inst(name, args, num_args);
}

/* Truncates the string at the first occurrence of the '#' character. */
static void skip_comments(char *str)
{
    char *comment_start = strchr(str, '#');
    if (comment_start)
    {
        *comment_start = '\0';
    }
}

/* Reads STR and determines whether it is a label (ends in ':'), and if so,
   whether it is a valid label, and then tries to add it to the symbol table.

   INPUT_LINE is which line of the input file we are currently processing. Note
   that the first line is line 1 and that empty lines are included in this
   count.

   BYTE_OFFSET is the offset of the NEXT instruction (should it exist).

   Four scenarios can happen:
    1. STR is not a label (does not end in ':'). Returns 0.
    2. STR ends in ':', but is not a valid label. Returns -1.
    3a. STR ends in ':' and is a valid label. Addition to symbol table fails.
        Returns -1.
    3b. STR ends in ':' and is a valid label. Addition to symbol table succeeds.
        Returns 1.
 */
static int add_if_label(uint32_t input_line, char *str, uint32_t byte_offset,
                        SymbolTable *symtbl, int check_align)
{
    size_t len = strlen(str);
    if (str[len - 1] == ':')
    {
        /* change the last to '\0' */
        str[len - 1] = '\0';
        if (is_valid_label(str))
        {
            /* add to table and return number of tokens, otherwise return -1 */
            if (add_to_table(symtbl, str, byte_offset, check_align) == 0)
            {   /*succeed*/
                return 1;
            }
            else
            {   /*fail*/
                return -1;
            }
        }
        else
        {
            /* we raise error if no valid label is found */
            raise_label_error(input_line, str);
            return -1;
        }
    }
    else
    {   /* STR is not a label */
        return 0;
    }
}

/*******************************
 * Implement the Following
 *******************************/

/* First pass of the assembler. You should implement pass_two() first.

   This function should read each line, strip all comments, scan for labels,
   and pass instructions to write_original_code(), pass data to write_static_data(). 
   The symbol table and data segment should also been built and written to 
   specified file. The input file may or may not be valid. Here are some guidelines:

   1. Only one of ".data" or ".text" may be present per line. It must be the 
      first token present. Once you see a ".data" or ".text", the rest part
      will be treat as data or text segment until the next ".data" or ".text"
      Appears. Ignore the rest token of this line. Default to text segment 
      if not indicated.
    
   2. For data segment, we only considering ".word", ".half", ".Byte" types.
      These types must be the first token of a line. The rest of the tokens 
      in this line will be stored as variables of that type in the data segment.
      You can assume that these numbers are legal with their types.

   3. Only one label may be present per line. It must be the first token
      present. Once you see a label, regardless of whether it is a valid label
      or invalid label, You can treat the NEXT token in this line as the 
      beginning of an instruction or a static data.

   4. If the first token is not a label, treat it as the name of an
      instruction. DO NOT try to check it is a valid instruction in this pass.
   
   5. Everything after the instruction name in the same line should be treated 
      as arguments to that instruction. If there are more than MAX_ARGS 
      arguments, call raise_extra_argument_error() and pass in the first extra 
      argument. Do not write that instruction to the original file (eg. don't call
      write_original_code())
   
   6. Only one instruction should be present per line. You do not need to do
       anything extra to detect this - it should be handled by guideline 5.
   
   8. A line containing only a label is valid. The address of the label should
      be the address of the next instruction or static data regardless of 
      whether there is a next instruction or static data or not.
   
   9. If an instruction contains an immediate, you should output it AS IS.
   
   10. Comments should always be skipped before any further process.

   11. Note that the starting address of the text segment is defined as TEXT_BASE
   in "../types.", and the starting address of the data segment is defined as 
   DATA_BASE in "../types.".

   12. Think about how you should use write_original_code() and write_static_data().
   
   Just like in pass_two(), if the function encounters an error it should NOT
   exit, but process the entire file and return -1. If no errors were
   encountered, it should return 0.
 */

int pass_one(FILE *input, FILE *original, FILE *symtbl, FILE *data) {
    char str[2048];/*to store each line*/
    int num_lines = 0;/*line number*/
    uint32_t i, j;/*index*/
    char* token;
    Byte buffer[2048];/*to store data*/
    uint32_t buf_len = 0;/*length of buffer*/
    uint32_t byte_offset_data = 0;/* address offset of data */
    uint32_t byte_offset_text = 0;/* address offset of text */
    int inst_flag = 0;/*whether read instruction or not*/
    char *args[2048];/*to store argument*/
    /*char tmp_args[8];*//*store args[i] temporarily*/
    uint32_t index;/*args的下标*/
    char *inst;/* the instruction in the original */
    uint32_t inst_num = 0;/* the number of written instruction */
    uint32_t addr;/*address*/
    int flag = 0;/*whether there exists errors*/
    uint32_t num_errors = 0;/*error number(no use?)*/
    int data_flag = 0;/*whether in data_time*/
    int text_flag = 0;/*whether in text time*/
    SymbolTable *symboltable = create_table(SYMBOLTBL_UNIQUE_NAME);
    uint32_t tmp_num;
    long ret;
    char *stop;
    int label_flag = 0;/*whether have the same label*/
    /* check if NULL */
    if(!input){
        return -1;
    }
    /* initialize the buffer to 0*/
    memset(buffer, 0 ,sizeof(Byte)*2048);
    /* default to be text segment */
    text_flag = 1;
    data_flag = 0;
    byte_offset_text = 0;
    byte_offset_data = 0;
    /* read each line to str[] */
    while(fgets(str,sizeof(str),input) != NULL){
        /* add the number of line */
        num_lines++;
        /* skip all the comments */
        skip_comments(str);
        /* ignore the empty line (only with \n)*/
        if(strlen(str) == 1){
            continue;
        }
        /* read the first token */
        token = strtok(str, IGNORE_CHARS);
        /* if skip comment , token == NULL */
        if(token == NULL){
            continue;
        }
        
        /* meet .data ,initialize the flags and offset */
        if(strcmp(token, ".data") == 0){
            data_flag = 1;
            text_flag = 0;
        }
        /* meet .text ,initialize the flags and offset */
        else if(strcmp(token, ".text") == 0){
            text_flag = 1;
            data_flag = 0;
        }
        /* meet .data */
        if(data_flag == 1 || (strlen(token) >= 5 && strcmp(token, ".data") == 0)){
            
            data_flag = 1;
            text_flag = 0;/* initialize */
                
                if(token == NULL){
                    continue;
                }
                /* meet .word */
                if(strcmp(token, ".word") == 0){
                    if(label_flag == 1){
                        continue;
                    }
                    inst = token;
                    index = 0;
                    token = strtok(NULL, IGNORE_CHARS);
                    while(token != NULL){
                        /* treat as instruction,consider MAX_ARGS*/
                        /*
                        if(index >= MAX_ARGS){
                            flag = 1;
                            num_errors++;
                            raise_extra_argument_error(num_lines, token);
                            break;
                        }*/
                        /* token = 0x-------- */
                        args[index] = token;
                        index++;
                        token = strtok(NULL, IGNORE_CHARS);
                    }
                    /* copy into buffer from args[][]*/
                    for(i = 0;i<index;i++){
                        ret = strtol(args[i], &stop, 0);
                        for(j = 0;j < 4;j++){
                            buffer[buf_len] = (Byte)(ret & 0xff);
                            /*tmp_args[j-2] = args[i][j];*/
                            /* the length of buf */
                            ret = ret >> 8;
                            buf_len++;
                        }
                        /* byte_offset + 4*/
                        byte_offset_data = byte_offset_data + 4;
                    }
                    index = 0;
                } 
                /* meet .half */
                else if(strcmp(token, ".half") == 0){
                    if(label_flag == 1){
                        continue;
                    }
                    inst = token;
                    index = 0;
                    token = strtok(NULL, IGNORE_CHARS);
                    while(token != NULL){
                        /* treat as instruction,consider MAX_ARGS*/
                        /*
                        if(index >= MAX_ARGS){
                            flag = 1;
                            num_errors++;
                            raise_extra_argument_error(num_lines, token);
                            break;
                        }*/
                        /* token = 0x---- */
                        args[index] = token;
                        index++;
                        token = strtok(NULL, IGNORE_CHARS);
                    }
                    /* copy into buffer from args[][]*/
                    for(i = 0;i<index;i++){
                        ret = strtol(args[i], &stop, 0);
                        for(j = 0;j < 2;j++){
                            buffer[buf_len] = (Byte)(ret & 0xff);
                            /*tmp_args[j-2] = args[i][j];*/
                            /* the length of buf */
                            ret = ret >> 8;
                            buf_len++;
                        }
                        /* byte_offset + 2*/
                        byte_offset_data = byte_offset_data + 2;
                    }
                    index = 0;


                } 
                /* meet .byte */
                else if(strcmp(token, ".byte") == 0){
                    if(label_flag == 1){
                        continue;
                    }
                    inst = token;
                    index = 0;
                    token = strtok(NULL, IGNORE_CHARS);
                    while(token != NULL){
                        /* treat as instruction,consider MAX_ARGS*/
                        /*
                        if(index >= MAX_ARGS){
                            flag = 1;
                            num_errors++;
                            raise_extra_argument_error(num_lines, token);
                            break;
                        }*/
                        /* token = 0x-- */
                        args[index] = token;
                        index++;
                        token = strtok(NULL, IGNORE_CHARS);
                    }
                    /* copy into buffer from args[][]*/
                    for(i = 0;i<index;i++){
                        ret = strtol(args[i], &stop, 0);
                        buffer[buf_len] = ret;
                            /*tmp_args[j-2] = args[i][j];*/
                            /* the length of buf */
                        buf_len++;
                        
                        /* byte_offset + 1*/
                        byte_offset_data = byte_offset_data + 1;
                    }
                    index = 0;
                }
                /* meet label */
                else if(token[strlen(token)-1] == ':'){
                    label_flag = 0;
                    /* calculate address */
                    addr = DATA_BASE + byte_offset_data;
                    /* add to symboltable *//*what is the checkalign*/
                    if(add_if_label(num_lines, token, addr, symboltable, 0) == -1){
                        label_flag = 1;
                    }
                    token = strtok(NULL, IGNORE_CHARS);
                    /* do while token != NULL */
                    if(token != NULL && label_flag == 0){
                        inst = token;
                        index = 0;
                        token = strtok(NULL, IGNORE_CHARS);
                        while(token != NULL){
                            /* treat as instruction,consider MAX_ARGS*/
                            /*
                            if(index >= MAX_ARGS){
                                flag = 1;
                                num_errors++;
                                raise_extra_argument_error(num_lines, token);
                                break;
                            }*/
                            /* token = 0x---- */
                            args[index] = token;
                            index++;
                            token = strtok(NULL, IGNORE_CHARS);
                        }
                        if(strcmp(inst, ".word") == 0){
                            /* copy into buffer from args[][]*/
                            for(i = 0;i<index;i++){
                                ret = strtol(args[i], &stop, 0);
                                for(j = 0;j < 4;j++){
                                    buffer[buf_len] = (Byte)(ret & 0xff);
                                    /*tmp_args[j-2] = args[i][j];*/
                                    /* the length of buf */
                                    ret = ret >> 8;
                                    buf_len++;
                                }
                                /* byte_offset + 4*/
                                byte_offset_data = byte_offset_data + 4;
                            }
                        }
                        else if(strcmp(inst, ".half") == 0){
                            /* copy into buffer from args[][]*/
                            for(i = 0;i<index;i++){
                                ret = strtol(args[i], &stop, 0);
                                for(j = 0;j < 2;j++){
                                    buffer[buf_len] = (Byte)(ret & 0xff);
                                    /*tmp_args[j-2] = args[i][j];*/
                                    /* the length of buf */
                                    ret = ret >> 8;
                                    buf_len++;
                                }
                                /* byte_offset + 2*/
                                byte_offset_data = byte_offset_data + 2;
                            }
                        }
                        else if(strcmp(inst, ".byte") == 0){
                            for(i = 0;i<index;i++){
                                ret = strtol(args[i], &stop, 0);
                                buffer[buf_len] = ret;
                                    /*tmp_args[j-2] = args[i][j];*/
                                    /* the length of buf */
                                buf_len++;
                                
                                /* byte_offset + 1*/
                                byte_offset_data = byte_offset_data + 1;
                            }
                        }
                        index = 0;
                    }
                    /* if NULL , read next line */
                    if(token == NULL || label_flag == 1)continue;
                }
                /*puts("========data_time=========");*/
                
            }
        else if(text_flag == 1 || (strlen(token) >= 5 && strcmp(token, ".text") == 0)){
            /* initialize */
            text_flag = 1;
            data_flag = 0;
            /* run until NULL */
            while(token != NULL){
                /* meet label */
                if(token[strlen(token)-1] == ':'){
                    label_flag = 0;
                    /* calculate address */
                    addr = TEXT_BASE + byte_offset_text;
                    /* add to symboltable *//*what is the checkalign*/
                    if(add_if_label(num_lines, token, addr, symboltable, 1) == -1){
                        label_flag = 1;
                    }
                    token = strtok(NULL, IGNORE_CHARS);
                    index = 0;
                    while(token != NULL ){
                        /*treat as instruction*/
                        if(inst_flag != 1){
                            inst = token;
                            /*change inst_flag*/
                            inst_flag = 1;
                        }
                        else{
                            /* raise error when index >= MAX_ARGS */
                            if(index >= MAX_ARGS){
                                flag = 1;
                                num_errors ++;
                                raise_extra_argument_error(num_lines, token);
                                break;
                            }
                            /* store the arguments into args[] */
                            args[index] = token;
                            /* num_args ++ */
                            index++;
                        }
                        token = strtok(NULL, IGNORE_CHARS);
                    }
                    
                }
                else{
                    index = 0;
                    while(token != NULL){
                        /* meet .text or meet label */
                        if(strcmp(token, ".text") == 0 || token[strlen(token)-1]==':'){
                            token = strtok(NULL, IGNORE_CHARS);
                            continue;
                        }
                        /* get instruction */
                        if(inst_flag != 1){
                            inst = token;
                            inst_flag = 1;
                        }
                        else{
                            /* raise error when index >= MAX_ARGS */
                            if(index >= MAX_ARGS){
                                flag = 1;
                                num_errors ++;
                                raise_extra_argument_error(num_lines, token);
                                break;
                            }
                            /* store the arguments into args[] */
                            args[index] = token;
                            index++;
                        }
                        token = strtok(NULL, IGNORE_CHARS);
                    }
                }
                /* do when no error */
                if(flag == 0 && inst_flag == 1 && label_flag == 0){
                    /* store the number of instructions written */
                    inst_num = write_original_code(original, inst, args, index);
                    if (inst_num == 0){
                        raise_instruction_error(num_lines, inst, args, index);
                    }
                    /* offset = 4 for each line */
                    byte_offset_text += 4*inst_num;
                }
                inst_flag = 0;
                
            } 
            /*puts("========text_time=========");*/
        }
       
    }
    /* Writes the SymbolTable TABLE to OUTPUT */
    write_table(symboltable, symtbl);
    /* calculate the number of calling write_static_number */
    tmp_num = buf_len / 4;
    if((buf_len % 4) != 0){
        tmp_num = tmp_num + 1;
    }
    /* call write_static_data */
    for(i = 0;i < tmp_num;i++){
        write_static_data(data, buffer);
        for(j = 0;j < tmp_num;j++){
            /* left move */
            memcpy(buffer + (j*4), buffer + (4*(j+1)), 4);
        }
    }
    free_table(symboltable);
    /*check errors*/
    if(num_errors == 0){
        return 0;
    }
    else{
        return -1;
    }
    /* YOUR CODE HERE */
}

/* Second pass of the assembler.

   This function should read an original file and the corresponding symbol table
   file, translates it into basic code and machine code. You may assume:
    1. The input file contains no comments
    2. The input file contains no labels
    3. The input file contains at maximum one instruction per line
    4. All instructions have at maximum MAX_ARGS arguments
    5. The symbol table file is well formatted
    6. The symbol table file contains all the symbol required for translation
    7. The line may contain only one "-" which is placeholder you can ignore.
   If an error is reached, DO NOT EXIT the function. Keep translating the rest of
   the document, and at the end, return -1. Return 0 if no errors were encountered. */


int pass_two(FILE *original, FILE *symtbl, FILE *basic, FILE *machine) {
    int flag = 0;/*whether there exists errors*/
    char *args[3];
    char *token;
    char *inst;/*指令存在哪啊*//* the instruction in the original */
    int inst_flag = 0;
    char str[2014];
    int num_lines = 0;
    int index;/*args的下标*/
    uint32_t addr = 0;
    SymbolTable *symboltable = create_table_from_file(SYMBOLTBL_UNIQUE_NAME, symtbl);/* what is the mode */
    /* check if NULL */
    if(!original || !symtbl){
        flag = 1;
    }
    /* read lines */
    while(fgets(str, 1024, original) != NULL){
        /* count line number */
        num_lines++;
        /* when meeting '-',ignore*/
        token = strtok(str, IGNORE_CHARS);
        if(token == NULL){
            continue;
        }
        /* meet '-' */
        if(strcmp(token, "-") == 0){
            addr += 4;
            continue;
        }
        index = 0;
        /* do while there are contents */
        while(token != NULL){
            /* read instruction */
            if(inst_flag != 1){
                inst = token;
                inst_flag = 1;
            }
            else{
                /* check the index */
                if(index >= MAX_ARGS){
                    flag = 1;
                    break;
                }
                /* store the arguments into args[] */
                args[index] = token;
                index++;
            }
            token = strtok(NULL, IGNORE_CHARS);
        }
        inst_flag = 0;
        if(flag != 1){
            
            /* meet pseudo-instruction */
            if(strcmp(inst, "jr")==0 || strcmp(inst, "beqz")==0 || strcmp(inst, "bnez")==0 || strcmp(inst, "j")==0 || strcmp(inst, "la")==0 || strcmp(inst, "li")==0 || strcmp(inst, "lw")==0 || strcmp(inst, "mv")==0 || strcmp(inst, "jal")==0 || strcmp(inst, "jalr")==0){/* 改 */
                if(translate_inst(basic, machine, inst, args, index, addr, symboltable) == 0){
                    /* raise error */
                    raise_instruction_error(num_lines, inst, args, index);
                }
            }
            /* meet instruction */
            else{
                write_instruction(basic, machine, inst, args, index, addr, symboltable);
                /*if( == -1){*/
                    /* raise error */
                    /*raise_instruction_error(num_lines, inst, args, index);
                }*/
            }
            
        }
        addr += 4;
        /*addr = 0;*/
    }
    free_table(symboltable);
    /*check errors*/
    if(flag == 0){
        return 0;
    }
    else{
        return -1;
    }
    /* YOUR CODE HERE */
}


/*******************************
 * Do Not Modify Code Below
 *******************************/


/* Runs the two-pass assembler. Most of the actual work is done in pass_one()
   and pass_two().
 */
int assemble(int mode, FILE *input, FILE *data, FILE *symtbl, FILE *orgin, FILE *basic, FILE *text)
{
    int err = 0;

    /* Execute pass one if mode two is not specified */
    if (mode != 2)
    {
        rewind(input);
        /* run pass one */
        if (pass_one(input, orgin, symtbl, data) != 0)
        {
            err = 1;
        }
    }

    /* Execute pass two if mode one is not specified */
    if (mode != 1)
    {
        rewind(orgin);
        rewind(symtbl);
        /* run pass two */
        if (pass_two(orgin, symtbl, basic, text) != 0)
        {
            err = 1;
        }
    }

    /* Error handling */
    if (err) {
        write_to_log("One or more errors encountered during assembly operation.\n");
    } else {
        write_to_log("Assembly operation completed successfully!\n");
    }

    return err;
}