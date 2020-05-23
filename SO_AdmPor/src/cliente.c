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

#include "cliente.h"
#include "main.h"
#include "memory.h"
#include "file.h"
#include "sotime.h"
#include "so.h"

extern struct configuration Config;

int cliente_executar(int id) {
	
	//==============================================
	// EXECUTAR OPERACOES DO CLIENTE
	//
	// modificar cliente do Projeto 1:
	// - buf deve ter capacidade para pelo menos 500 chars
	// - repetir a mesma operacao Config.N vezes (escrever
	// e ler de memória, processar e imprimir resultados)
	// - fazer sleep de 1ms entre pedidos
	// - guardar operacao.cliente e tempo de execucao de cada
	// operacao em Ind.tempos, usando id, Config.N e i para indexar
	// - modificar mensagem impressa para ficar igual à de
	// soadmpor e para usar dados vindos de operacao 
	//
	return so_cliente_operations(id);
	//==============================================
}

