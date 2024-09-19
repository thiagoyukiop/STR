/*
Arquivo: main_executivo_ciclico.c
Autor: Thiago Yukio Horita Pacheco e Larissa de Sousa Gouvea
Função do arquivo: Software requisitado, em uma atividade, para um fábrica de automóveis por meio de uma versão executivo cíclico
Criado em 19 de setembro de 2024
Modificado em 19 de setembro de 2024

*/
#include <stdio.h>
#include <unistd.h> // para usar sleep


// Definir tempos de ciclo em milissegundos
#define PERIODO_DISPLAY 1000        // Atualização do display a cada 1 segundo
#define PERIODO_CINTO_SEGURANCA 1000 // Cinto de segurança a cada 1 segundo
#define PERIODO_ABS 100             // ABS a cada 100 ms
#define PERIODO_AIRBAG 100          // Airbag a cada 100 ms
#define PERIODO_TEMPERATURA 20      // Temperatura do motor a cada 20 ms
#define PERIODO_INJECAO 0.5         // Injeção eletrônica a cada 0.5 ms


// Funções simulando cada subsistema
void atualiza_injecao_eletronica() {
    printf("Atualizando injeção eletrônica...\n");
}


void monitora_temperatura_motor() {
    printf("Monitorando temperatura do motor...\n");
}


void monitora_abs() {
    printf("Monitorando ABS...\n");
}


void monitora_airbag() {
    printf("Monitorando airbag...\n");
}


void monitora_cinto_seguranca() {
    printf("Monitorando cinto de segurança...\n");
}


void atualiza_display() {
    printf("Atualizando display...\n");
}


// Função para simular espera em milissegundos
void wait_ms(int ms) {
    usleep(ms * 1000); // Usar usleep para milissegundos
}


void app_main() {
    int timer_injecao = 0;
    int timer_temperatura = 0;
    int timer_abs = 0;
    int timer_airbag = 0;
    int timer_cinto = 0;
    int timer_display = 0;


    while (1) {
        // Incrementa timers (simulação de temporização)
        timer_injecao += 1;
        timer_temperatura += 1;
        timer_abs += 1;
        timer_airbag += 1;
        timer_cinto += 1;
        timer_display += 1;


        // Executar tarefas dentro do período designado


        // Injeção eletrônica (executar a cada 0.5 ms)
        if (timer_injecao >= PERIODO_INJECAO) {
            atualiza_injecao_eletronica();
            timer_injecao = 0;
        }


        // Temperatura do motor (executar a cada 20 ms)
        if (timer_temperatura >= PERIODO_TEMPERATURA) {
            monitora_temperatura_motor();
            timer_temperatura = 0;
        }


        // ABS (executar a cada 100 ms)
        if (timer_abs >= PERIODO_ABS) {
            monitora_abs();
            timer_abs = 0;
        }


        // Airbag (executar a cada 100 ms)
        if (timer_airbag >= PERIODO_AIRBAG) {
            monitora_airbag();
            timer_airbag = 0;
        }


        // Cinto de segurança (executar a cada 1 segundo)
        if (timer_cinto >= PERIODO_CINTO_SEGURANCA) {
            monitora_cinto_seguranca();
            timer_cinto = 0;
        }


        // Atualizar o display (executar a cada 1 segundo)
        if (timer_display >= PERIODO_DISPLAY) {
            atualiza_display();
            timer_display = 0;
        }


        // Aguarda 1 ms antes de repetir o ciclo
        wait_ms(1);
    }
}


