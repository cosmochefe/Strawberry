//
//  scanner.h
//  Oberon
//
//  Created by Alvaro Costa Neto on 10/12/13.
//  Copyright (c) 2013 Alvaro Costa Neto. All rights reserved.
//

#ifndef Oberon_scanner_h
#define Oberon_scanner_h

#include <stdio.h>
#include <stdbool.h>

#define SCANNER_MAX_ID_LENGTH 16

typedef enum _symbol {
	symbol_null = 0,

	//aritméticos
	symbol_times = 1,
	symbol_division = 2, //divisão
	symbol_div = 3, //divisão de número inteiro
	//symbol_mod = 4,
	symbol_plus = 6,
	symbol_minus = 7,
    symbol_power = 76,
	//lógicos
	symbol_and = 5,
	symbol_or = 8,
	symbol_not = 32,
	symbol_xor = 65,

	//comparação
	symbol_equal = 9,
	symbol_not_equal = 10,
	symbol_less = 11,
	symbol_less_equal = 12,
	symbol_greater = 13,
	symbol_greater_equal = 14,

	//pontuação
	symbol_period = 18,
	symbol_comma = 19,
	symbol_colon = 20,
	symbol_close_paren = 22,
	symbol_close_bracket = 23,
	symbol_close_braces = 24,
	symbol_open_paren = 29,
	symbol_open_bracket = 30,
	symbol_open_braces = 31,
	symbol_becomes = 33, //atribuição	symbol_semicolon = 38,
    symbol_range = 77, //".."
	//elemento léxico
	symbol_number = 34,
	symbol_id = 37,

	//palavras-chave
	symbol_of = 25, //de
	symbol_then = 26, //então
	symbol_do = 27, // faça
	symbol_end = 40, //fim    symbol_end_if = 66, //fim_se    symbol_end_case = 67, //fim_caso    symbol_end_while = 68, //fim_enquanto    symbol_end_for = 69, //fim_para    symbol_end_until_true = 70, //fimaté_seja    symbol_else = 41, //senão
	//symbol_elsif = 42,
	symbol_until = 43, //até_que
	symbol_if = 44, //se
	symbol_case = 45, //caso    symbol_while = 46, //enquanto
	symbol_repeat = 47, //repita    symbol_for = 48, //para    symbol_until_true = 49, //até_seja    symbol_is = 50, //seja    symbol_perform = 51, //efetue    symbol_execute = 52, //execute    symbol_while_is = 53, //enquanto_for    symbol_loop = 71, //laço    symbol_leave_if = 72, //saia_caso    symbol_from = 73, //de    symbol_up_to = 74, //até    symbol_step = 75, //passo
	symbol_array = 54,
	symbol_record = 55,
	symbol_const = 57,
	symbol_type = 58,
	symbol_var = 59,
	//symbol_proc = 60,    symbol_func = 60,
    symbol_read = 78, //leia    symbol_write = 79, //escreva	symbol_begin = 61, //inicio
//	symbol_module = 63,
	symbol_program = 63,
	symbol_eof = 64
} symbol_t;

#include "backend.h"

// TODO: Organizar esta bagunça. Simplificar!

typedef char identifier_t[SCANNER_MAX_ID_LENGTH + 1];

typedef struct _position {
	unsigned int line;
	unsigned int column;
	fpos_t index;
} position_t;

typedef struct _lexem {
	identifier_t id;
	symbol_t symbol;
} lexem_t;

typedef struct _token {
	lexem_t lexem;
	value_t value;
	position_t position;
} token_t;

extern token_t current_token, last_token;

extern const position_t position_zero;

bool is_first(const char *non_terminal, symbol_t symbol);
bool is_follow(const char *non_terminal, symbol_t symbol);

char *id_for_symbol(symbol_t symbol);

symbol_t inverse_condition(symbol_t symbol);

void initialize_scanner(FILE *file);
void read_token();

#endif
