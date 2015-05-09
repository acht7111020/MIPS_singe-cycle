//
//  main.c
//  project1
//
//  Created by Mac on 2015/4/2.
//  Copyright (c) 2015年 Mac. All rights reserved.
//

//
//  main.c
//  project1_single_cycle
//
//  Created by Mac on 2015/3/15.
//  Copyright (c) 2015年 Mac. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADD 0x20
#define SUB 0x22
#define AND 0x24
#define OR 0x25
#define XOR 0x26
#define NOR 0x27
#define NAND 0x28
#define SLT 0x2A
#define SLL 0x00
#define SRL 0x02
#define SRA 0x03
#define JR 0x08

#define ADDI 0x08
#define LW 0x23
#define LH 0x21
#define LHU 0x25
#define LB 0x20
#define LBU 0x24
#define SW 0x2b
#define SH 0x29
#define SB 0x28
#define LUI 0x0f
#define ANDI 0x0c
#define ORI 0x0d
#define NORI 0x0e
#define SLTI 0x0a
#define BEQ 0x04
#define BNE 0x05

#define J 0x02
#define JAL 0x03



void ReadFile();
void LoadInMem();
void decode(int instruction_code);
void R_type(int instruction_code);
void I_type(int instruction_code);
void J_type(int instruction_code);
void ErrorDetect(int err);
//void TestError2(int in1 , int in2 , int out);
void TestError(int in1 , int in2 , int out ,int type);
int Change_endian(int c);

static int * code_i, * code_d;
static int data_i[256] , data_d[256] ,reg[32] , PC ;
static int op , rs , rt , rd , shamt , funct , imm , address ;
static int cycle_count , HaltFlag;
//int error3_re ;
int value_rs,value_imm,value_rt;
int main() {
    
    FILE * fw_ptr = fopen("snapshot.rpt", "w");
    FILE * fw_err = fopen("error_dump.rpt", "w");
    
    
    ReadFile();
    
    /* clean array first */
    int i;
    for (i = 0; i < 256 ; i++ ) {
        data_i[i] = 0;
        data_d[i] = 0;
    }
    for (i = 0; i < 32 ; i++ ) {
        reg[i] = 0;
    }
    PC = 0;
    cycle_count = 0;
    HaltFlag = 0;
    
    LoadInMem();
    
    while (1) {
        //printf("begin while.\n");
        op = rs = rt = rd = shamt = funct = imm = address = 0;
        //error3_re = 0;
        
        if (cycle_count > 500000) {
            printf("cycle counts more than 500,000 .\n");
        }
        /*
         fprinf to snapshot.rpt;
         */
        int i ;
        fprintf( fw_ptr , "cycle %d\n" , cycle_count);
        for (i = 0; i < 32; i ++) {
            fprintf( fw_ptr , "$%02d: 0x%08X\n", i , reg[i]);
        }
        fprintf( fw_ptr , "PC: 0x%08X\n\n\n" , PC );
        
        cycle_count++;
        if(PC%4 != 0 || PC > 1024 || PC < 0){
            printf("pc is wrong in cycle %d\n",cycle_count);
            exit(1);
        }
        decode(data_i[PC/4]);
        if (op == 0x3F) {
            break;
        }
        
    }
    fclose(fw_ptr);
    fclose(fw_err);
    
    return 0;
}

