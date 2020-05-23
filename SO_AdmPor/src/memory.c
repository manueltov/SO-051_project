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
#include "memory.h"
#include "prodcons.h"
#include "control.h"
#include "file.h"
#include "sotime.h"
#include "scheduler.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct request_d BDescricao; 	// buffer cliente-intermediario (request_description)
struct request_r BPedido; 	// buffer intermediario-empresa (request_operation)
struct response_s BAgendamento;  	// buffer empresa-cliente (response_scheduling)

//******************************************
// CRIAR ZONAS DE MEMORIA
//
void * memory_create(char * name, int size) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE MEMÓRIA PARTILHADA
	//
	// usar getuid() para gerar nome unico na forma:
	// sprintf(name_uid,"/%s_%d", name, getuid())
	// usar name_uid em shm_open
	// usar tambem: ftruncate e mmap
	return so_memory_create(name, size);
	//==============================================
}
void memory_create_capacidade_portuaria() {
	//==============================================
	// CRIAR ZONA DE MEMÓRIA PARA A CAPACIDADE DE PORTUARIA
	//
	// utilizar a função genérica memory_create(char *,int)
	// para criar ponteiro que se guarda em Config.capacidade_portuaria
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [OPERACOES] para inteiro
	so_memory_create_capacidade_portuaria();
	//==============================================
}
void memory_create_buffers() {
	//==============================================
	// CRIAR ZONAS DE MEMÓRIA PARA OS BUFFERS: DESCRICAO, PEDIDO e AGENDAMENTO
	//
	// utilizar a função genérica memory_create(char *,int)
	//
	// para criar ponteiro que se guarda em BAgendamento.ptr
	// que deve ter capacidade para struct pointers
	// para criar ponteiro que se guarda em BAgendamento.buffer
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [BUFFER_AGENDAMENTO] para struct operation
	//
	// para criar ponteiro que se guarda em BPedido.ptr
	// que deve ter capacidade para struct pointers
	// para criar ponteiro que se guarda em BPedido.buffer
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [BUFFER_PEDIDO] para struct operation
	//
	// para criar ponteiro que se guarda em BDescricao.ptr
	// que deve ter capacidade para um vetor unidimensional
	// de inteiros com a dimensao [BUFFER_DESCRICAO]
	// para criar ponteiro que se guarda em BDescricao.buffer
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [BUFFER_DESCRICAO] para struct operation
	so_memory_create_buffers();
	//==============================================
}
void memory_create_scheduler() {
	//==============================================
	// CRIAR ZONA DE MEMÓRIA PARA O MAPA DE ESCALONAMENTO
	//
	// utilizar a função genérica memory_create(char *,int)
	// para criar ponteiro que se guarda em Schedule.ptr
	// que deve ter capacidade para um vetor bidimensional
	// com a dimensao [OPERACOES,EMPRESAS] para inteiro
	so_memory_create_scheduler();
	//==============================================
}

void memory_destroy(char * name, void * ptr, int size) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE MEMÓRIA PARTILHADA
	//
	// usar getuid() para gerar nome unico na forma:
	// sprintf(name_uid,"/%s_%d", name, getuid())
	// usar name_uid em shm_unlink
	// usar tambem: munmap
	so_memory_destroy(name, ptr, size);
	//==============================================
}

//******************************************
// MEMORIA_DESTRUIR
//
void memory_destroy_all() {
	//==============================================
	// DESTRUIR MAPEAMENTO E NOME DE PÁGINAS DE MEMÓRIA
	//
	// utilizar a função genérica memory_destroy(char *,void *,int)
	so_memory_destroy_all();
	//==============================================
}

//******************************************
// memory_request_d_write
//
void memory_request_d_write(int id, struct operation *operacao) {
	prodcons_request_d_produce_begin();

	// registar hora do pedido de servico
	time_register(&operacao->time_descricao);

	//==============================================
	// ESCREVER DESCRICAO DE OPERACOES NO BUFFER DESCRICOES
	//
	// procurar posicao vazia no buffer BDescricao
	// copiar conteudo relevante da estrutura operacao para
	// a posicao BDescricao.ptr do buffer BDescricao
	// conteudo: cliente, id, time_descricao
	// colocar BDescricao.ptr[n] = 1 na posicao respetiva
	so_memory_request_d_write(id, operacao);
	//==============================================

	prodcons_request_d_produce_end();

	// informar INTERMEDIARIO de pedido de operacao
	control_cliente_submete_descricao(id);

	// log
	file_write_log_file(1, id);
}
//******************************************
// memory_request_d_read
//
int memory_request_d_read(int id, struct operation *operacao) {
	// testar se existem clientes e se o SO_AdmPor está open
	if (control_intermediario_esperapor_descricao(id) == 0)
		return 0;

	prodcons_request_d_consume_begin();

	//==============================================
	// LER DESCRICAO DO BUFFER DE DESCRICOES
	//
	// procurar descrição de operação para a empresa no buffer BDescricao
	// copiar conteudo relevante da posicao encontrada
	// no buffer BDescricao para operacao
	// conteudo: cliente, id, time_descricao
	// colocar BDescricao.ptr[n] = 0 na posicao respetiva
	so_memory_request_d_read(id, operacao);
	//==============================================

	// testar se existe capacidade portuaria para servir cliente
	if (prodcons_update_capacidade_portuaria(operacao->id) == 0) {
		operacao->disponibilidade = 0;
		prodcons_request_d_consume_end();
		return 2;
	} else
		operacao->disponibilidade = 1;

	prodcons_request_d_consume_end();

	// log
	file_write_log_file(2, id);

	return 1;
}

