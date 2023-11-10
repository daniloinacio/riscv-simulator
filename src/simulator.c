/*Simulador de instruções do RISCV RV32I
compilador: gcc
sistema operacional: Ubuntu 16.04
IDE: Atom
*/
#include "simulator.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//memoria
int32_t mem[MEM_SIZE];

//banco de registradores
int32_t breg[REG_SIZE];

//registradores adicionais
uint32_t pc;
uint32_t ri;

//chamada para sair
int CALLEXIT = FALSE;

//campos das instruções
uint8_t opcode;
uint32_t rs1;
uint32_t rs2;
uint32_t rd;
uint32_t shamt;
uint8_t funct3;
uint8_t funct7;
int32_t imm12_i;
int32_t imm12_s;
int32_t imm13;
int32_t imm20_u;
int32_t imm21;

//inicializa os registradores
void set_registers(){

  for(int i=0; i< REG_SIZE; i++){
    breg[i] = 0x00000000;
  }
  breg[sp] = 0x00003ffc;
  breg[gp] = 0x00001800;
  pc = 0x00000000;
  ri = 0x00000000;
}

//imprime os valores dos registradores em hexa ou em decimal
void dump_reg(char format){
  if((format != 'h') && (format != 'd')){
    printf("Erro, format incorrect\n");
    return;
  }

  printf("\n----------REGISTERS-----------\n\n" );

  if(format == 'h'){
    printf("pc  : 0x%08x\n", pc);
    printf("ri  : 0x%08x\n", ri);
  }
  else{
    printf("pc  : %010d\n", pc);
    printf("ri  : %010d\n", ri);
  }

  for(int reg=0; reg<REG_SIZE; reg++){
    switch (reg) {
      case zero: printf("zero: "); break;
      case ra  : printf("ra  : ");break;
      case sp  : printf("sp  : ");break;
      case gp  : printf("gp  : ");break;
      case tp  : printf("tp  : ");break;
      case t0  : printf("t0  : ");break;
      case t1  : printf("t1  : ");break;
      case t2  : printf("t2  : ");break;
      case s0  : printf("s0  : ");break;
      case s1  : printf("s1  : ");break;
      case a0  : printf("a0  : ");break;
      case a1  : printf("a1  : ");break;
      case a2  : printf("a2  : ");break;
      case a3  : printf("a3  : ");break;
      case a4  : printf("a4  : ");break;
      case a5  : printf("a5  : ");break;
      case a6  : printf("a6  : ");break;
      case a7  : printf("a7  : ");break;
      case s2  : printf("s2  : ");break;
      case s3  : printf("s3  : ");break;
      case s4  : printf("s4  : ");break;
      case s5  : printf("s5  : ");break;
      case s6  : printf("s6  : ");break;
      case s7  : printf("s7  : ");break;
      case s8  : printf("s8  : ");break;
      case s9  : printf("s9  : ");break;
      case s10 : printf("s10 : ");break;
      case s11 : printf("s11 : ");break;
      case t3  : printf("t3  : ");break;
      case t4  : printf("t4  : ");break;
      case t5  : printf("t5  : ");break;
      case t6  : printf("t6  : ");break;
    }
    if(format == 'h'){
      printf("0x%08x\n", breg[reg]);
    }
    else{
      printf("%010d\n", breg[reg]);
    }

  }

  printf("\n----------------------------------------------------\n");

  return;
}

//imprime palavras de 32 bits da memoria em hexa ou decimal
void dump_mem(int start, int end, char format){
  uint32_t windex;
  if(((start % 4) != 0) || ((end % 4) != 0)){
    printf("Erro, address not aligned to word boundary\n");
    return;
  }

  printf("\n----------MEMORY-----------\n\n" );
  for(windex = 0 ; windex < ((end - start) >> 2); windex++){
    if((windex%2 == 0) && (windex != 0)){
      printf("\n");
    }

    printf("mem[0x%08x] = ", start + (windex*4));
    if(format == 'h'){

      printf("0x%08x  ", mem[(start >> 2) + windex]);
    }
    else if(format == 'd'){
      printf("%010d  ", mem[(start >> 2) + windex]);
    }
    else{
      printf("Erro, format incorrect\n");
      return;
    }
  }
  printf("\n----------------------------------------------------\n");
  return;
}

//lê um inteiro alinhado - endereços múltiplos
int32_t lw(uint32_t address, int32_t kte){
  if((address + kte) % 4 != 0){
    printf("Erro, address not aligned to word boundary\n");
    return 0;
  }
  return mem[(address + kte) >> 2];
}