void ReadFile(){ //put all binary code into men and detect error
    
    FILE * fr_i = fopen("iimage.bin", "rb");
    FILE * fr_d = fopen("dimage.bin", "rb");
    
    
    if(fr_i == NULL || fr_d == NULL){
        printf("Your testcase is WRONG or can't find.\n");
        exit(2);
    }
    
    int a = fseek(fr_i, 0L , SEEK_END);
    long int filesize_i = ftell(fr_i);
    int b = fseek(fr_i, 0L, SEEK_SET);
    //  filesize_i /= 4;
    
    if(a == 0 && b == 0)  code_i= malloc(filesize_i);
    else {
        printf("fseek iimage wrong and malloc failed .\n");
        exit(3);
    }
    
    a = fseek(fr_d, 0L , SEEK_END);
    long int filesize_d = ftell(fr_d);
    b = fseek(fr_d, 0L, SEEK_SET);
    //  filesize_d /= 4;
    
    if(a == 0 && b == 0)  code_d = malloc(filesize_d);
    else {
        printf("fseek dimage wrong and malloc failed .\n");
        exit(3);
    }
    
    fread(code_i, filesize_i , 1 , fr_i);
    fread(code_d, filesize_d , 1 , fr_d);
    //printf("%ld %d\n",sizeof(code_i),filesize_i);
    //printf("filesize = %ld %ld\n",filesize_i,filesize_d);
    
    int x = 0x1234;
    char y = *(char*)&x;
    
    if(y != 0x12){ /* little endian */
        //printf("shift begin\n") ;
        int i ;
        
        code_i[1] = Change_endian(code_i[1]);
        //printf("1 = %08x\n",code_i[1]);
        
        code_i[0] = Change_endian(code_i[0]);
        //printf("0 = %08x\n",code_i[0]);
        
        for ( i = 2; i < code_i[1] + 2; i++){
            
            code_i[i] = Change_endian(code_i[i]);
            //printf("i = %d ,%08x\n",i,code_i[i]);
        }
        //printf("code_i[1] = %08x\n",code_i[1]);
        
        //printf("origainal 1 = %08x\n",code_d[1]);
        code_d[1] = Change_endian(code_d[1]);
        //printf("1 = %08x\n",code_d[1]);
        
        code_d[0] = Change_endian(code_d[0]);
        //printf("0 = %08x\n",code_d[0]);
        if (code_d[1] > 1024) {
            //printf("brokennnnnnn .\n");
            exit(1);
        }
        for ( i = 2; i < code_d[1] + 2 ; i++){
            code_d[i] = Change_endian(code_d[i]);
            //printf("i = %d ,%08x\n",i,code_d[i]);
        }
        //printf("code_d[1] = %08x\n",code_d[1]);
    }
    
    
    
    fclose(fr_i);
    fclose(fr_d);
}
int Change_endian(int c){
    
    return ((c << 24 ) & 0xff000000 ) | ((c << 8 ) & 0x00ff0000 ) |
    ((c >> 8 ) & 0x0000ff00 ) | ((c >> 24 ) & 0x000000ff) ;
    
}


void LoadInMem(){
    
    PC = code_i[0];
    reg[29] = code_d[0];
    int instr_num_i = code_i[1];
    int instr_num_d = code_d[1];
    //printf("PC = %08X, num = %08X .\n",PC ,instr_num_i);
    if ( PC + instr_num_i > 1024) {
        printf("iimage instructions more than 1K . Memory overflow .\n");
        printf("PC = %08X, num = %08X .\n",PC ,instr_num_i);
        
        exit(1);
    }
    if (instr_num_d > 1024) {
        printf("dimage instructions more than 1K . Memory overflow .\n");
        printf("$s = %08X, num = %08X .\n",reg[29] ,instr_num_d);
        exit(1);
    }
    
    int i , j;
    for (i = PC/4 , j = 2 ; j < 2 + instr_num_i ; i++, j++ )
        data_i[i] = code_i[j];
    
    for (i = 0 , j = 2 ; j < 2 + instr_num_d ; i++, j++ )
        data_d[i] = code_d[j];
    //printf("before free\n");
    /*free(code_i);
     free(code_d);*/
    
}

void decode(int instruction_code){
    op = (instruction_code >> 26) & 0x3f;
    
    if(op == 0x3F){
        exit(0);
    }
    
    PC = PC + 4 ; /*在執行這行的時候 ＰＣ已經跳到下一個 */
    
    if(op == 0x00){
        R_type(instruction_code);
    }
    else if(op == 0x02 || op == 0x03){
        J_type(instruction_code);
    }
    else{
        I_type(instruction_code);
    }
    
}