//******************************************
// memory_request_r_write
//
void memory_request_r_write(int id, struct operation *operacao) {
	int pos, empresa;

	prodcons_request_r_produce_begin();

	// decidir a que empresa se destina
	empresa = scheduler_get_empresa(id, operacao->id);

	//==============================================
	// ESCREVER PEDIDO NO BUFFER DE PEDIDOS DE OPERACOES
	//
	// copiar conteudo relevante da estrutura operacao para
	// a posicao BDescricao.ptr->in do buffer BPedido
	// conteudo: cliente, id, disponibilidade, intermediario, empresa, time_descricao
	// e atualizar BPedido.ptr->in
	pos = so_memory_request_r_write(id, operacao, empresa);
	//==============================================

	prodcons_request_r_produce_end();

	// informar empresa respetiva de pedido de operacao
	control_intermediario_submete_pedido(empresa);

	// registar hora pedido (operacao)
	time_register(&BAgendamento.buffer[pos].time_pedido);

	// log
	file_write_log_file(3, id);
}
//******************************************
// memory_request_r_read
//
int memory_request_r_read(int id, struct operation *operacao) {
	// testar se existem pedidos e se o SO_AdmPor está open
	if (control_empresa_esperapor_pedido(id) == 0)
		return 0;

	prodcons_request_r_consume_begin();

	//==============================================
	// LER PEDIDO DO BUFFER DE PEDIDOS DE OPERACOES
	//
	// copiar conteudo relevante dessa posicao BPedido.ptr->out
	// do buffer BPedido para a estrutura operacao
	// conteudo: cliente, id, disponibilidade, intermediario, time_descricao, time_pedido
	// e atualizar BPedido.ptr->out
	so_memory_request_r_read(id, operacao);
	//==============================================

	prodcons_request_r_consume_end();

	// log
	file_write_log_file(4, id);

	return 1;
}

//******************************************
// memory_response_s_write
//
void memory_response_s_write(int id, struct operation *operacao) {
	int pos;

	prodcons_response_s_produce_begin();

	//==============================================
	// ESCREVER AGENDAMENTO NO BUFFER DE AGENDAMENTO
	//
	// copiar conteudo relevante da estrutura operacao para
	// a posicao BAgendamento.ptr->in do buffer BAgendamento
	// conteudo: cliente, id, disponibilidade, intermediario, empresa, time_descricao, time_pedido
	// e atualizar BAgendamento.ptr->in
	pos = so_memory_response_s_write(id, operacao);
	//==============================================

	prodcons_response_s_produce_end();

	// informar cliente de que a agendamento esta pronta
	control_empresa_submete_agendamento(operacao->cliente);

	// registar hora pronta (agendamento)
	time_register(&BAgendamento.buffer[pos].time_agendamento);

	// log
	file_write_log_file(5, id);
}
//******************************************
// memory_response_s_read
//
void memory_response_s_read(int id, struct operation *operacao) {
	// aguardar agendamento
	control_cliente_esperapor_agendamento(operacao->cliente);

	prodcons_response_s_consume_begin();

	//==============================================
	// LER AGENDAMENTO DO BUFFER DE AGENDAMENTO
	//
	// copiar conteudo relevante da posicao BAgendamento.ptr->out
	// do buffer BAgendamento para a estrutura operacao
	// conteudo: cliente, disponibilidade, intermediario, empresa, time_descricao, time_pedido, time_agendamento
	// e atualizar BAgendamento.ptr->out
	so_memory_response_s_read(id, operacao);
	//==============================================

	prodcons_response_s_consume_end();

	// log
	file_write_log_file(6, id);
}

//******************************************
// MEMORIA_CRIAR_INDICADORES
//
void memory_create_statistics() {
	//==============================================
	// CRIAR ZONAS DE MEMÓRIA PARA OS INDICADORES
	//
	// criação dinâmica de memória
	// para cada campo da estrutura indicadores
	so_memory_create_statistics();
	// iniciar indicadores relevantes:
	// - Ind.capacidade_inicial_portuaria (c/ Config.capacidade_portuaria respetivo)
	// - Ind.clientes_servidos_por_intermediarios (c/ 0)
	// - Ind.clientes_servidos_por_empresas (c/ 0)
	// - Ind.serviços_recebidos_por_clientes (c/ 0)
	so_memory_prepare_statistics();
	//==============================================
}

