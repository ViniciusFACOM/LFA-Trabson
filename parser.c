/*
 parser.c : analisador sintatico (exemplo de automato com pilha)
 Autor: Edna A. Hoshino

 S  -> Function S_ 
 S_ -> Function S_ 
      | epsilon
 Function -> Type Function_
 Type -> void 
       | int 
       | float
 Function_ -> main() { D B } 
            | id() { D B }
 B -> C B 
      | epsilon
 C -> id = E ; 
      | while (E) C 
      | { B }

 D -> Type L ; D
 	| epsilon
 E -> TE'
 E'-> +TE' 
      | epsilon
 T -> FT'
 T'-> *FT' 
      | epsilon
 F -> (E) 
      | id 
      | num
 L -> id L_
 L_ -> , L
 	 | epsilon

 A saida do analisador apresenta o total de linhas processadas e uma mensagem de erro ou sucesso. 
 Atualmente, nao ha controle sobre a coluna e a linha em que o erro foi encontrado.
*/

#include "lex.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

/* variaveis globais */
int lookahead;
FILE *fin;

int lex();
void S();
void S_();
void Function();
void Type();
void Function_();
void B();
void C();
void D();
void E();
void T();
void F();
void E_();
void T_();
void L();
void L_();

void match(int t)
{
  if(lookahead==t){
    lookahead=lex();
  }
  else{
    printf("\nErro: token %s (cod=%d) esperado.## Encontrado \"%s\" ##\n", terminalName[t], t, lexema);
    exit(1);
  }
}

void S(){
  Function();
  S_();
}

void S_(){
  if(lookahead==INT || lookahead==FLOAT || lookahead==VOID){
    Function();
    S_();
  }
}

void Function(){
  Type();
  Function_();
}

void Type(){  
  if(lookahead==INT){
    match(INT);
  }
  else if(lookahead==FLOAT){
    match(FLOAT);
  }
  else{
    match(VOID);
  }
}

void Function_(){
  if(lookahead == MAIN){
    match(MAIN);
    match(ABRE_PARENT);
    match(FECHA_PARENT);
    match(ABRE_CHAVES);
    D();
    B();
    match(FECHA_CHAVES);
  }
  else{
    match(ID);
    match(ABRE_PARENT);
    match(FECHA_PARENT);
    match(ABRE_CHAVES);
    D();
    B();
    match(FECHA_CHAVES);
  }
}

void B(){
  if(lookahead==ID || lookahead==WHILE || lookahead==ABRE_CHAVES){
    C();
    B();
  }
}
void C(){
  if(lookahead==ID){
    match(ID);
    match(OP_ATRIB);
    E();
    match(PONTO_VIRG);
  }
  else if(lookahead==WHILE){
    match(WHILE);
    match(ABRE_PARENT);
    E();
    match(FECHA_PARENT);
    C();
  }
  else if(lookahead==ABRE_CHAVES){
    match(ABRE_CHAVES);
    B();
    match(FECHA_CHAVES);
  }
}


void E(){
  T();
  E_();
}

void T(){
  F();
  T_();
}
void E_(){
  if(lookahead==OP_ADIT){
    match(OP_ADIT);
    T();
    E_();
  }
}
void F(){
  if(lookahead==ABRE_PARENT){
    match(ABRE_PARENT);
    E();
    match(FECHA_PARENT);
  }
  else{
    if(lookahead==ID){
      match(ID);
    }
    else
      match(NUM);
  }
}
void T_(){
  if(lookahead==OP_MULT){
    match(OP_MULT);
    F();
    T_();
  }
}

void D(){
	if(lookahead == INT || lookahead == FLOAT || lookahead == VOID){
		Type();
		L();
		match(PONTO_VIRG);
		D();
	}
}
void L(){
	match(ID);
	L_();
}

void L_(){
	if(lookahead == VIRG){
		match(VIRG);
		L();
	}
}

/*******************************************************************************************
 parser(): 
 - efetua o processamento do automato com pilha AP
 - devolve uma mensagem para indicar se a "palavra" (programa) estah sintaticamente correta.
********************************************************************************************/
char *parser()
{
  lookahead=lex();
  S();
  if(lookahead==FIM)
    return("Programa sintaticamente correto!");
  else
    return("Fim de arquivo esperado");
}

int main(int argc, char**argv)
{
  if(argc<2){
    printf("\nUse: compile <filename>\n");
    return 1;
  }
  else{
    printf("\nAnalisando lexica e sintaticamente o programa: %s", argv[1]);
    fin=fopen(argv[1], "r");
    if(!fin){
      printf("\nProblema na abertura do programa %s\n", argv[1]);
      return 1;
    }
    printf("\nTotal de linhas processadas: %d\nResultado: %s\n", lines, parser());
    fclose(fin);
    return 0;
  }
}

