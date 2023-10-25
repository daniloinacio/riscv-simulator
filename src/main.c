/*Simulador de instruções do RISCV RV32I
compilador: gcc
sistema operacional: Ubuntu 16.04
IDE: Atom
*/
#include "simulator.h"
#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[]){
  FILE *text, *data;
  int written_mem;
  uint32_t start, end;
  char input, format;

  if(argc < 3){
    printf("Incorrect number of arguments\n");
    return -1;
  }

  printf("-----------------------------------------------------------");
  printf("\n\nRISCV SIMULATOR\n\n");
  printf("###############################################################\n");
  printf("###############################################################\n");
  printf("#          ####   #            ###          ##    ########    #\n");
  printf("#   ####    ###   #    ####    #     #####   ##    ######    ##\n");
  printf("#   ###    ####   #    #########   #############    ####    ###\n");
  printf("#        ######   ##          ##   ##############    ##    ####\n");
  printf("#    ##    ####   ########    ##   ###############        #####\n");
  printf("#    ####    ##   #   ####    ##     #####   ######      ######\n");
  printf("#    #####    #   #           ####          ########    #######\n");
  printf("###############################################################\n");
  printf("###############################################################\n");
  printf("\n");
  printf("Brasilia University - 2019\n");
  printf("Project develop in Organization and Architecture of Computers discipline\n");
  printf("\n---------------------------------------------------\n");

  //carrega para a memória o arquivo binário do .text
  text = fopen(argv[1], "rb");
  if (text == NULL){
    printf("File opening error!\n");
    return -1;
  }
  fread(mem, sizeof(int32_t), TEXT_SIZE, text);

  //carrega para a memória o arquivo binário do .data
  data = fopen(argv[2], "rb");
  if (data == NULL){
    printf("File opening error!\n");
    return -1;
  }
  written_mem = fread(mem + DATA_INIT, sizeof(int32_t), DATA_SIZE, data);

  //inicializa os registradores
  set_registers();

  printf("\nSelect the display format for values:\n\n");
  printf("(h) - hexadecimal\n(d) - decimal\n\n");
  do{
    format = getchar();

  }while((format != 'h') && (format != 'd'));
  printf("\n---------------------------------------------------\n");
  printf("\nEnter in hex the memory range that will be displayed\n");
  printf("\nStart:\n\n");
  do{
    scanf("%x", &start);
  }while((start < 0) || (start > 0x4000));

  printf("\nEnd:\n\n");
  do{
    scanf("%x", &end);
  }while((end < start) || (end <= 0) || (end > 0x4000));

  printf("\n---------------------------------------------------\n");
  while(!CALLEXIT){
    printf("\n(g) - Run the current program\n(s) - Run one step at a time\n");
    printf("(q) - Exit the simulation\n(r) - Reset program\n\n");
    do{
      input = getchar();
    }while((input != 'g') && (input != 's') &&  (input != 'q') && (input != 'r'));
    printf("\n---------------------------------------------------\n");

    switch (input) {
      case 'g':
        printf("\n\n----------CONSOLE-----------\n\n");
        run();
        printf("\n---------------------------------------------------\n");
        dump_reg(format);
        dump_mem(start, end, format);
        break;
      case 's':
        printf("\n\n----------CONSOLE-----------\n\n");
        step();
        printf("\n----------------------------------------------------\n");
        dump_reg(format);
        dump_mem(start, end, format);
        break;
      case 'q':
        CALLEXIT = TRUE;
        break;
      case 'r':       //reseta os registradores e a memoria
        set_registers();
        CALLEXIT = FALSE;
        rewind(text);
        rewind(data);
        fread(mem, sizeof(int32_t), TEXT_SIZE, text);
        fread(mem + DATA_INIT, sizeof(int32_t), DATA_SIZE, data);
        break;
    }
  }
  printf("\n\n----------SIMULATION CLOSED----------\n\n");
  fclose(text);
  fclose(data);

  return 0;
}
