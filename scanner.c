//
//  scanner.c
//  Oberon
//
//  Created by Alvaro Costa Neto on 10/12/13.
//  Copyright (c) 2013 Alvaro Costa Neto. All rights reserved.
//

//
//  Vocabulário:
//
//  - * div mod & + - or
//  - = # < <= > >= . , : ) ]
//  - of then do until ( [ ~ := ;
//  - end else elsif if while repeat
//  - array record const type var procedure begin module
//
// Conjuntos first(K), com ø indicando o vazio:
//
//  - selector = . { ø
//  - factor = ( ~ number id
//  - term = ( ~ number id
//  - simple_expr = + - ( ~ number id
//  - expr = + - ( ~ number id
//  - assignment = id
//  - proc_call = id
//  - stmt = id if while repeat ø
//  - stmt_sequence = id if while repeat ø
//  - field_list = id ø
//  - type = id array record
//  - formal_params_section = id var
//  - formal_params = (
//  - proc_head = procedure
//  - proc_body = end const type var procedure begin
//  - proc_decl = procedure
//  - declarations = const type var procedure ø
//  - module = module
//
// Conjuntos follow(K), com ø indicando o vazio:
//
//  - selector = * div mod & + - or = # < <= > >= , ) ] := of then do ; end else elsif until
//  - factor = * div mod & + - or = # < <= > >= , ) ] of then do ; end else elsif until
//  - term = + - or = # < <= > >= , ) ] of then do ; end else elsif until
//  - simple_expr = = # < <= > >= , ) ] of then do ; end else elsif until
//  - expr = , ) ] of then do ; end else elsif until
//  - assignment = ; end else elsif until
//  - proc_call = ; end else elsif until
//  - stmt = ; end else elsif until
//  - stmt_sequence = end else elsif until
//  - field_list = ; end
//  - type = ) ;
//  - formal_params_section = ) ;
//  - formal_params = ;
//  - proc_head = ;
//  - proc_body = ;
//  - proc_decl = ;
//  - declarations = end begin
//

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "errors.h"
#include "scanner.h"

FILE *input_file;

// Variávies e constantes globais
token_t current_token, last_token;
const position_t position_zero = { .line = 0, .column = 0, .index = 0 };

char current_char, last_char;
position_t current_position;

// Vetor com todas as palavras-chave da linguagem
lexem_t keywords[] = {
  { .id = "faça",         .symbol = symbol_do },
  { .id = "se",           .symbol = symbol_if },
  { .id = "de",           .symbol = symbol_of },
  { .id = "OU",           .symbol = symbol_or },
  { .id = "XOU",          .symbol = symbol_xor },
  { .id = "fim",          .symbol = symbol_end },
//  { .id = "mod",        .symbol = symbol_mod },
  { .id = "var",          .symbol = symbol_var },
  { .id = "senão",        .symbol = symbol_else },
  { .id = "então",        .symbol = symbol_then },
  { .id = "tipo",         .symbol = symbol_type },
  { .id = "matriz",       .symbol = symbol_array },
  { .id = "início",       .symbol = symbol_begin },
  { .id = "const",        .symbol = symbol_const },
//  { .id = "elsif",      .symbol = symbol_elsif },
  { .id = "até_que",      .symbol = symbol_until },
  { .id = "enquanto",     .symbol = symbol_while },
  { .id = "record",       .symbol = symbol_record },
  { .id = "repita",       .symbol = symbol_repeat },
//  { .id = "procedure",  .symbol = symbol_proc },
  { .id = "div",          .symbol = symbol_div },
//  { .id = "module",     .symbol = symbol_module },
  { .id = "fim_se",       .symbol = symbol_end_if },
  { .id = "fim_caso",     .symbol = symbol_end_case },
  { .id = "fim_para",     .symbol = symbol_end_for },
  { .id = "fim_até_seja", .symbol = symbol_end_until_true },
  { .id = "caso",         .symbol = symbol_case },
  { .id = "enquanto",     .symbol = symbol_while },
  { .id = "para",         .symbol = symbol_for },
  { .id = "até_seja",     .symbol = symbol_until_true },
  { .id = "seja",         .symbol = symbol_is },
  { .id = "efetue",       .symbol = symbol_perform },
  { .id = "execute",      .symbol = symbol_execute },
  { .id = "enquanto_for", .symbol = symbol_while_is },
  { .id = "laço",         .symbol = symbol_loop },
  { .id = "saia_caso",    .symbol = symbol_leave_if },
  { .id = "de",           .symbol = symbol_from },
  { .id = "até",          .symbol = symbol_up_to },
  { .id = "passo",        .symbol = symbol_step },
  { .id = "função",       .symbol = symbol_func },
  { .id = "leia",         .symbol = symbol_read },
  { .id = "escreva",      .symbol = symbol_write },
  { .id = "programa",     .symbol = symbol_program }
};
const unsigned int keywords_count = sizeof(keywords) / sizeof(lexem_t);

