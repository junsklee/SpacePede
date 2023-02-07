/**********************************************************************
  Module: main.c
  Author: Junseok Lee

  Purpose: Runs the centipede program and exits with a done message

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "centipede.h" 

int main(int argc, char**argv) {
	centipedeRun();
	printf("done!\n");
	return EXIT_SUCCESS;
}