//lê meia palavra, 16 bits - retorna inteiro com sinal
int32_t lh(uint32_t address, int32_t kte){
  int16_t hword;
  if((((address + kte) % 4) % 2) != 0){
    printf("Erro, address not aligned on halfword boundary\n");
    return 0;
  }
  hword = ((mem[(address + kte) >> 2] >> (((address + kte) % 4) * 8)) & hwmask);
  return hword;
}

//lê meia palavra, 16 bits - retorna inteiro sem sinal
uint32_t lhu(uint32_t address, int32_t kte){
  uint16_t hword;
  if((((address + kte) % 4) % 2) != 0){
    printf("Erro, address not aligned on halfword boundary\n");
    return 0;
  }
  hword = ((mem[(address + kte) >> 2] >> (((address + kte) % 4) * 8)) & hwmask);
  return hword;
}

//lê um byte - retorna inteiro com sinal
int32_t  lb(uint32_t address, int32_t kte){
  int8_t byte;
  byte = ((mem[(address + kte) >> 2] >> (((address + kte) % 4) * 8)) & bytemask);
  return byte;
}

//lê um byte - retorna inteiro sem sinal
uint32_t  lbu(uint32_t address, int32_t kte){
  uint8_t byte;
  byte = ((mem[(address + kte) >> 2] >> (((address + kte) % 4) * 8)) & bytemask);
  return byte;
}

//escreve um inteiro alinhado na memória - endereços múltiplos de 4
void  sw(uint32_t address, int32_t kte, int32_t dado){
  if((address + kte) % 4 != 0){
    printf("Erro, address not aligned to word boundary\n");
    return;
  }
  mem[(address + kte) >> 2] = dado;
  return;
}

//escreve meia palavra, 16 bits - endereços múltiplos de 2
void  sh(uint32_t address, int32_t kte, int16_t dado){
  uint32_t temp;
  if((((address + kte) % 4) % 2) != 0){
    printf("Erro, address not aligned on halfword boundary\n");
    return;
  }
  mem[(address + kte) >> 2] &= ~(hwmask << (((address + kte) % 4) * 8));
  temp = (dado & hwmask) << (((address + kte) % 4) * 8);
  mem[(address + kte) >> 2] |= temp;
  return;
}

//escreve um byte na memória
void  sb(uint32_t address, int32_t kte, int8_t dado){
  uint32_t temp;
  mem[(address + kte) >> 2] &= ~( bytemask << (((address + kte) % 4) * 8));
  temp = (dado & bytemask) << (((address + kte) % 4) * 8);
  mem[(address + kte) >> 2] |= temp;
  return;
}

void fetch(){
  ri = mem[pc >> 2];
  pc += 4;
  return;
}

void decode(){
  opcode = ri & bitsmask7;
  rd = (ri >> 7) & bitsmask5;
  funct3 = (ri >> 12) & bitsmask3;
  rs1 = (ri >> 15) & bitsmask5;
  rs2 = (ri >> 20) & bitsmask5;
  funct7 = (ri >> 25) & bitsmask7;
  shamt = (ri >> 20) & bitsmask5;
  imm12_i = ((int32_t) ri) >> 20;
  imm12_s = ((ri  >> 7) & bitsmask5) | ((((int32_t) ri) >> 20) & (~bitsmask5) );
  imm13 = (((ri >> 7) & bitmask) << 11) | (((ri >> 8) & nibblemask) << 1) | ((((ri >> 25) & bitsmask6)) << 5) | ((((int32_t) ri) >> 19) & (~bitsmask12));
  imm20_u = ri & (~bitsmask12);
  imm21 = (((ri >> 12) & bytemask) << 12) | (((ri >> 20) & bitmask) << 11) | (((ri >> 21) & nibblemask) << 1) |
  (((ri >> 25) & bitsmask6) << 5) | ((((int32_t) ri) >> 11) & (~bitsmask20));
  return;
}

