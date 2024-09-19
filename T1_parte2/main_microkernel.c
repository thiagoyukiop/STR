/*
Arquivo: main_microkeernel.c
Autor: Thiago Yukio Horita Pacheco e Larissa de Sousa Gouvea
Função do arquivo: Software requisitado, em uma atividade, para um fábrica de automóveis com uma versão com microkernel
Criado em 19 de setembro de 2024
Modificado em 19 de setembro de 2024

*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"


// Definir os pinos dos sensores
#define SENSOR_INJECAO_PIN 32  
#define SENSOR_TEMPERATURA_PIN 33  
#define SENSOR_ABS_PIN 34  
#define SENSOR_AIRBAG_PIN 35  
#define SENSOR_CINTO_PIN 36  
#define SENSOR_VELOCIDADE_PIN 37
#define SENSOR_CONSUMO_PIN 38


// Definir os tempos de resposta em milissegundos
#define TEMPO_INJECAO_MS 15
#define TEMPO_TEMPERATURA_MS 20
#define TEMPO_ABS_MS 100
#define TEMPO_AIRBAG_MS 100
#define TEMPO_CINTO_MS 1000
#define TEMPO_VELOCIDADE_MS 100
#define TEMPO_CONSUMO_MS 100


#define AMOSTRAS 200


// Estruturas para mensagens
typedef struct {
    float velocidade;
    float consumo;
} SensorData;


QueueHandle_t queue_sensor_data;


// Prototipos das funções
void monitoramento_injecao(void *pvParameter);
void monitoramento_temperatura(void *pvParameter);
void monitoramento_abs(void *pvParameter);
void monitoramento_airbag(void *pvParameter);
void monitoramento_cinto(void *pvParameter);
void monitoramento_velocidade(void *pvParameter);
void monitoramento_consumo(void *pvParameter);
void atualizar_display(void *pvParameter);


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


// Funções de monitoramento (simplificadas)
void monitoramento_injecao(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_INJECAO_PIN)) {
            printf("Injeção eletrônica acionada!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_INJECAO_MS));
    }
}


void monitoramento_temperatura(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_TEMPERATURA_PIN)) {
            printf("Temperatura do motor acima do limite!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_TEMPERATURA_MS));
    }
}


void monitoramento_abs(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_ABS_PIN)) {
            printf("ABS acionado!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_ABS_MS));
    }
}


void monitoramento_airbag(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_AIRBAG_PIN)) {
            printf("Airbag acionado!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_AIRBAG_MS));
    }
}


void monitoramento_cinto(void *pvParameter) {
    while (1) {
        if (gpio_get_level(SENSOR_CINTO_PIN)) {
            printf("Cinto de segurança acionado!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_CINTO_MS));
    }
}


void monitoramento_velocidade(void *pvParameter) {
    float amostras_velocidade[AMOSTRAS] = {0};
    int contagem_velocidade = 0;


    while (1) {
        if (contagem_velocidade < AMOSTRAS) {
            float velocidade = (float)(rand() % 100); // Simulação de leitura de velocidade
            amostras_velocidade[contagem_velocidade++] = velocidade;
        } else {
            float soma = 0;
            for (int i = 0; i < AMOSTRAS; i++) {
                soma += amostras_velocidade[i];
            }
            SensorData data;
            data.velocidade = soma / AMOSTRAS;
            xQueueSend(queue_sensor_data, &data, portMAX_DELAY);
            contagem_velocidade = 0; // Resetar contagem
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_VELOCIDADE_MS));
    }
}


void monitoramento_consumo(void *pvParameter) {
    float amostras_consumo[AMOSTRAS] = {0};
    int contagem_consumo = 0;


    while (1) {
        if (contagem_consumo < AMOSTRAS) {
            float consumo = (float)(rand() % 15); // Simulação de leitura de consumo
            amostras_consumo[contagem_consumo++] = consumo;
        } else {
            float soma = 0;
            for (int i = 0; i < AMOSTRAS; i++) {
                soma += amostras_consumo[i];
            }
            SensorData data;
            data.consumo = soma / AMOSTRAS;
            xQueueSend(queue_sensor_data, &data, portMAX_DELAY);
            contagem_consumo = 0; // Resetar contagem
        }
        vTaskDelay(pdMS_TO_TICKS(TEMPO_CONSUMO_MS));
    }
}


void atualizar_display(void *pvParameter) {
    SensorData data;
    while (1) {
        if (xQueueReceive(queue_sensor_data, &data, portMAX_DELAY)) {
            printf("Velocidade média: %.2f km/h\n", data.velocidade);
            printf("Consumo médio: %.2f L/100km\n", data.consumo);
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Atualiza a cada 1 segundo
    }
}


void app_main() {
    queue_sensor_data = xQueueCreate(10, sizeof(SensorData));


    configurar_sensores();


    xTaskCreate(monitoramento_injecao, "monitoramento_injecao", 2048, NULL, 6, NULL);
    xTaskCreate(monitoramento_temperatura, "monitoramento_temperatura", 2048, NULL, 5, NULL);
    xTaskCreate(monitoramento_abs, "monitoramento_abs", 2048, NULL, 4, NULL);
    xTaskCreate(monitoramento_airbag, "monitoramento_airbag", 2048, NULL, 3, NULL);
    xTaskCreate(monitoramento_cinto, "monitoramento_cinto", 2048, NULL, 2, NULL);
    xTaskCreate(monitoramento_velocidade, "monitoramento_velocidade", 2048, NULL, 1, NULL);
    xTaskCreate(monitoramento_consumo, "monitoramento_consumo", 2048, NULL, 1, NULL);
    xTaskCreate(atualizar_display, "atualizar_display", 2048, NULL, 1, NULL);
}
