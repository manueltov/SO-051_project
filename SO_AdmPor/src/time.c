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
#include <ctype.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "sotime.h"

struct timespec t_inicial;
long intervalo_alarme;

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct timespec t_inicial;
long intervalo_alarme;

void time_begin(long intervalo) {
    //==============================================
    // INICIAR ESTRUTURA t_inicial COM VALOR DE RELOGIO (CLOCK_REALTIME)
	//
	// funções de tempo:
	// - clock_gettime() dá um resultado em nanosegundos
	// - gettimeofday()  dá um resultado em milisegundos
	// como a função clock_gettime() dá um valor mais preciso do que gettimeofday()
	// deve ser usada clock_gettime()
	//
    // fazer:
	// - se intervalo!=0 então intervalo_alarme = intervalo
	// - se intervalo!=0 então chamar time_setup_alarm();
	// - iniciar estrutura t_inicial com clock_gettime usando CLOCK_REALTIME
    so_time_begin(intervalo);
    //==============================================
}

void time_destroy(long intervalo)
{
    //==============================================
    // DESATIVAR ALARME
    //
	// ignorar SIGALRM
    so_time_destroy(intervalo);
    //==============================================
}

void time_setup_alarm() {
    //==============================================
    // ARMAR ALARME DE ACORDO COM intervalo_alarme (SIGNAL E SETTIMER)
    //
	// fazer:
	// - associar SIGALRM com a função time_write_log_timed
	// - usar setitimer preenchendo apenas os campos value da estrutura
    so_time_setup_alarm();
    //==============================================
}

void time_write_log_timed(int signum) {
    //==============================================
    // ESCREVER LOG NO ECRAN DE FORMA TEMPORIZADA 
    //
	// rearmar alarme chamando novamente time_setup_alarm
	// escrever para o ecrã a informação esperada
    so_time_write_log_timed(signum);
    //==============================================
}

double time_difference(struct timespec t1, struct timespec t2) {
    //==============================================
    // CALCULAR A DIFERENCA, EM NANOSEGUNDOS, ENTRE t1 E t2
    // o resultado deve estar em segundos representado como um double
	// realizar as operações aritméticas necessárias para obter o resultado
    return so_time_difference(t1,t2);
    //==============================================
}

double time_untilnow() {
    //==============================================
    // CALCULAR O INTERVALO DE TEMPO ENTRE t_inicial E O INSTANTE ATUAL
    //
	// fazer:
	// - obter o tempo atual com clock_gettime
	// - chamar time_difference
    return so_time_untilnow();
    //==============================================
}

void time_register(struct timespec *t) {
    //==============================================
    // REGISTAR O TEMPO ATUAL EM t (CLOCK_REALTIME)
    //
	// usar clock_gettime com CLOCK_REALTIME
    so_time_register(t);
    //==============================================
}

void time_processing_order() {
	//==============================================
	// ADORMECER UM NUMERO ALEATORIO DE SEGUNDOS
	// MAXIMO DE 100 MILISEGUNDOS E MINIMO 1 MILISEGUNDO
	//
	// usar funcoes usleep, rand e % (resto divisao inteira)
	so_time_processing_order();
	//==============================================
}

float time_average_statistic(int id) {
	//==============================================================
	// CALCULAR MEDIA DOS TEMPOS DOS PEDIDOS DO CLIENTE
	//
	// percorrer Ind.tempos com duplo for, usando Config.CLIENTES
	// e Config.N para indexar, ver quais os tempos do cliente id e
	// fazer a media dos que são != 0. Se o cliente não tiver tempos,
	// devolver 0.
	return so_time_average_statistic(id);
	//==============================================
}

float time_minimum_statistic(int id) {
	//==============================================================
	// CALCULAR MINIMO DOS TEMPOS DOS PEDIDOS DO CLIENTE
	//
	// percorrer Ind.tempos com duplo for, usando Config.CLIENTES
	// e Config.N para indexar, ver quais os tempos do cliente id e
	// fazer o minimo dos que são != 0. Se o cliente não tiver tempos,
	// devolver 0.
	return so_time_minimum_statistic(id);
	//==============================================
}

float time_maximum_statistic(int id) {
	//==============================================================
	// CALCULAR MAXIMO DO TEMPO DOS PEDIDOS DO CLIENTE
	//
	// percorrer Ind.tempos com duplo for, usando Config.CLIENTES
	// e Config.N para indexar, ver quais os tempos do cliente id e
	// fazer o maximo dos que são != 0. Se o cliente não tiver tempos,
	// devolver 0.
	return so_time_maximum_statistic(id);
	//==============================================
}