void execute(){
  uint32_t pca = pc - 4; //ponteiro para a instrução atual
  char *str;
  int i;
  switch (opcode) {
    case LUI:
      breg[rd] = imm20_u;
      break;
    case AUIPC:
      breg[rd] = pca + imm20_u;
      break;
    case JAL:
      breg[rd] = pc;
      pc = pca + imm21;
      break;
    case JALR:
      breg[rd] = pc;
      pc = breg[rs1] + (imm12_i&~bitmask);
      break;
    case ECALL:
      switch (breg[a7]) {
        case PrintInt:
          printf("%d", breg[a0]);
          break;
        case PrintString:
          str = (char*)mem;
          str = str + breg[a0];
          i = 0;
          while(*(str+i) != '\0'){
            printf("%c", *(str+i));
            i++;
          }
          break;
        case Exit:
          CALLEXIT = TRUE;
          return;
          break;
      }
      break;
    case ILType:
      switch (funct3) {
        case LB3:
          breg[rd] = lb(breg[rs1], imm12_i);
          break;
        case LH3:
          breg[rd] = lh(breg[rs1], imm12_i);
          break;
        case LW3:
          breg[rd] = lw(breg[rs1], imm12_i);
          break;
        case LBU3:
          breg[rd] = lbu(breg[rs1], imm12_i);
          break;
        case LHU3:
          breg[rd] = lhu(breg[rs1], imm12_i);
          break;
      }
      break;
    case Btype:
      switch (funct3) {
        case BEQ3:
          if(breg[rs1] == breg[rs2]){
            pc = pca + imm13;
          }
          break;
        case BNE3:
          if(breg[rs1] != breg[rs2]){
            pc = pca + imm13;
          }
          break;
        case BLT3: //com sinal
         if(breg[rs1] < breg[rs2]){
           pc = pca + imm13;
         }
          break;
        case BGE3: //com sinal
          if(breg[rs1] >= breg[rs2]){
            pc = pca + imm13;
          }
          break;
        case BLTU3: //sem sinal
          if((uint32_t)breg[rs1] < (uint32_t)breg[rs2]){
            pc = pca + imm13;
          }
          break;
        case BGEU3: //sem sinal
          if((uint32_t)breg[rs1] >= (uint32_t)breg[rs2]){
            pc = pca + imm13;
          }
          break;
      }
      break;
    case StoreType:
      switch (funct3) {
        case SB3:
          sb(breg[rs1], imm12_s, breg[rs2]);
          break;
        case SH3:
          sh(breg[rs1], imm12_s, breg[rs2]);
          break;
        case SW3:
          sw(breg[rs1], imm12_s, breg[rs2]);
          break;
      }
      break;
    case ILAType:
      switch (funct3) {
        case ADDI3:
          breg[rd] = breg[rs1] + imm12_i;
          break;
        case ORI3:
          breg[rd] = breg[rs1] | imm12_i;
          break;
        case SLTI3:
          if(breg[rs1] < imm12_i){
            breg[rd] = 1;
          }
          else{
            breg[rd] = 0;
          }
          break;
        case XORI3:
          breg[rd] = breg[rs1] ^ imm12_i;
          break;
        case ANDI3:
          breg[rd] = breg[rs1] & imm12_i;
          break;
        case SLTIU3:
          if((uint32_t)breg[rs1] < (uint32_t)imm12_i){
            breg[rd] = 1;
          }
          else{
            breg[rd] = 0;
          }
          break;
        case SLLI3:
          breg[rd] = breg[rs1] << shamt;
          break;
        case SRI3:
          switch (funct7) {
            case SRAI7:
              breg[rd] = breg[rs1] >> shamt;
              break;
            case SRLI7:
              breg[rd] = ((uint32_t) breg[rs1]) >> shamt;
              break;
          }
          break;
      }
      break;

    case RegType:
      switch (funct3) {
        case ADDSUB3:
          switch (funct7) {
            case ADD7:
              breg[rd] = breg[rs2] + breg[rs1];
              break;
            case SUB7:
              breg[rd] = breg[rs1] - breg[rs2];
              break;
          }
          break;
        case SLL3:
          breg[rd] = breg[rs1] << breg[rs2];
          break;
        case SLT3:
          if(breg[rs1] < breg[rs2]){
            breg[rd] = 1;
          }
          else{
            breg[rd] = 0;
          }
          break;
        case SLTU3:
          if((uint32_t)breg[rs1] < (uint32_t)breg[rs2]){
            breg[rd] = 1;
          }
          else{
            breg[rd] = 0;
          }
          break;
        case XOR3:
          breg[rd] = breg[rs1] ^ breg[rs2];
          break;
        case SR3:
          switch (funct7) {
            case SRA7:
              breg[rd] = breg[rs1] >> breg[rs2];
              break;
            case SRL7:
              breg[rd] = ((uint32_t) breg[rs1]) >> breg[rs2];
              break;
          }
          break;
        case OR3:
          breg[rd] = breg[rs1] | breg[rs2];
          break;
        case AND3:
          breg[rd] = breg[rs1] & breg[rs2];
          break;
      }
      break;
  }
  breg[zero] = 0x00000000;      //mantem o registrador zero como zero
  return;
}

void step(){
  fetch();
  decode();
  execute();
  return;
}

void run(){
  while(!CALLEXIT){ //enquanto não ocorrer uma chamada exit do ecall
    step();
    if(((pc >> 2) > TEXT_SIZE)){ //se ultrapassar memoria de código
      break;
    }
  }
}
