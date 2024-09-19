/*
Arquivo: main_principal.c
Autor: Thiago Yukio Horita Pacheco e Larissa de Sousa Gouvea
Função do arquivo: Software requisitado, em uma atividade, para um fábrica de automóveis
Criado em 19 de setembro de 2024
Modificado em 19 de setembro de 2024

*/

// Bibliotecas necessárias para executar programa
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_timer.h"

// Definir os pinos dos sensores
#define SENSOR_INJECAO_PIN 32  // GPIO para sensor de injeção eletrônica
#define SENSOR_TEMPERATURA_PIN 33  // GPIO para sensor de temperatura do motor
#define SENSOR_ABS_PIN 34  // GPIO para sensor de ABS
#define SENSOR_AIRBAG_PIN 35  // GPIO para sensor de airbag
#define SENSOR_CINTO_PIN 36  // GPIO para sensor de cinto de segurança
#define SENSOR_VELOCIDADE_PIN 37 // Exemplo de GPIO para sensor de velocidade
#define SENSOR_CONSUMO_PIN 38 // Exemplo de GPIO para sensor de consumo

// Definir os tempos de resposta em milissegundos
#define TEMPO_INJECAO_MS 15 // 500 us = 0.5 ms
#define TEMPO_TEMPERATURA_MS 20 // 20 ms
#define TEMPO_ABS_MS 100 // 100 ms
#define TEMPO_AIRBAG_MS 100 // 100 ms
#define TEMPO_CINTO_MS 1000 // 1 segundo = 1000 ms
#define TEMPO_VELOCIDADE_MS 100 // Intervalo de amostragem para velocidade
#define TEMPO_CONSUMO_MS 100 // Intervalo de amostragem para consumo

#define AMOSTRAS 200 // Número de amostras para cálculo da média

static bool motor_ativo = false;
static bool frenagem_ativo = false;
static bool vida_ativa = false;

static float amostras_velocidade[AMOSTRAS];
static float amostras_consumo[AMOSTRAS];
static int contagem_velocidade = 0;
static int contagem_consumo = 0;
static float velocidade_media = 0;
static float consumo_media = 0;

// Configuração dos sensores
void configurar_sensores() {
    // Configurar os pinos dos sensores como entradas
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


void monitoramento_injecao(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_INJECAO_PIN)) {

            int64_t start_time = esp_timer_get_time();

            motor_ativo = true;
            printf("\033[32mInjeção eletrônica acionada!\033[0m\n");

            int64_t end_time = esp_timer_get_time();  // Captura o tempo após a ação
            printf("\033[32mTempo da Injeção Eletrônica: %lld μs\033[0m\n", end_time - start_time);

            // Aguarda por 500 μs
            // while ((esp_timer_get_time() - start_time) < 500);
        }

        vTaskDelay(pdMS_TO_TICKS(TEMPO_INJECAO_MS)); // Atraso para simulação
        // vTaskDelay(pdUS_TO_TICKS(TEMPO_INJECAO_MS));
    }
}

// Função para monitorar o sensor de temperatura do motor
void monitoramento_temperatura(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_TEMPERATURA_PIN)) {
            int64_t start_time = esp_timer_get_time();
            motor_ativo = true;
            printf("\033[31mTemperatura do motor acima do limite!\033[0m\n");
            // printf("Temperatura do motor acima do limite!\n");
            int64_t end_time = esp_timer_get_time();  // Captura o tempo de fim
            printf("\033[31mTempo da Temperatura: %lld μs\033[0m\n", end_time - start_time);
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_TEMPERATURA_MS)); // Atraso de acordo com o deadline da temperatura
    }
}

// Função para monitorar o sensor de ABS
void monitoramento_abs(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_ABS_PIN)) {
            int64_t start_time = esp_timer_get_time();
            frenagem_ativo = true;
            // printf("ABS acionado!\n");
            printf("\033[34mABS acionado!\033[0m\n");
            int64_t end_time = esp_timer_get_time();  // Captura o tempo de fim
            printf("\033[34mTempo da ABS: %lld μs\033[0m\n", end_time - start_time);
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_ABS_MS)); // Atraso de acordo com o deadline do ABS
    }
}

