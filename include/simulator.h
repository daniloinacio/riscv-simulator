/*Simulador de instruções dO RISCV RV32I
compilador: gcc
sistema operacional: Ubuntu 16.04
IDE: Atom
*/
#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_
#include <stdint.h>
#define MEM_SIZE 4096
#define REG_SIZE 32
#define TEXT_SIZE 2048
#define DATA_SIZE 2048
#define DATA_INIT 2048
#define TRUE  1
#define FALSE 0


//memoria
int32_t mem[MEM_SIZE];

//banco de registradores
int32_t breg[REG_SIZE];

//registradores adicionais
uint32_t pc;
uint32_t ri;

//chamada para sair
extern int CALLEXIT;

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

//nomes dos registradores
enum REGISTER{
  zero=0, ra, sp, gp, tp, t0, t1, t2, s0, s1,
  a0, a1, a2, a3, a4, a5, a6, a7, s2, s3, s4,
  s5, s6, s7, s8, s9, s10, s11, t3, t4, t5, t6
};

//chamadas do sistema
enum SYSCALL{
  PrintInt    = 1,
  PrintString = 4,
  Exit        = 10
};


enum OPCODES { // so são considerados os 7 primeiros bits dessas constantes
  LUI       = 0x37, AUIPC = 0x17, //atribui 20 bits mais significativos
  ILType    = 0x03,               //Load type
  Btype     = 0x63,               //branch condicional
  JAL       = 0x6F, JALR  = 0x67, //jumps - JAL formato UJ, JALR formato I
  StoreType = 0x23,               //store
  ILAType   = 0x13,               //logica-aritmeticas com imediato
  RegType   = 0x33,               //operações LA com registradores
  ECALL     = 0x73                //chamada do sistema - formato I
};

enum FUNCT3{ // campo auxiliar de 3 bits
  BEQ3    =  0, BNE3  = 01, BLT3  = 04, BGE3  = 05, BLTU3 = 0x06, BGEU3 = 07,  //branches
  LB3     =  0, LH3   = 01, LW3   = 02, LBU3  = 04, LHU3  = 05,                //loads
  SB3     =  0, SH3   = 01, SW3   = 02,                                        //stores
  ADDSUB3 =  0, SLL3  = 01, SLT3  = 02, SLTU3 = 03,                            //LA com
  XOR3    = 04, SR3   = 05, OR3   = 06, AND3  = 07,                            //registradores
  ADDI3   =  0, ORI3  = 06, SLTI3 = 02, XORI3 = 04, ANDI3 = 07,                //LA com
  SLTIU3  = 03, SLLI3 = 01, SRI3  = 05                                         //imediatos
};

enum FUNCT7 {
  // campo auxiliar de 7 bits para as instruções SRLI/SRAI, ADD/SUB, SRL/SRA
  ADD7=0, SUB7=0x20, SRA7=0x20, SRL7=0, SRLI7=0x00, SRAI7=0x20
};

//mascaras usadas na decodificação
enum MASK{
  hwmask     = 0x0000ffff, bytemask   = 0x000000ff,
  nibblemask = 0x0000000f, bitmask    = 0x00000001,
  bitsmask6  = 0x0000003f, bitsmask5  = 0x0000001f,
  bitsmask3  = 0x00000007, bitsmask7  = 0x0000007f,
  bitsmask12 = 0x00000fff, bitsmask20 = 0x000fffff
};

void set_registers();

//le uma instrução da memoria, coloca em ri e incrementa pc (pc+=4)
void fetch();

//decodifica a instrução contida em ri, extraindo seus campos
void decode();

//executa uma instrução decodificada
void execute();

//executa uma etapa completa de simulação chamando fetch(), decode(), e execute()
void step();

//executa step até ultrapassar a memoria de código ou receber uma chamada
//de saida do sistema (CALLEXIT = TRUE)
void run();

//imprime palavras de 32 bits da memoria em hexa ou decimal
//start e end dever ser multiplos de 4
void dump_mem(int start, int end, char format);

//imprime os valores dos registradores em hexa ou em decimal
void dump_reg(char format);



#endif