// Vetor com todos os operadores da linguagem
lexem_t operators[] = {
  { .id = "*",    .symbol = symbol_times },
  { .id = "/",    .symbol = symbol_division },
//  { .id = "&",    .symbol = symbol_and },
  { .id = "E",    .symbol = symbol_and },
  { .id = "+",    .symbol = symbol_plus },
  { .id = "-",    .symbol = symbol_minus },
  { .id = "=",    .symbol = symbol_equal },
//  { .id = "#",    .symbol = symbol_not_equal },
  { .id = "<>",   .symbol = symbol_not_equal },
  { .id = "<",    .symbol = symbol_less },
  { .id = "<=",   .symbol = symbol_less_equal },
  { .id = ">",    .symbol = symbol_greater },
  { .id = ">=",   .symbol = symbol_greater_equal },
//  { .id = "~",      .symbol = symbol_not },
  { .id = "NÃO",  .symbol = symbol_not },
  { .id = "<-",   .symbol = symbol_becomes },
//  { .id = ":=",     .symbol = symbol_becomes },
  { .id = "^",    .symbol = symbol_power }
};
const unsigned int operators_count = sizeof(keywords) / sizeof(lexem_t);

// Vetor com todos os sinais de pontuação da linguagem
lexem_t punctuation[] = {
  { .id = ".",  .symbol = symbol_period },
  { .id = ",",  .symbol = symbol_colon },
  { .id = ":",  .symbol = symbol_comma },
  { .id = ")",  .symbol = symbol_close_paren },
  { .id = "]",  .symbol = symbol_close_bracket },
  { .id = "}",  .symbol = symbol_close_braces },
  { .id = "(",  .symbol = symbol_open_paren },
  { .id = "[",  .symbol = symbol_open_bracket },
  { .id = "{",  .symbol = symbol_open_braces },
  { .id = ";",  .symbol = symbol_semicolon },
  { .id = "\"", .symbol = symbol_quotes },
  { .id = "..", .symbol = symbol_range }
};
const unsigned int punctuation_count = sizeof(keywords) / sizeof(lexem_t);

//
// Analisador léxico
//

// As funções “is_letter”, “is_digit” e “is_blank” chamam as versões internas da linguagem C. Por enquanto...
bool is_letter(char c)
{
  return isalpha(c);
}

bool is_digit(char c)
{
  return isdigit(c);
}

bool is_blank(char c)
{
  return (c == ' ' || c == '\t');
}

bool is_newline(char c, char p)
{
  return (c == '\n' && p != '\r') || c == '\r';
}

// Esta função é responsável por verificar se o identificar “id” é uma palavra reservada ou não
// O símbolo equivalente à palavra reservada é armazenado via referência no parâmetro “symbol”
bool is_keyword(identifier_t id, symbol_t *symbol)
{
  unsigned int index = 0;
  while (index < keywords_count && strcmp(keywords[index].id, id) != 0)
    index++;
  if (index < keywords_count) {
    if (symbol)
      *symbol = keywords[index].symbol;
    return true;
  }
  if (symbol)
    *symbol = symbol_null;
  return false;
}


char *id_for_symbol(symbol_t symbol)
{
  if (symbol == symbol_integer)
    return "inteiro";
  else if (symbol == symbol_real)
    return "real";
//  else if (symbol == symbol_string)
//    return "cadeia";
  for (unsigned int index = 0; index < keywords_count; index++)
    if (keywords[index].symbol == symbol)
      return keywords[index].id;
  for (unsigned int index = 0; index < operators_count; index++)
    if (operators[index].symbol == symbol)
      return operators[index].id;
  for (unsigned int index = 0; index < punctuation_count; index++)
    if (punctuation[index].symbol == symbol)
      return punctuation[index].id;
  return "unknown";
}

symbol_t inverse_condition(symbol_t symbol)
{
  switch (symbol) {
    case symbol_equal: return symbol_not_equal; break;
    case symbol_not_equal: return symbol_equal; break;
    case symbol_less: return symbol_greater_equal; break;
    case symbol_less_equal: return symbol_greater; break;
    case symbol_greater: return symbol_less_equal; break;
    case symbol_greater_equal: return symbol_less; break;
    default: break;
  }
  return symbol_null;
}