// Função para monitorar o sensor de airbag
void monitoramento_airbag(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_AIRBAG_PIN)) {
            int64_t start_time = esp_timer_get_time();
            vida_ativa = true;
            // printf("Airbag acionado!\n");
            printf("\033[35mAirbag acionado!\033[0m\n");
            int64_t end_time = esp_timer_get_time();  // Captura o tempo de fim
            printf("\033[35mTempo da Airbag: %lld μs\033[0m\n", end_time - start_time);
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_AIRBAG_MS)); // Atraso de acordo com o deadline do airbag
    }
}

// Função para monitorar o sensor de cinto de segurança
void monitoramento_cinto(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_CINTO_PIN)) {
            int64_t start_time = esp_timer_get_time();
            vida_ativa = true;
            // printf("Cinto de segurança acionado!\n");
            printf("\033[36mCinto de segurança acionado!\033[0m\n");
            int64_t end_time = esp_timer_get_time();  // Captura o tempo de fim
            printf("\033[36mTempo da cinto: %lld μs\033[0m\n", end_time - start_time);
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_CINTO_MS)); // Atraso de acordo com o deadline do cinto de segurança
    }
}

void monitoramento_velocidade(void *pvParameter) {
    while (1) {
        if (contagem_velocidade < AMOSTRAS) {
            // Simular leitura de velocidade do sensor
            float velocidade = (float)(rand() % 100); // Exemplo de velocidade aleatória
            amostras_velocidade[contagem_velocidade++] = velocidade;
        } else {
            // Calcular a média quando atingir 200 amostras
            float soma = 0;
            for (int i = 0; i < AMOSTRAS; i++) {
                soma += amostras_velocidade[i];
            }
            velocidade_media = soma / AMOSTRAS;
            contagem_velocidade = 0; // Resetar contagem
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_VELOCIDADE_MS));
    }
}


void monitoramento_consumo(void *pvParameter) {
    while (1) {
        if (contagem_consumo < AMOSTRAS) {
            // Simular leitura de consumo do sensor
            float consumo = (float)(rand() % 15); // Exemplo de consumo aleatório
            amostras_consumo[contagem_consumo++] = consumo;
        } else {
            // Calcular a média quando atingir 200 amostras
            float soma = 0;
            for (int i = 0; i < AMOSTRAS; i++) {
                soma += amostras_consumo[i];
            }
            consumo_media = soma / AMOSTRAS;
            contagem_consumo = 0; // Resetar contagem
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_CONSUMO_MS));
    }
}

// Função para atualizar o estado dos subsistemas
void atualizar_display(void *pvParameter) {
    while (1) {
        printf("Estado dos subsistemas:\n");
        printf("Motor: %s\n", motor_ativo ? "Ativo" : "Inativo");
        printf("Frenagem: %s\n", frenagem_ativo ? "Ativo" : "Inativo");
        printf("Vida: %s\n", vida_ativa ? "Ativo" : "Inativo");
        printf("Velocidade média: %.2f km/h\n", velocidade_media);
        printf("Consumo médio: %.2f L/100km\n", consumo_media);

        // Reseta o estado dos subsistemas para o próximo ciclo
        motor_ativo = false;
        frenagem_ativo = false;
        vida_ativa = false;

        vTaskDelay(pdMS_TO_TICKS(1000));  // Atualiza a cada 1 segundo
    }
}

// Função principal
void app_main() {

    // Configuração dos sensores
    configurar_sensores();

    // Criação das tarefas de monitoramento dos sensores com prioridades baseadas nos deadlines
    xTaskCreate(monitoramento_injecao, "monitoramento_injecao", 2048, NULL, 6, NULL); // Alta prioridade
    xTaskCreate(monitoramento_temperatura, "monitoramento_temperatura", 2048, NULL, 5, NULL); // Prioridade média-alta
    xTaskCreate(monitoramento_abs, "monitoramento_abs", 2048, NULL, 3, NULL); // Prioridade média
    xTaskCreate(monitoramento_airbag, "monitoramento_airbag", 2048, NULL, 4, NULL); // Prioridade média-alta
    xTaskCreate(monitoramento_cinto, "monitoramento_cinto", 2048, NULL, 2, NULL); // Prioridade baixa
    xTaskCreate(atualizar_display, "atualizar_display", 2048, NULL, 1, NULL); // Prioridade mais baixa
    xTaskCreate(monitoramento_velocidade, "monitoramento_velocidade", 2048, NULL, 2, NULL); // Prioridade baixa
    xTaskCreate(monitoramento_consumo, "monitoramento_consumo", 2048, NULL, 2, NULL); // Prioridade baixa
}
