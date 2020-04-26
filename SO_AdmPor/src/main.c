/*
SO-051
Ivo Veiga, nº 44865
Joao Silva, nº 48782
Manuel Tovar, nº 49522
*/

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
#include "intermediario.h"
#include "cliente.h"
#include "empresa.h"


struct statistics Ind;     		// indicadores do funcionamento do SO_AdmPor
struct configuration Config; 	// configuração da execução do SO_AdmPor

/* main_cliente recebe como parâmetro o nº de clientes a criar */
void main_cliente(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos cliente igual a quant através de um ciclo com i=0,1,...
	// após a criação de cada processo, o processo filho realiza duas atividades:
	// - chama a função cliente_executar passando como parâmetro o valor da variável de controlo do ciclo i=0,1,...
	// - chama a função exit passando como parâmetro o valor devolvido pela função cliente_executar
	// o processo pai guarda o pid do filho no vetor Ind.pid_clientes[i], com i=0,1,... e termina normalmente a função
    int pid;

	for(int i = 0; i<quant; i++){
			pid = fork();

			if(pid == -1){
					perror("pidError");
					exit(1);
			}
			//filho
			else if(pid == 0){
				exit(cliente_executar(i));
			}
			//pai
			else
			{
				Ind.pid_clientes[i] = pid;
			}
	}
    //so_main_cliente(quant);
    //==============================================
}

/* main_intermediario recebe como parâmetro o nº de intermediarios a criar */
void main_intermediario(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos intermediarios igual a quant através de um ciclo com i=0,1,...
	// após a criação de cada processo, o processo filho realiza duas atividades:
	// - chama a função intermediarios_executar passando como parâmetro o valor da variável de controlo do ciclo i=0,1,...
	// - chama a função exit passando como parâmetro o valor devolvido pela função intermediario_executar
	// o processo pai guarda o pid do filho no vetor Ind.pid_intermediarios[i], com i=0,1,... e termina normalmente a função
    int pid;

	for(int i = 0; i<quant; i++){
		pid = fork();
		if(pid == -1){
			perror("pidError");
			exit(1);
		}
		//filho
		else if(pid == 0){
			exit(intermediario_executar(i));
		}
		//pai
		else
		{
			Ind.pid_intermediarios[i] = pid;
		}
	}
    //so_main_intermediario(quant);
    //==============================================
}

/* main_empresas recebe como parâmetro o nº de empresas a criar */
void main_empresas(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos empresas igual a quant através de um ciclo com i=0,1,...
	// após a criação de cada processo, o processo filho realiza duas atividades:
	// - chama a função empresas_executar passando como parâmetro o valor da variável de controlo do ciclo i=0,1,...
	// - chama a função exit passando como parâmetro o valor devolvido pela função empresas_executar
	// o processo pai guarda o pid do filho no vetor Ind.pid_empresas[i], com i=0,1,... e termina normalmente a função
    int pid;
	for(int i = 0; i<quant; i++){
		pid = fork();
		if(pid == -1){
			perror("pidError");
			exit(1);
		}
		//filho
		else if(pid == 0){
			exit(empresa_executar(i));
		}
		//pai
		else
		{
			Ind.pid_empresas[i] = pid;
		}
	}
    //so_main_empresas(quant);
    //==============================================
}

