#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h> //mmap
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "prodcons.h"
#include "control.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct prodcons ProdCons;

//******************************************
// SEMAFORO_CRIAR
//
sem_t * semaphore_create(char * name, int value) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE UM SEMÁFORO
	//
	return so_semaphore_create(name, value);
	//==============================================
}

void prodcons_create_capacidade_portuaria() {
	//==============================================
	// CRIAR MUTEX PARA CONTROLAR O ACESSO A CAPACIDADE_PORTUARIA
	//
	// utilizar a função genérica semaphore_create
	so_prodcons_create_capacidade_portuaria();
	//==============================================
}

void prodcons_create_buffers() {
	//==============================================
	// CRIAR SEMAFOROS PARA CONTROLAR O ACESSO AOS 3 BUFFERS
	//
	// utilizar a função genérica semaphore_create
	so_prodcons_create_buffers();
	//==============================================
}

void semaphore_destroy(char * name, void * ptr) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE UM SEMÁFORO
	//
	so_semaphore_destroy(name, ptr);
	//==============================================
}

void prodcons_destroy() {
	//==============================================
	// DESTRUIR SEMÁFORO E RESPETIVO NOME
	//
	// utilizar a função genérica semaphore_destroy
	so_prodcons_destroy();
	//==============================================
}

//******************************************
void prodcons_request_d_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER DESCRICAO
	//
	so_prodcons_request_d_produce_begin();
	//==============================================
}

//******************************************
void prodcons_request_d_produce_end() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER DESCRICAO
	//
	so_prodcons_request_d_produce_end();
	//==============================================
}

//******************************************
void prodcons_request_d_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER DESCRICAO
	//
	so_prodcons_request_d_consume_begin();
	//==============================================
}

//******************************************
void prodcons_request_d_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER DESCRICAO
	//
	so_prodcons_request_d_consume_end();
	//==============================================
}

//******************************************
void prodcons_request_r_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER PEDIDO
	//
	so_prodcons_request_r_produce_begin();
	//==============================================
}

//******************************************
void prodcons_request_r_produce_end() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER PEDIDO
	//
	so_prodcons_request_r_produce_end();
	//==============================================
}

//******************************************
void prodcons_request_r_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO
	//
	so_prodcons_request_r_consume_begin();
	//==============================================
}

//******************************************
void prodcons_request_r_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO
	//
	so_prodcons_request_r_consume_end();
	//==============================================
}

//******************************************
void prodcons_response_s_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER AGENDAMENTO
	//
	so_prodcons_response_s_produce_begin();
	//==============================================
}

//******************************************
void prodcons_response_s_produce_end() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER AGENDAMENTO
	//
	so_prodcons_response_s_produce_end();
	//==============================================
}

//******************************************
void prodcons_response_s_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER AGENDAMENTO
	//
	so_prodcons_response_s_consume_begin();
	//==============================================
}

//******************************************
void prodcons_response_s_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER AGENDAMENTO
	//
	so_prodcons_response_s_consume_end();
	//==============================================
}

//******************************************
void prodcons_buffers_begin() {
	//==============================================
	// GARANTIR EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	so_prodcons_buffers_begin();
	//==============================================
}

//******************************************
void prodcons_buffers_end() {
	//==============================================
	// FIM DA ZONA DE EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	so_prodcons_buffers_end();
	//==============================================
}

//******************************************
int prodcons_update_capacidade_portuaria(int operacao) {
	//==============================================
	// OBTER MUTEX DA CAPACIDADE PORTUARIA E ATUALIZAR CAPACIDADE PORTUARIA
	//
	// se capacidade_portuaria da operacao > 0 então decrementá-lo de uma unidade e
	//   função devolve 1
	// se capacidade_portuaria da operacao = 0 então função devolve 0
	return so_prodcons_update_capacidade_portuaria(operacao);
	//==============================================
}
