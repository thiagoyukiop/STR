/*
Arquivo: main_laco.c
Autor: Thiago Yukio Horita Pacheco e Larissa de Sousa Gouvea
Função do arquivo: Software requisitado, em uma atividade, para um fábrica de automóveis com uma versão com laço de repetição com tratador de interrupções
Criado em 19 de setembro de 2024
Modificado em 19 de setembro de 2024

*/
#include <stdio.h>
#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


// Definir os pinos dos sensores
#define SENSOR_INJECAO_PIN 32
#define SENSOR_TEMPERATURA_PIN 33
#define SENSOR_ABS_PIN 34
#define SENSOR_AIRBAG_PIN 35
#define SENSOR_CINTO_PIN 36


// Definir os tempos de resposta em milissegundos
#define TEMPO_INJECAO_MS 15
#define TEMPO_TEMPERATURA_MS 20
#define TEMPO_ABS_MS 100
#define TEMPO_AIRBAG_MS 100
#define TEMPO_CINTO_MS 1000


static bool motor_ativo = false;
static bool frenagem_ativo = false;
static bool vida_ativa = false;


// Configuração dos sensores
void configurar_sensores() {
    esp_rom_gpio_pad_select_gpio(SENSOR_INJECAO_PIN);
    gpio_set_direction(SENSOR_INJECAO_PIN, GPIO_MODE_INPUT);


    esp_rom_gpio_pad_select_gpio(SENSOR_TEMPERATURA_PIN);
    gpio_set_direction(SENSOR_TEMPERATURA_PIN, GPIO_MODE_INPUT);


    esp_rom_gpio_pad_select_gpio(SENSOR_ABS_PIN);
    gpio_set_direction(SENSOR_ABS_PIN, GPIO_MODE_INPUT);


    esp_rom_gpio_pad_select_gpio(SENSOR_AIRBAG_PIN);
    gpio_set_direction(SENSOR_AIRBAG_PIN, GPIO_MODE_INPUT);


    esp_rom_gpio_pad_select_gpio(SENSOR_CINTO_PIN);
    gpio_set_direction(SENSOR_CINTO_PIN, GPIO_MODE_INPUT);
}


void monitoramento_injecao() {
    if (gpio_get_level(SENSOR_INJECAO_PIN)) {
        motor_ativo = true;
        printf("Injeção eletrônica acionada!\n");
    }
}


void monitoramento_temperatura() {
    if (gpio_get_level(SENSOR_TEMPERATURA_PIN)) {
        motor_ativo = true;
        printf("Temperatura do motor acima do limite!\n");
    }
}


void monitoramento_abs() {
    if (gpio_get_level(SENSOR_ABS_PIN)) {
        frenagem_ativo = true;
        printf("ABS acionado!\n");
    }
}


void monitoramento_airbag() {
    if (gpio_get_level(SENSOR_AIRBAG_PIN)) {
        vida_ativa = true;
        printf("Airbag acionado!\n");
    }
}


void monitoramento_cinto() {
    if (gpio_get_level(SENSOR_CINTO_PIN)) {
        vida_ativa = true;
        printf("Cinto de segurança acionado!\n");
    }
}


void atualizar_display() {
    printf("Estado dos subsistemas:\n");
    printf("Motor: %s\n", motor_ativo ? "Ativo" : "Inativo");
    printf("Frenagem: %s\n", frenagem_ativo ? "Ativo" : "Inativo");
    printf("Vida: %s\n", vida_ativa ? "Ativo" : "Inativo");


    // Reseta o estado dos subsistemas para o próximo ciclo
    motor_ativo = false;
    frenagem_ativo = false;
    vida_ativa = false;
    vTaskDelay(pdMS_TO_TICKS(1000)); // Ajuste o tempo conforme necessário
}


void app_main() {
    configurar_sensores();


    while (1) {
        monitoramento_injecao();
        monitoramento_temperatura();
        monitoramento_abs();
        monitoramento_airbag();
        monitoramento_cinto();
       
        atualizar_display();


        // Atraso para evitar sobrecarga da CPU e permitir que outras tarefas sejam executadas
        vTaskDelay(pdMS_TO_TICKS(100)); // Ajuste o tempo conforme necessário
    }
}