void R_type(int instruction_code){
    funct = instruction_code & 0x3f;
    shamt = (instruction_code >> 6) & 0x1f;
    rd = (instruction_code >> 11) & 0x1f;
    rt = (instruction_code >> 16) & 0x1f;
    rs = (instruction_code >> 21) & 0x1f;
    
    if (funct != ADD && funct != SUB && funct != AND &&
        funct != OR && funct != XOR && funct != NOR &&
        funct != NAND && funct != SLT && funct != SLL &&
        funct != SRL && funct != SRA && funct != JR ) {
        printf("R_type function is wrong , in Cycle %d .\n",cycle_count);
        exit(-1);
    }
    
    if (rd == 0 && funct!=JR) {
        /*|| funct == SRL || funct == SRA */
        if ( (funct == SLL  ) && rt == 0 && shamt == 0)
            ErrorDetect(0);
        else{
            ErrorDetect(1);
            //printf("cycle = %d\n",cycle_count);
            if(funct == ADD){
                TestError(reg[rs],reg[rt],reg[rs] + reg[rt],2);
            }
            else if (funct == SUB){
                TestError(reg[rs],(-reg[rt]),reg[rs] + (-reg[rt]) ,2);
                // printf("sub = %08x - %08x = %08x\n",reg[rs],(-reg[rt]),reg[rs] + (-reg[rt]));
            }
        }
    }
    
    else{
        switch (funct) {
            case ADD:
                TestError(reg[rs],reg[rt],reg[rs] + reg[rt],2);
                reg[rd] = reg[rs] + reg[rt];
                
                break;
            case SUB:
                TestError(reg[rs],(-reg[rt]),reg[rs] + (-reg[rt]),2);
                reg[rd] = reg[rs] + (-reg[rt]);
                /*  value_rs =  reg[rs] ;
                 value_rt =  reg[rt] ;
                 reg[rd] = reg[rs] - reg[rt] ;
                 // TestError2
                 int s_in1 = (value_rs >> 31) & 1 ;
                 int s_in2 = (value_rt >> 31) & 1 ;
                 int s_out = (reg[rd] >> 31) & 1 ;
                 //N-P = P OR P-N = N
                 if ((s_in1 != s_in2 ) && ( s_in2 == s_out) ) {
                 ErrorDetect(2);
                 }*/
                break;
            case AND:
                reg[rd] = reg[rs] & reg[rt];
                break;
            case OR:
                reg[rd] = reg[rs] | reg[rt];
                break;
            case XOR:
                reg[rd] = reg[rs] ^ reg[rt];
                break;
            case NOR:
                reg[rd] = ~(reg[rs] | reg[rt]);
                break;
            case NAND:
                reg[rd] = ~(reg[rs] & reg[rt]);
                break;
            case SLT:
                reg[rd] = (reg[rs] < reg[rt])?1:0;
                break;
            case SLL:
                reg[rd] = reg[rt] << shamt;
                break;
            case SRL:/* unsigned shift */
                if (shamt==0)
                    reg[rd] = reg[rt];
                else
                    reg[rd] = (reg[rt] >> shamt) & ~(0xffffffff << (32 - shamt));
                break;
            case SRA:/* signed shift */
                reg[rd] = reg[rt] >> shamt;
                break;
            case JR:
                PC = reg[rs] ;
                break;
            default:
                break;
        }
        
    }
    
}

void J_type(int instruction_code){
    
    address = instruction_code & 0x3ffffff;
    switch (op) {
        case J:
            PC = ( PC & 0xf0000000 ) | (4 * address) ;
            break;
        case JAL:
            reg[31] = PC;
            PC = ( PC & 0xf0000000 ) | (4 * address) ;
            break;
        default:
            printf("J_type opcode is wrong in cycle %d .\n",cycle_count);
            break;
    }
    
}

