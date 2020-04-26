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

extern struct configuration Config;

int cliente_executar(int id) {
	int n, count, ret;
	struct operation operacao;
	int operation_id;
	char buf[100];
	char *result;

	setbuf(stdout, NULL);

	n = 0;
	count = 0;
	result = Config.list_clientes;
	while (n < id) {
		while (Config.list_clientes[count++] != '\0')
			;
		result = &Config.list_clientes[count];
		n++;
	}

	operation_id = atoi(result);
	operacao.id = operation_id;
	operacao.cliente = id;

	//==============================================================
	// Introduzir código que permite ao cliente após
	// a conclusão duma operação, não abandonar o SO_AdmPor e esperar
	// para submeter outra operação até a um máximo de N (Config.N) vezes

	memory_request_d_write(id, &operacao);
	memory_response_s_read(id, &operacao);

	if (operacao.disponibilidade == 1) {
		printf(
				"     CLIENTE %03d solicitou %d e obteve %d (v:%02d c:%02d t:%.9fs)\n",
				id, operation_id, operacao.id, operacao.intermediario, operacao.empresa,
				time_difference(operacao.time_descricao, operacao.time_agendamento));
		sprintf(buf, "     CLIENTE %03d solicitou %d e obteve %d\n", id,
				operation_id, operacao.id);
		ret = operacao.id;
	} else {
		printf("     CLIENTE %03d solicitou %d mas nao estava disponivel!\n",
				id, operation_id);
		sprintf(buf,
				"     CLIENTE %03d solicitou %d mas nao estava disponivel!\n",
				id, operation_id);
		ret = Config.OPERATIONS;
	}

	file_write_line(buf);
	//===================================================

	return ret;
}

