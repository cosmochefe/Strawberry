//
//  main.c
//  Oberon
//
//  Created by Alvaro Costa Neto on 10/11/13.
//  Copyright (c) 2013 Alvaro Costa Neto. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "scanner.h"

#define OUTPUT_EXTENSION ".asm"

void initialize_backend(FILE *file);

int main(int argc, const char *argv[])
{
  if (argc < 2) { //Procura argumentos, quantos argumentos foram usados (mais que dois).
    printf("Usage: stbry input [output]\n");
    return EXIT_FAILURE;
  }
  FILE *input_file = fopen(argv[1], "r"); //Fopen abre dois arugmentos (um arquivo e uma string).
  if (!input_file) { //Tenta abrir os arquivos de entrada.
    printf("Input file could not be opened.\n");
    return EXIT_FAILURE;
  }
  FILE *output_file;
  if (argc > 2)
    output_file = fopen(argv[2], "w+");
  else {
    char output_path[strlen(argv[1]) + strlen(OUTPUT_EXTENSION) + 1];
    strcpy(output_path, argv[1]);
    strcat(output_path, OUTPUT_EXTENSION);
    output_file = fopen(output_path, "w+");
  }
  if (!output_file) {
    printf("Output file could not be created.\n");
    return EXIT_FAILURE;
  }
//  if (!initialize_parser(input_file))
//    printf("Empty or damaged input file.\n");
//  else {
//    initialize_backend(output_file);
//    parse();
//  }

  initialize_scanner(input_file); //analizador lexico.
  while (current_token.lexem.symbol != symbol_eof) {
    read_token();
    printf("Lexema: \"%s\"; Símbolo: \"%s\"\n", current_token.lexem.id, id_for_symbol(current_token.lexem.symbol));
  }

  fclose(input_file);
  fclose(output_file);
  return EXIT_SUCCESS;
}