// A razão de se criar uma função somente para isto é aproveitá-la se a codificação do arquivo de código-fonte mudar
bool read_char()
{
  last_char = current_char;
  if (fread(&current_char, sizeof(char), 1, input_file) == sizeof(char)) {
    if (is_newline(current_char, last_char)) {
      current_position.line++;
      current_position.column = 0;
    } else current_position.column++;
    current_position.index++;
    return true;
  }
  return false;
}

//
// ATENÇÃO: todas as funções do analisador léxico devem garantir que “current_char” termine com o caractere subsequente
// ao lexema reconhecido. Por exemplo, ao analisar “var x: integer”, a função “id“ será a primeira a ser invocada para
// reconhecer “var”. Ao terminar, “current_char” deve conter o espaço em branco entre “var” e “x”
//
// As funções “id”, “integer” e “number” fazem parte da EBNF e deveriam ser consideradas parte do analisador sintático.
// No entanto, pela forma com que o compilador está definido, o reconhecimento de lexemas também é estipulado pela EBNF
// fazendo com que a análise léxica seja realizada por um “mini descendente recursivo” ao invés de um autômato finito
//

void id()
{
  unsigned int index = 0;
  current_token.position = current_position;
  while (index < SCANNER_MAX_ID_LENGTH && (is_letter(current_char) || is_digit(current_char))) {
    current_token.lexem.id[index++] = current_char;
    if (!read_char())
      break;
  }
  // O tamanho máximo para um identificador é especificado por “id_length”, a variável “scanner_id” possui tamanho
  // “id_length + 1” e por isso o caractere terminador pode ser incluído mesmo que o limite seja alcançado
  current_token.lexem.id[index] = '\0';
  if (!is_keyword(current_token.lexem.id, &current_token.lexem.symbol))
    current_token.lexem.symbol = symbol_id;
}

// TODO: Adicionar verificação se o número é muito longo
// Por definição, somente números positivos inteiros são reconhecidos
void number()
{
  unsigned int index = 0; //contador da string
  current_token.position = current_position;
  current_token.value = 0;
  identifier_t id; //o número em si
  while (index < SCANNER_MAX_ID_LENGTH && is_digit(current_char)) { //para número inteiro
    id[index] = current_char;
    current_token.lexem.id[index] = current_char;
    index++;
    // Efetua o cálculo do valor, dígito-a-dígito, com base nos caracteres lidos
    current_token.value = 10 * current_token.value + (current_char - '0');
    if (!read_char())
      break;
  }
  current_token.lexem.symbol = symbol_integer;
  if (current_char == '.') { //para número real
    id[index] = current_char;
    current_token.lexem.id[index] = current_char;
    index++;
    read_char();
    float factor = 0.1;
    while (index < SCANNER_MAX_ID_LENGTH && is_digit(current_char)) {
      id[index] = current_char;
      current_token.lexem.id[index] = current_char;
      index++;
      // Efetua o cálculo do valor, dígito-a-dígito, com base nos caracteres lidos
      current_token.value = current_token.value + (current_char - '0') * factor;
      factor *= 0.1;
      if (!read_char())
        break;
    }
    current_token.lexem.symbol = symbol_real;
  }
  current_token.lexem.id[index] = '\0';
  // Avalia se há caracteres inválidos após os dígitos do número
  bool invalid_ending = false;
  while (index < SCANNER_MAX_ID_LENGTH && (is_letter(current_char) || current_char == '_')) {
    id[index++] = current_char;
    invalid_ending  = true;
    if (!read_char())
      break;
  }
  if (invalid_ending)
    mark(error_warning, "\"%s\" is not a number. Assuming \"%s\".", id, current_token.lexem.id);
}

void string ()
{
  unsigned int index = 0; //contador da string
  read_char(); //lë o próximo caracter e armazena no arquivo
  while (index < SCANNER_MAX_ID_LENGTH && current_char != '\"') { //comprimento máximo da string e o que está dentro dela
    index++; //lë próximo caracter
    read_char();
  }
  if (current_char == '\"'){ //símbolo final da string não deve ser armazenado
    current_char = '\0'; //fim da string
    read_char();
    break;
  }
}

// Ao entrar nesta função, o analisador léxico já encontrou os caracteres "(*" que iniciam o comentário e “current_char”
// possui o asterisco como valor
void comment()
{
  current_token.position = current_position;
  while (read_char()) {
    // Comentários aninhados
    if (current_char == '*' && last_char == '(')
      comment();
    // Fim do comentário
    if (current_char == ')' && last_char == '*') {
      read_char();
      return;
    }
  }
  mark(error_fatal, "Endless comment detected.");
  current_token.lexem.symbol = symbol_eof;
}