void I_type(int instruction_code){
    
    imm = ((instruction_code & 0xffff) << 16 ) >> 16;
    rt = (instruction_code >> 16) & 0x1f ;
    rs = (instruction_code >> 21) & 0x1f ;
    
    if (op != ADDI && op != LW && op != LH &&
        op != LHU && op != LB && op != LBU &&
        op != SW && op != SH && op != SB &&
        op != LUI && op != ANDI && op != ORI &&
        op != NORI && op != SLTI && op != BEQ && op != BNE) {
        printf("J_type function is wrong , in Cycle %d .\n",cycle_count);
        exit(-1);
    }
    
    if (rt == 0 && op != SW && op != SH && op != SB
        && op != BEQ && op != BNE) {
        ErrorDetect(1);
        if(op == LW || op == LH || op == LHU || op == LB||
           op == LBU || op == SW || op == SH || op == SB){
            TestError(reg[rs], imm , reg[rs] + imm ,2);
            TestError(reg[rs], imm , 0 ,3);
            if(op == LW || op == SW){
                if ( (reg[rs] + imm) % 4 != 0)
                    ErrorDetect(4);
            }
            else if (op == LH || op == SH || op	== LHU){
                if ( (reg[rs] + imm) % 2 != 0)
                    ErrorDetect(4);
            }
            if (HaltFlag==1)
                exit(1);
        }
        else if(op == ADDI)TestError(reg[rs], imm , reg[rs] + imm ,2);
    }
    else {
        switch ( op ) {
            case ADDI :
                value_rs =  reg[rs] ;
                value_imm =  imm;
                reg[rt] = reg[rs] + imm;
                TestError(value_rs, value_imm, reg[rt], 2);
                break;
            case LW :
                TestError(reg[rs], imm , reg[rs] + imm ,2);
                TestError(reg[rs], imm , 0 ,3);
                if ( (reg[rs] + imm) % 4 != 0)
                    ErrorDetect(4);
                if (HaltFlag==1)
                    exit(1);
                reg[rt] = data_d[ (reg[rs] + imm) / 4 ];
                break;
            case LH : /*load 整個word的一半 所以還是 /4 */
                TestError(reg[rs], imm , reg[rs] + imm ,2);
                TestError(reg[rs], imm , 0 ,3);
                if ( (reg[rs] + imm) % 2 != 0)
                    ErrorDetect(4);
                if (HaltFlag==1)
                    exit(1);
                if ((reg[rs] + imm) % 4 == 0 )
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ]  >> 16;
                else
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ] << 16 >> 16 ;
                break;
            case LHU :
                TestError(reg[rs], imm , reg[rs] + imm ,2);
                TestError(reg[rs], imm , 0 ,3);
                if ( (reg[rs] + imm) % 2 != 0)
                    ErrorDetect(4);
                if (HaltFlag==1)
                    exit(1);
                
                if ((reg[rs] + imm) % 4 == 0 )
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ]  >> 16 & 0x0000ffff;
                else
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ] << 16 >> 16 & 0x0000ffff;
                break;
                
            case LB :
                TestError(reg[rs], imm , reg[rs] + imm ,2);
                TestError(reg[rs], imm , 0 ,3);
                if (HaltFlag==1)
                    exit(1);
                
                if ((reg[rs] + imm) %4 == 0)
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ]  >> 24;
                else if ((reg[rs] + imm) %4 == 1)
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ] << 8 >> 24;
                else if ((reg[rs] + imm) %4 == 2)
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ] << 16 >> 24;
                else
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ] << 24 >> 24 ;
                break;
            case LBU :
                TestError(reg[rs], imm , reg[rs] + imm ,2);
                TestError(reg[rs], imm , 0 ,3);
                if (HaltFlag==1)
                    exit(1);
                
                if ((reg[rs] + imm) %4 == 0)
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ]  >> 24 & 0x000000ff;
                else if ((reg[rs] + imm) %4 == 1)
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ] << 8 >> 24 & 0x000000ff;
                else if ((reg[rs] + imm) %4 == 2)
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ] << 16 >> 24 & 0x000000ff;
                else
                    reg[rt] = data_d[ (reg[rs] + imm) / 4 ] << 24 >> 24 & 0x000000ff;
                break;
            case SW :
                TestError(reg[rs], imm , reg[rs] + imm ,2);
                TestError(reg[rs], imm , 0 ,3);
                if ( (reg[rs] + imm) % 4 != 0 )
                    ErrorDetect(4);
                if (HaltFlag==1)
                    exit(1);
                
                data_d[ (reg[rs] + imm) / 4 ] = reg[rt];
                break;
            case SH :
                TestError(reg[rs], imm , reg[rs] + imm ,2);
                TestError(reg[rs], imm , 0 ,3);
                if ( (reg[rs] + imm) % 2 != 0)
                    ErrorDetect(4);
                if (HaltFlag==1)
                    exit(1);
                
                if ((reg[rs] + imm) % 4 == 0 ){
                    data_d[ (reg[rs] + imm) / 4 ] &= 0x0000ffff ;
                    data_d[ (reg[rs] + imm) / 4 ] |= ((reg[rt] & 0x0000ffff)<< 16 );
                }
                else{
                    data_d[ (reg[rs] + imm) / 4 ] &= 0xffff0000 ;
                    data_d[ (reg[rs] + imm) / 4 ] |= (reg[rt] & 0x0000ffff);
                }
                
                break;
            case SB :
                TestError(reg[rs], imm , reg[rs] + imm ,2);
                TestError(reg[rs], imm , 0 ,3);
                if (HaltFlag==1)
                    exit(1);
                
                if ((reg[rs] + imm) %4 == 0){
                    data_d[ (reg[rs] + imm) / 4 ] &= 0x00ffffff ;
                    data_d[ (reg[rs] + imm) / 4 ] |= ( (reg[rt] & 0x000000ff) << 24 );
                }
                else if ((reg[rs] + imm) %4 == 1){
                    data_d[ (reg[rs] + imm) / 4 ] &= 0xff00ffff ;
                    data_d[ (reg[rs] + imm) / 4 ] |= ( (reg[rt] & 0x000000ff) << 16 );
                }
                else if ((reg[rs] + imm) %4 == 2){
                    data_d[ (reg[rs] + imm) / 4 ] &= 0xffff00ff ;
                    data_d[ (reg[rs] + imm) / 4 ] |= ( (reg[rt] & 0x000000ff)  << 8 );
                }
                else{
                    data_d[ (reg[rs] + imm) / 4 ] &= 0xffffff00 ;
                    data_d[ (reg[rs] + imm) / 4 ] |=  (reg[rt] & 0x000000ff)  ;
                }
                break;
            case LUI :
                reg[rt] = imm << 16;
                break;
            case ANDI :
                reg[rt] = reg[rs] & ( imm & 0x0000ffff );
                break;
            case ORI :
                reg[rt] = reg[rs] | (imm & 0x0000ffff) ;
                break;
            case NORI :
                reg[rt] = ~(reg[rs] | (imm & 0x0000ffff));
                break;
            case SLTI :
                reg[rt] = (reg[rs] < imm)? 1 : 0 ;
                break;
            case BEQ :
                if (reg[rs] == reg [rt]) {
                    PC += imm * 4 ;
                }
                break;
            case BNE :
                if (reg[rs] != reg [rt]) {
                    PC += imm * 4 ;
                }
                break;
                
            default:
                break;
        }
        
    }
    
    
}

