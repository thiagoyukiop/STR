#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>

#include <bits/pthread_stack_min-dynamic.h>

#include <stdbool.h>
#include <stdint.h>

#include <inttypes.h>

#include <glib.h>

#define DEADLINE_ABS 100000 // 100 ms
#define DEADLINE_AIRBAG 100000 // 100 ms

volatile bool SENSOR_ABS = false;
volatile bool SENSOR_AIRBAG = false;

volatile clock_t WCET_ABS = 0;
volatile clock_t WCET_AIRBAG = 0;

volatile clock_t WCRT_ABS = 0;
volatile clock_t WCRT_AIRBAG = 0;

volatile clock_t TEMPO_MEDIO_ABS = 0;
volatile clock_t TEMPO_MEDIO_AIRBAG = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutexattr_t  mutexattr_prioceiling;

GList *ABS_list = NULL;

GList *AIRBAG_list = NULL;

int kbhit() {
    struct termios original, t;
    int ch, oldf;

    // Salva as configurações atuais do terminal
    tcgetattr(STDIN_FILENO, &original);
    t = original;
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &original);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

// Função para configurar o terminal para modo não bloqueante
void set_input_mode(void) {
    struct termios tattr;
    tcgetattr(STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON | ECHO); // Desabilita o modo canônico e o echo
    tattr.c_cc[VMIN] = 1; // Define que 1 caractere mínimo deve ser lido
    tattr.c_cc[VTIME] = 0; // Não há tempo de espera
    tcsetattr(STDIN_FILENO, TCSANOW, &tattr);
}

// Função para restaurar o modo original do terminal
void reset_input_mode(void) {
    struct termios tattr;
    tcgetattr(STDIN_FILENO, &tattr);
    tattr.c_lflag |= (ICANON | ECHO); // Restaura o modo canônico e o echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tattr);
}

void *thread_obter_tecla_func(void *data) {
    char tecla;
    while(1) {
        if(kbhit()) {
            tecla = getchar();
            if(tecla == 'b') {
                pthread_mutex_lock(&mutex);
                if(SENSOR_AIRBAG == false) {
                    SENSOR_AIRBAG = true;
                    printf("\033[35mSensor Airbag ativado!\033[0m\n");
                }
                pthread_mutex_unlock(&mutex);
            }
            else if(tecla == 'a') {
                pthread_mutex_lock(&mutex);
                if(SENSOR_ABS == false) {
                    SENSOR_ABS = true;
                    printf("\033[34mSensor ABS ativado!\033[0m\n");
                }
                pthread_mutex_unlock(&mutex);
            }
        }
        usleep(10000);
    }
}

void *thread_abs_func(void *data) {
    clock_t start_time, end_time;  // Variáveis para medir o tempo
    uint64_t execution_time, total_time, total_execution_time = 0;
    const int m = 8, k = 10;
    int count_m = 0, count = 0, aux, deadline_ultrapassado = 0, ativacoes_totais = 0;
    char target_key = 'a';
    double fator_skip;

    printf("Thread ABS iniciada! Pressione '%c' para ver a mensagem.\n", target_key);

    while (1) {
        // Verifica se uma tecla foi pressionada

        if(SENSOR_ABS) {
            // Marcar o tempo de início com clock()
            start_time = clock();

            count++;
            ativacoes_totais++;

            // Simulação de algumas tarefas (delays)
            usleep(26000);

            // Calcular o tempo de execução em segundos
            execution_time = (uint64_t)(((double)(clock() - start_time) / CLOCKS_PER_SEC) * 1e6);

            // Exibir o tempo de execução em segundos
            printf("\033[34m%d:Tempo de execução do ABS: %" PRIu64 " us\033[0m\n", count, execution_time);

            if (execution_time > DEADLINE_ABS) {
                deadline_ultrapassado++;
                printf("\033[34mTempo de execução do ABS passou do Deadline\033[0m\n");
                // ESTÁ DENTRO DOS LIMITES? 
                ABS_list = g_list_append(ABS_list, GINT_TO_POINTER(false));

            }
            else {
                ABS_list = g_list_append(ABS_list, GINT_TO_POINTER(true));
            }

            pthread_mutex_lock(&mutex);

            total_execution_time += execution_time;

            TEMPO_MEDIO_ABS = total_execution_time / count;

            if (execution_time > WCET_ABS) {
                WCET_ABS = execution_time;
            }

            // TEMPO_INTERFERENCIA_ABS = HWM_AIRBAG;

            total_time = (uint64_t)(((double)(clock() - start_time) / CLOCKS_PER_SEC) * 1e6);
            if(total_time > WCRT_ABS) {
                WCRT_ABS = total_time;
            }

            SENSOR_ABS = false;

            if(count % 20 == 0) {
                // Cálculo do fator skip
                fator_skip = (double)deadline_ultrapassado / ativacoes_totais;
                printf("\033[34mFator Skip do ABS: %.2f\n\033[0m", fator_skip);

                // Resetando contadores a cada 20 ativações
                deadline_ultrapassado = 0;
                ativacoes_totais = 0;
                count_m = 0;
                aux = 0;
                for (GList *l = ABS_list; l != NULL && aux < 10; l = l->next) {
                    gboolean value = GPOINTER_TO_INT(l->data);
                    if(!value) {
                        count_m++;
                    }
                    aux++;
                }
                if(count_m > m) {
                    printf("\033[34mPrimeiros 10 valores, fora do limite\033[0m\n");
                } 
                else {
                    printf("\033[34mPrimeiros 10 valores, dentro do limite\033[0m\n");
                }
                
                count_m = 0;
                aux = 0;

                for (GList *l = g_list_nth(ABS_list, 10); l != NULL && aux < 10; l = l->next) {
                    gboolean value = GPOINTER_TO_INT(l->data);
                    if(!value) {
                        count_m++;
                    }
                    aux++;
                }
                if(count_m > m) {
                    printf("\033[34mÚltimos 10 valores, fora do limite\033[0m\n");
                } 
                else {
                    printf("\033[34mÚltimos 10 valores, dentro do limite\033[0m\n");
                }
                g_list_free(ABS_list);
                ABS_list = NULL;
            }
            pthread_mutex_unlock(&mutex);
        }
        usleep(1000);  // Delay de 1ms
    }
}