void read_token() //analisador léxico
{
  // O “last_token” não é mais usado... deixei aqui só por precaução
  // last_token = current_token;
  if (feof(input_file)) {
    if (current_token.lexem.symbol != symbol_eof) {
      strcpy(current_token.lexem.id, "EOF");
      current_token.lexem.symbol = symbol_eof;
    }
    return;
  }
  // Salta os caracteres em branco
  while (is_blank(current_char))
    read_char();
  // Os casos de um identificador ou um número são considerados separadamente para que o código no “switch” não precise
  // incluir uma chamada a “read_char” em cada “case”
  if (is_letter(current_char)) {
    id();
    return;
  }
  else if (is_digit(current_char)) {
    number();
    return;
  }
  else if (current_char == '\"') {
    string();
    return;
  }
  current_token.position = current_position;
  current_token.lexem.id[0] = current_char;
  switch (current_token.lexem.id[0]) {
    case '&': current_token.lexem.symbol = symbol_and;            break;
    case '*': current_token.lexem.symbol = symbol_times;          break;
    case '+': current_token.lexem.symbol = symbol_plus;           break;
    case '-': current_token.lexem.symbol = symbol_minus;          break;
    case '=': current_token.lexem.symbol = symbol_equal;          break;
    case '#': current_token.lexem.symbol = symbol_not_equal;      break;
    case '<': current_token.lexem.symbol = symbol_less;           break;
    case '>': current_token.lexem.symbol = symbol_greater;        break;
    case ';': current_token.lexem.symbol = symbol_semicolon;      break;
    case ',': current_token.lexem.symbol = symbol_comma;          break;
    case ':': current_token.lexem.symbol = symbol_colon;          break;
    case '.': current_token.lexem.symbol = symbol_period;         break;
    case '(': current_token.lexem.symbol = symbol_open_paren;     break;
    case ')': current_token.lexem.symbol = symbol_close_paren;    break;
    case '[': current_token.lexem.symbol = symbol_open_bracket;   break;
    case ']': current_token.lexem.symbol = symbol_close_bracket;  break;
    case '~': current_token.lexem.symbol = symbol_not;            break;
    default:  current_token.lexem.symbol = symbol_null;           break;
  }
  current_token.lexem.id[1] = '\0';
  read_char();
  if (current_token.lexem.symbol == symbol_null) {
    mark(error_scanner, "\"%s\" is not a valid symbol.", current_token.lexem.id);
    return;
  }
  // Os casos abaixo representam os lexemas com mais de um caracter (como “>=”, “:=” etc.)
  if (current_token.lexem.symbol == symbol_less && current_char == '=') {
    current_token.lexem.id[1] = '=';
    current_token.lexem.id[2] = '\0';
    read_char();
    current_token.lexem.symbol = symbol_less_equal;
  }
  else if (current_token.lexem.symbol == symbol_greater && current_char == '=') {
    current_token.lexem.id[1] = '=';
    current_token.lexem.id[2] = '\0';
    read_char();
    current_token.lexem.symbol = symbol_greater_equal;
  }
  else if (current_token.lexem.symbol == symbol_colon && current_char == '=') {
    current_token.lexem.id[1] = '=';
    current_token.lexem.id[2] = '\0';
    read_char();
    current_token.lexem.symbol = symbol_becomes;
  }
  /*else if (current_token.lexem.symbol == symbol_period && current_char == 'o') {
    read_char();
    if (current_char == 'u')
      read_char();
    // TODO: Adicionar verificação de erros!
    if (current_char == '.')
      read_char();
    strcat(current_token.lexem.id, "ou.");
    current_token.lexem.symbol = symbol_or;
  }*/
  else if (current_token.lexem.symbol == symbol_open_paren && current_char == '*') {
    read_char();
    // Ignora os caracteres entre “(*” e “*)” como sendo comentários e entra novamente na função para buscar o próximo
    // lexema válido
    comment();
    read_token();
  }
}

void initialize_scanner(FILE *file)
{
  input_file = file;
  strcpy(current_token.lexem.id, "");
  current_token.position = position_zero;
  current_token.lexem.symbol = symbol_null;
  current_token.value = 0;
  current_position.line = 1;
  current_position.column = 0;
  current_position.index = 0;
  current_char = '\0';
  read_char();
}