int main(int argc, char *argv[])
{
    char *ficEntrada = NULL;
    char *ficSaida = NULL;
    char *ficLog = NULL;
    long intervalo = 0;

    //==============================================
    // TRATAR PARÂMETROS DE ENTRADA
    // parâmetro obrigatório: file_configuracao
    // parâmetros opcionais:
    //   file_resultados
    //   -l file_log
    //   -t intervalo(us)    // us: microsegundos
    //
	// ignorar parâmetros desconhecidos
	// em caso de ausência de parâmetros escrever mensagem de como utilizar o programa e terminar
	// considerar que os parâmetros apenas são introduzidos na ordem indicada pela mensagem
	// considerar que são sempre introduzidos valores válidos para os parâmetros
    if(argc < 2){
		printf("Por favor use o programa da seguinte maneira: ./bin/soadmpor file_configuracao [file_resultados] -l [file_log] -t [intervalo(us)]\n");
		printf("Ex: ./bin/soperpro ../tests/in/scenario1 ../tests/out/scenario1 -l ../tests/log/scenario1.log -t 1000\n");
		exit(1);
	}

	ficEntrada = argv[1];

	if(argc > 2){
		ficSaida = argv[2];
	}

	//verificar o resto dos argumentos
	for(int i = 2; i<argc; i++){
		//verificar se estamos no parametro -l
		if(strcmp(argv[i], "-l")==0){
			//atribuir prox arg ao log
			ficLog = argv[i+1];
		}
		//verificar se estamos no parametro -t
		else if(strcmp(argv[i], "-t")==0){
			//atribuir prox arg ao intervalo, convertendo-o de string para int (atoi) e dar cast para long
			intervalo = (long) atoi(argv[i+1]);
		}
	}
    //intervalo = so_main_args(argc, argv, &ficEntrada, &ficSaida, &ficLog);
    //==============================================

    printf(
	"\n------------------------------------------------------------------------");
    printf(
	"\n----------------------------- SO_AdmPor ------------------------------");
    printf(
	"\n------------------------------------------------------------------------\n");

    // Ler dados de entrada
    file_begin(ficEntrada, ficSaida, ficLog);
    // criar zonas de memória e semáforos
    memory_create_buffers();
    prodcons_create_buffers();
    control_create();

    // Criar estruturas para indicadores e guardar capacidade inicial de portuaria
    memory_create_statistics();

    printf("\n\t\t\t*** Open SO_AdmPor ***\n\n");
    control_open_soadmpor();

    // Registar início de operação e armar alarme
    time_begin(intervalo);

    //
    // Iniciar sistema
    //

    // Criar INTERMEDIARIOS
    main_intermediario(Config.INTERMEDIARIO);
    // Criar EMPRESAS
    main_empresas(Config.EMPRESAS);
    // Criar CLIENTES
    main_cliente(Config.CLIENTES);

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR ESTATISTICAS
    //
    // esperar por cada cliente individualmente
	// se o cliente terminou normalmente
	// então testar se o valor do status é igual a OPERACOES
    //   se for igual entao nao atualizar as estatisticas
    //   se for inferior entao incrementar o indicador da respetiva servico (servico?) obtida
    // Ind.serviços_recebidos_por_clientes[OPERACAO], i=0,1,...
    int clientState = 0;
    int exitStatusOfClient;
	for(int i = 0; i<Config.CLIENTES; i++){
		waitpid(Ind.pid_clientes[i],&clientState, 0);
		exitStatusOfClient = WEXITSTATUS(clientState);
		if(clientState == -1){
            //nao atualizar estatisticas
			perror("clientState == -1");
		}

		if(exitStatusOfClient < Config.OPERATIONS){
            //incrementar o indicador da respetiva operacao obtida
			Ind.servicos_recebidos_por_clientes[exitStatusOfClient]++;
		}
	}
    //so_main_wait_clientes();
    //==============================================

    // Desarmar alarme
    time_destroy(intervalo);

    printf("\n\t\t\t*** Close SO_AdmPor ***\n\n");
    control_close_soadmpor();

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS INTERMEDIARIOS E ATUALIZAR INDICADORES
    //
    // esperar por cada intermediario individualmente
	// se a intermediario terminou normalmente
	// então atualizar o indicador de clientes atendidos
    // Ind.clientes_servidos_por_intermediarios[i], i=0,1,...
    // guardando nele o estado devolvido pela terminação do processo
    int intermediarioState = 0;
    int exitStatusOfIntermediario;
	for(int i = 0; i<Config.INTERMEDIARIO; i++){
		waitpid(Ind.pid_intermediarios[i], &intermediarioState,0);
		exitStatusOfIntermediario = WEXITSTATUS(intermediarioState);
		if(intermediarioState == -1){
			perror("intermediarioState == -1");
		}

		Ind.clientes_servidos_por_intermediarios[i]=exitStatusOfIntermediario;
	}
    //so_main_wait_intermediarios();
    //==============================================

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DAS EMPRESAS E ATUALIZAR INDICADORES
    //
    // esperar por cada empresa individualmente
	// se a empresa terminou normalmente
	// então atualizar o indicador de clientes atendidos
    // Ind.clientes_servidos_por_empresas[i], i=0,1,...
    // guardando nele o estado devolvido pela terminação do processo
    int empresaState = 0;
    int existStatusOfEmpresa;
	for(int i = 0; i<Config.EMPRESAS; i++){
		waitpid(Ind.pid_empresas[i], &empresaState,0);
		existStatusOfEmpresa = WEXITSTATUS(empresaState);
		if(empresaState == -1){
			perror("empresaState == -1");
		}

		Ind.clientes_servidos_por_empresas[i]=existStatusOfEmpresa;
	}
    //so_main_wait_empresas();
    //==============================================

    printf(
	"------------------------------------------------------------------------\n\n");
    printf("\t\t\t*** Statistics ***\n\n");
    so_write_statistics();
    printf("\t\t\t*******************\n");

    // destruir zonas de memória e semáforos
    file_destroy();
    control_destroy();
    prodcons_destroy();
    memory_destroy_all();

    return 0;
}