void *thread_airbag_func(void *data) {
    clock_t start_time, end_time;  // Variáveis para medir o tempo
    uint64_t execution_time , total_time, total_execution_time = 0;
    const int m = 9, k = 10;
    int count_m = 0, count = 0, aux, deadline_ultrapassado = 0, ativacoes_totais = 0;
    char target_key = 'b';
    double fator_skip;

    printf("Thread Airbag iniciada! Pressione '%c' para ver a mensagem.\n", target_key);

    while (1) {
        // Verifica se uma tecla foi pressionada

        if(SENSOR_AIRBAG) {
            // Marcar o tempo de início com clock()
            start_time = clock();

            count++;
            ativacoes_totais++;

            // Simulação de algumas tarefas (delays)
            usleep(26000);

            // Calcular o tempo de execução em segundos
            execution_time = (uint64_t)(((double)(clock() - start_time) / CLOCKS_PER_SEC) * 1e6);

            // Exibir o tempo de execução em microsegundos
            printf("\033[35m%d:Tempo de execução do Airbag: %" PRIu64 " us\033[0m\n", count, execution_time);

            if (execution_time > DEADLINE_AIRBAG) {
                deadline_ultrapassado++;
                printf("\033[35mTempo de execução do Airbag Passou do Deadline\033[0m\n");
                // ESTÁ DENTRO DOS LIMITES? 
                AIRBAG_list = g_list_append(AIRBAG_list, GINT_TO_POINTER(false));
            }
            else {
                AIRBAG_list = g_list_append(AIRBAG_list, GINT_TO_POINTER(true));
            }

            pthread_mutex_lock(&mutex);

            total_execution_time += execution_time;

            TEMPO_MEDIO_AIRBAG = total_execution_time / count;

            if (execution_time > WCET_AIRBAG) {
                WCET_AIRBAG = execution_time;
            }

            // TEMPO_INTERFERENCIA_AIRBAG = 0;

            total_time = (uint64_t)(((double)(clock() - start_time) / CLOCKS_PER_SEC) * 1e6);
            if(total_time > WCRT_AIRBAG) {
                WCRT_AIRBAG = total_time;
            }

            SENSOR_AIRBAG = false;
            
            if(count % 20 == 0) {
                // Cálculo do fator skip
                fator_skip = (double)deadline_ultrapassado / ativacoes_totais;
                printf("\033[35mFator Skip do Airbag: %.2f\n\033[0m", fator_skip);

                // Resetando contadores a cada 20 ativações
                deadline_ultrapassado = 0;
                ativacoes_totais = 0;
                count_m = 0;
                aux = 0;
                for (GList *l = AIRBAG_list; l != NULL && aux < 10; l = l->next) {
                    gboolean value = GPOINTER_TO_INT(l->data);
                    if(!value) {
                        count_m++;
                    }
                    aux++;
                }
                if(count_m > m) {
                    printf("\033[35mPrimeiros 10 valores, fora do limite\033[0m\n");
                } 
                else {
                    printf("\033[35mPrimeiros 10 valores, dentro do limite\033[0m\n");
                }
                
                count_m = 0;

                for (GList *l = g_list_nth(AIRBAG_list, 10); l != NULL && aux < 20; l = l->next) {
                    gboolean value = GPOINTER_TO_INT(l->data);
                    if(!value) {
                        count_m++;
                    }
                    aux++;
                }
                if(count_m > m) {
                    printf("\033[35mÚltimos 10 valores, fora do limite\033[0m\n");
                } 
                else {
                    printf("\033[35mÚltimos 10 valores, dentro do limite\033[0m\n");
                }
                g_list_free(AIRBAG_list);
                AIRBAG_list = NULL;
            }
            pthread_mutex_unlock(&mutex);
        }
        usleep(1000);  // Delay de 1ms
    }
}

