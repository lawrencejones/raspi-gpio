#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

// Define macro values for the paging and block size
#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)

#define VALUE_OF_PIN(g,w) (w & (7 << (g/3))) >> (g/3)
void printBinary(uint32_t* base)
{
	int pin;
	printf("|                                          |");
	printf("   |                                          |");
	printf("   |                                          |\n");
	// printf("|           --- PINS 29-20 ---           |");
	// printf("     |           --- PINS 19-10 ---           |");
	// printf("     |            --- PINS 9-0 ---            |\n");
	for (int j = 0; j < 3; j++)
	{
	  printf("|  ");
	  for (int i = 9; i >= 0; i--)
	  	printf(" %02d ", i + (2-j)*10);
		printf("|   ");
	}
	printf("\n");
	for (int j = 0; j < 3; j++)
	{
		uint32_t x = *(base + (3-j));
	  for (int i = 0; i < 32; i++)
	  {
	  	if (!((i + 1) % 3)) printf(" ");
	  	if (i > 1) {
				pin = VALUE_OF_PIN(32 - i, x);
	  		printf("\x1b[3%d;1m", ((i-2)/3)%7 + 1 + pin);
	  	}
	  	printf("%d", 0x01 & (x >> (31 - i)));
	  }
		printf(" |   ");
	}
	printf("\x1b[0m\n");
}

#define BASE 0x20200000u

int main(int argv, char** argc)
{
	if (argv != 2)
  {
  	fprintf(stderr, "Wrong number of arguments.");
  	exit(EXIT_FAILURE);
  }
  //uint32_t addr = strtol(argc[1], NULL, 16);
	//int addr = atoi(argc[1]);
	int gpio_file = open("/dev/mem", O_RDWR|O_SYNC); 
	void* gpioMap = mmap(
    NULL,        // specific address not required
    BLOCK_SIZE,  // the length of the mapping, to include
                 // all the gpio reserved space
    PROT_READ|PROT_WRITE,  // enable RW to memory
    MAP_SHARED,  // share this location with other processes
    gpio_file,      // specific file to open
    BASE    // move to the GPIO reserved space by offset
  );
	close(gpio_file);
	volatile unsigned *memword = (volatile unsigned *)gpioMap;
	printBinary((uint32_t *)memword);
	return 0;
}