/*void TestError2(int in1 , int in2 , int out){ // number overflow
 int s_in1 = (in1 >> 31) & 1 ;
 int s_in2 = (in2 >> 31) & 1 ;
 int s_out = (out >> 31) & 1 ;
 
 if ((s_in1 == s_in2 ) && ( s_out != s_in1 )) {
 ErrorDetect(2);
 }
 }*/

void TestError(int in1 , int in2 , int out ,int type ){
    int s_in1, s_in2 , s_out;
    
    switch (type) {
        case 2:/* number overflow */
            s_in1 = (in1 >> 31) & 1 ;
            s_in2 = (in2 >> 31) & 1 ;
            s_out = (out >> 31) & 1 ;
            //  printf("singned = %d %d %d in cycle = %d\n",s_in1,s_in2,s_out,cycle_count);
            //  printf("num = %8x %8x %8x in cycle = %d\n",in1,in2,out,cycle_count);
            if ( (s_in1 == s_in2 ) && ( s_out != s_in1 ) ) {
                ErrorDetect(2);
               /* if(op == LW || op == LH || op == LHU || op == LB
                   || op == LBU ||op == SB || op == SH || op == SW ){
                    ErrorDetect(3); //if overflow then address overflow , toooooo
                    error3_re = 1;
                }*/
            }
            
            break;
        case 3:/* D Memory Address overflow  --   lw, lh, lhu, lb,
                lbu, sw, sh, sb, */
            if( (in1 + in2) < 0 || (in1 + in2 )> 1023){
                //if(error3_re == 1){break;}
                ErrorDetect(3);
                
            }
            else if( (op == SW || op == LW) && ((in1 + in2 ) > 1020) ) ErrorDetect(3);
            else if( (op == SH || op == LH || op == LHU) && ((in1 + in2 ) > 1022) ){
                ErrorDetect(3);
            }
            break;
        default:
            break;
    }
}




void ErrorDetect(int err){
    FILE * fw_err = fopen("error_dump.rpt", "a");
    switch (err) {
        case 1:
            fprintf( fw_err, "In cycle %d: Write $0 Error\n", cycle_count);
            break;
        case 2:
            fprintf( fw_err , "In cycle %d: Number Overflow\n", cycle_count);
            break;
        case 3:
            fprintf( fw_err , "In cycle %d: Address Overflow\n", cycle_count);
            HaltFlag = 1;
            break;
        case 4:
            fprintf( fw_err , "In cycle %d: Misalignment Error\n", cycle_count);
            HaltFlag = 1;
            break;
        default:
            break;
    }
    
    fclose(fw_err);
    
    /* if (stop == 1) {
     exit(1);
     } */
    
    
    
}