void *thread_display_func(void *data) {
    int count = 0;
    while (1) {
        pthread_mutex_lock(&mutex);
        printf("\033[34m%d:Tempo Médio de Execução do ABS: %" PRIu64 " us\033[0m\n", count, TEMPO_MEDIO_ABS);
        printf("\033[34m%d:WCET do ABS: %" PRIu64 " us\033[0m\n", count, WCET_ABS);
        printf("\033[34m%d:WCRT do ABS: %" PRIu64 " us\033[0m\n", count, WCRT_ABS);
        printf("\033[35m%d:Tempo Médio de Execução do Airbag: %" PRIu64 " us\033[0m\n", count, TEMPO_MEDIO_AIRBAG);
        printf("\033[35m%d:WCET do Airbag: %" PRIu64 " us\033[0m\n", count, WCET_AIRBAG);
        printf("\033[35m%d:WCRT do Airbag: %" PRIu64 " us\033[0m\n", count, WCRT_AIRBAG);
        count++;
        pthread_mutex_unlock(&mutex);
        sleep(1);  // Atualiza a cada 1 segundo
    }
}

int init_pthread_attributes(pthread_attr_t *attr, struct sched_param *param, int policy, int priority) {
    int ret = pthread_attr_init(attr);
    if (ret) {
        printf("Falha ao inicializar atributos do pthread\n");
        return ret;
    }

    pthread_attr_setschedpolicy(attr, policy);
    param->sched_priority = priority;
    pthread_attr_setschedparam(attr, param);
    pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);

    return 0;
}

int create_thread(pthread_t *thread, pthread_attr_t *attr, void *(*func)(void *), void *arg, const char *name) {
    int ret = pthread_create(thread, attr, func, arg);
    if (ret) {
        printf("Falha ao criar thread %s\n", name);
    }
    return ret;
}

int main(int argc, char *argv[]) {
    pthread_attr_t attr_abs, attr_airbag, attr_display, attr_obter_tecla;
    pthread_t thread_abs, thread_airbag, thread_display, thread_obter_tecla;
    struct sched_param param_abs, param_airbag, param_display, param_obter_tecla;
    int ret;

    set_input_mode();

    /* Lock memory */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        printf("mlockall falhou: %m\n");
        exit(-2);
    }

    /* Inicializar atributos */
    ret = init_pthread_attributes(&attr_abs, &param_abs, SCHED_FIFO, 45);
    if (ret) {
        goto out;
    }

    ret = init_pthread_attributes(&attr_airbag, &param_airbag, SCHED_FIFO, 90);
    if (ret) {
        goto out;
    }

    ret = init_pthread_attributes(&attr_display, &param_display, SCHED_FIFO, 10);
    if (ret) {
        goto out;
    }

    ret = init_pthread_attributes(&attr_obter_tecla, &param_obter_tecla, SCHED_FIFO, 30);
    if (ret) {
        goto out;
    }

    /* Criar threads */
    ret = create_thread(&thread_abs, &attr_abs, thread_abs_func, NULL, "ABS");
    if (ret) {
        goto out;
    }

    ret = create_thread(&thread_airbag, &attr_airbag, thread_airbag_func, NULL, "Airbag");
    if (ret) {
        goto out;
    }

    ret = create_thread(&thread_display, &attr_display, thread_display_func, NULL, "Display");
    if (ret) {
        goto out;
    }

    ret = create_thread(&thread_obter_tecla, &attr_obter_tecla, thread_obter_tecla_func, NULL, "Obter Tecla");
    if (ret) {
        goto out;
    }

    /* Aguardar threads */
    pthread_join(thread_abs, NULL);
    pthread_join(thread_airbag, NULL);
    pthread_join(thread_display, NULL);
    pthread_join(thread_obter_tecla, NULL);

    reset_input_mode();

    // g_list_free(ABS_list);
    // g_list_free(AIRBAG_list);

out:
    return ret;
}