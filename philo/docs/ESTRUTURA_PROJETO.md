# Estrutura de Projeto Recomendada - Philosophers

## 📁 Organização Sugerida

```
philosophers/
│
├── Makefile                        # Compilação
│
├── src/                            # Código fonte
│   ├── main.c                      # Entry point
│   ├── init.c                      # Inicialização
│   ├── routine.c                   # Rotina dos filósofos
│   ├── actions.c                   # Ações (eat, sleep, think)
│   ├── monitor.c                   # Monitoramento (morte, fim)
│   ├── utils.c                     # Utilitários (time, print)
│   └── cleanup.c                   # Limpeza de recursos
│
├── includes/                       # Headers
│   └── philo.h                     # Estruturas e protótipos
│
├── obj/                            # Objetos compilados (gitignore)
│
├── docs/                           # Documentação (opcional)
│   ├── README.md                   # Guia principal
│   ├── CONCEITOS.md                # Conceitos fundamentais
│   └── ESTRUTURA_PROJETO.md        # Este arquivo
│
└── tests/                          # Scripts de teste (opcional)
    ├── test_basic.sh               # Testes básicos
    ├── test_death.sh               # Testes de morte
    └── test_stress.sh              # Testes de stress
```

---

## 📋 Descrição de Cada Arquivo

### 🎯 main.c - Entry Point (Ponto de Entrada)

**Responsabilidade**: Orquestrar o fluxo principal do programa.

```c
/* ************************************************************************** */
/*                                  main.c                                    */
/* Fluxo: Parse → Init → Create → Monitor → Join → Cleanup                  */
/* ************************************************************************** */

#include "philo.h"

int main(int argc, char **argv)
{
    t_data  data;

    // 1. Validar e parsear argumentos
    if (!parse_arguments(argc, argv, &data))
        return (error_message("Invalid arguments"));

    // 2. Inicializar estruturas e mutexes
    if (!init_data(&data))
        return (error_message("Initialization failed"));

    // 3. Criar threads dos filósofos
    if (!create_philosophers(&data))
        return (cleanup_and_exit(&data, "Thread creation failed"));

    // 4. Monitorar simulação
    monitor_simulation(&data);

    // 5. Aguardar threads terminarem
    join_philosophers(&data);

    // 6. Limpar recursos
    cleanup(&data);

    return (0);
}

/*
 * FLUXO VISUAL:
 * 
 * MAIN
 *   ↓
 * PARSE args (5 800 200 200)
 *   ↓
 * INIT estruturas + mutexes
 *   ↓
 * CREATE threads (filósofos)
 *   ↓
 * MONITOR (loop checando morte/fim)
 *   ↓
 * JOIN threads
 *   ↓
 * CLEANUP mutexes + memória
 *   ↓
 * EXIT
 */
```

**Funções principais**:
- `parse_arguments()` → Valida e converte args
- `init_data()` → Inicializa tudo
- `create_philosophers()` → Cria threads
- `monitor_simulation()` → Loop de monitoramento
- `join_philosophers()` → Espera threads
- `cleanup()` → Libera recursos

---

### 🔧 init.c - Inicialização

**Responsabilidade**: Preparar todas as estruturas e mutexes.

```c
/* ************************************************************************** */
/*                                  init.c                                    */
/* Responsabilidade: Inicializar estruturas, mutexes e filósofos            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Inicializa estrutura principal
 */
int init_data(t_data *data)
{
    // Inicializar timestamps
    data->start_time = get_time_ms();
    data->dead_flag = 0;
    data->all_ate = 0;

    // Criar mutexes de controle
    if (!init_mutexes(data))
        return (0);

    // Criar array de filósofos
    if (!init_philosophers(data))
        return (0);

    return (1);
}

/*
 * Cria os mutexes (garfos + controle)
 */
int init_mutexes(t_data *data)
{
    int i;

    // Alocar array de garfos (mutexes)
    data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
    if (!data->forks)
        return (0);

    // Inicializar cada garfo
    i = 0;
    while (i < data->num_philos)
    {
        if (pthread_mutex_init(&data->forks[i], NULL) != 0)
            return (0);
        i++;
    }

    // Mutexes de controle
    if (pthread_mutex_init(&data->write_lock, NULL) != 0)
        return (0);
    if (pthread_mutex_init(&data->death_lock, NULL) != 0)
        return (0);
    if (pthread_mutex_init(&data->meal_lock, NULL) != 0)
        return (0);

    return (1);
}

/*
 * Configura cada filósofo
 */
int init_philosophers(t_data *data)
{
    int i;

    // Alocar array
    data->philos = malloc(sizeof(t_philo) * data->num_philos);
    if (!data->philos)
        return (0);

    // Configurar cada um
    i = 0;
    while (i < data->num_philos)
    {
        data->philos[i].id = i + 1;
        data->philos[i].times_eaten = 0;
        data->philos[i].last_meal_time = data->start_time;
        data->philos[i].data = data;
        
        // Atribuir garfos (circular)
        data->philos[i].left_fork = &data->forks[i];
        data->philos[i].right_fork = &data->forks[(i + 1) % data->num_philos];
        
        i++;
    }

    return (1);
}

/*
 * FLUXO:
 * init_data()
 *   ↓
 * init_mutexes() → Cria N garfos + 3 mutexes controle
 *   ↓
 * init_philosophers() → Configura N filósofos com ponteiros para garfos
 */
```

**O que inicializa**:
- ✅ Timestamps (start_time)
- ✅ Flags (dead_flag, all_ate)
- ✅ Mutexes dos garfos (N mutexes)
- ✅ Mutexes de controle (write, death, meal)
- ✅ Array de filósofos
- ✅ Ponteiros left_fork e right_fork

---

### 🏃 routine.c - Rotina dos Filósofos

**Responsabilidade**: Loop principal de cada thread (filósofo).

```c
/* ************************************************************************** */
/*                                routine.c                                   */
/* Responsabilidade: Loop de vida de cada filósofo (thread)                 */
/* ************************************************************************** */

#include "philo.h"

/*
 * Thread principal de cada filósofo
 * Esta função roda em paralelo (N threads ao mesmo tempo)
 */
void *philosopher_routine(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;

    // Filósofos pares esperam um pouco (evita race no início)
    if (philo->id % 2 == 0)
        usleep(1000);

    // Loop principal: comer → dormir → pensar
    while (!simulation_should_stop(philo))
    {
        // Pensar
        philo_think(philo);

        // Pegar garfos → comer → soltar garfos
        philo_eat(philo);

        // Dormir
        philo_sleep(philo);
    }

    return (NULL);
}

/*
 * Cria todas as threads
 */
int create_philosophers(t_data *data)
{
    int i;

    i = 0;
    while (i < data->num_philos)
    {
        if (pthread_create(&data->philos[i].thread, NULL,
                          philosopher_routine, &data->philos[i]) != 0)
            return (0);
        i++;
    }

    return (1);
}

/*
 * Aguarda todas as threads terminarem
 */
void join_philosophers(t_data *data)
{
    int i;

    i = 0;
    while (i < data->num_philos)
    {
        pthread_join(data->philos[i].thread, NULL);
        i++;
    }
}

/*
 * Verifica se deve parar (alguém morreu ou todos comeram)
 */
int simulation_should_stop(t_philo *philo)
{
    int stop;

    pthread_mutex_lock(&philo->data->death_lock);
    stop = philo->data->dead_flag;
    pthread_mutex_unlock(&philo->data->death_lock);

    return (stop);
}

/*
 * FLUXO DE CADA THREAD:
 * 
 * philosopher_routine()
 *   ↓
 * [LOOP enquanto não parar]
 *   ↓
 * philo_think() → Pensar (opcional: print)
 *   ↓
 * philo_eat() → Pegar garfos → comer → soltar
 *   ↓
 * philo_sleep() → Dormir
 *   ↓
 * [volta ao loop]
 */
```

**Conceitos-chave**:
- Cada filósofo é uma thread rodando `philosopher_routine()`
- Loop infinito até morte ou fim
- Pares esperam 1ms (evita todos pegarem garfo esquerdo juntos)

---

### 🍝 actions.c - Ações (Comer, Dormir, Pensar)

**Responsabilidade**: Implementar as ações dos filósofos.

```c
/* ************************************************************************** */
/*                                actions.c                                   */
/* Responsabilidade: Ações individuais dos filósofos                        */
/* ************************************************************************** */

#include "philo.h"

/*
 * PENSAR
 */
void philo_think(t_philo *philo)
{
    print_status(philo, "is thinking");
}

/*
 * COMER - A ação mais importante!
 */
void philo_eat(t_philo *philo)
{
    // 1. Pegar garfos (ordem depende se é par/ímpar)
    take_forks(philo);

    // 2. Atualizar timestamp da última refeição (CRÍTICO!)
    pthread_mutex_lock(&philo->data->meal_lock);
    philo->last_meal_time = get_time_ms();
    philo->times_eaten++;
    pthread_mutex_unlock(&philo->data->meal_lock);

    // 3. Printar status
    print_status(philo, "is eating");

    // 4. Esperar time_to_eat
    precise_sleep(philo->data->time_to_eat);

    // 5. Soltar garfos
    drop_forks(philo);
}

/*
 * DORMIR
 */
void philo_sleep(t_philo *philo)
{
    print_status(philo, "is sleeping");
    precise_sleep(philo->data->time_to_sleep);
}

/*
 * PEGAR GARFOS - Evita deadlock!
 */
void take_forks(t_philo *philo)
{
    // Estratégia: pares e ímpares pegam em ordem diferente
    if (philo->id % 2 == 0)
    {
        // Pares: direita → esquerda
        pthread_mutex_lock(philo->right_fork);
        print_status(philo, "has taken a fork");
        pthread_mutex_lock(philo->left_fork);
        print_status(philo, "has taken a fork");
    }
    else
    {
        // Ímpares: esquerda → direita
        pthread_mutex_lock(philo->left_fork);
        print_status(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
        print_status(philo, "has taken a fork");
    }
}

/*
 * SOLTAR GARFOS
 */
void drop_forks(t_philo *philo)
{
    pthread_mutex_unlock(philo->left_fork);
    pthread_mutex_unlock(philo->right_fork);
}

/*
 * FLUXO DE COMER:
 * 
 * philo_eat()
 *   ↓
 * take_forks()
 *   → lock(fork1)
 *   → print "taken fork"
 *   → lock(fork2)
 *   → print "taken fork"
 *   ↓
 * Atualizar last_meal_time (PROTEGIDO!)
 *   ↓
 * print "is eating"
 *   ↓
 * precise_sleep(time_to_eat)
 *   ↓
 * drop_forks()
 *   → unlock(left)
 *   → unlock(right)
 */
```

**Detalhes importantes**:
- `take_forks()` usa estratégia par/ímpar → **EVITA DEADLOCK**
- `last_meal_time` é atualizado **protegido por mutex**
- `precise_sleep()` garante tempo exato

---

### 👁️ monitor.c - Monitoramento

**Responsabilidade**: Verificar morte e condição de parada.

```c
/* ************************************************************************** */
/*                               monitor.c                                    */
/* Responsabilidade: Monitorar morte e condição de fim                      */
/* ************************************************************************** */

#include "philo.h"

/*
 * Loop principal de monitoramento (roda na thread principal)
 */
void monitor_simulation(t_data *data)
{
    while (1)
    {
        // Verificar se alguém morreu
        if (check_death(data))
            break;

        // Verificar se todos comeram suficiente
        if (data->must_eat_count != -1 && check_all_ate(data))
            break;

        // Pequena pausa (não precisa verificar a toda microsegundo)
        usleep(1000); // 1ms
    }
}

/*
 * Verifica se algum filósofo morreu
 */
int check_death(t_data *data)
{
    int     i;
    long    current_time;
    long    time_since_meal;

    i = 0;
    while (i < data->num_philos)
    {
        current_time = get_time_ms();

        // Pegar last_meal_time protegido
        pthread_mutex_lock(&data->meal_lock);
        time_since_meal = current_time - data->philos[i].last_meal_time;
        pthread_mutex_unlock(&data->meal_lock);

        // Morreu?
        if (time_since_meal > data->time_to_die)
        {
            // Printar morte
            print_death(&data->philos[i]);

            // Setar flag
            pthread_mutex_lock(&data->death_lock);
            data->dead_flag = 1;
            pthread_mutex_unlock(&data->death_lock);

            return (1);
        }
        i++;
    }
    return (0);
}

/*
 * Verifica se todos comeram o suficiente
 */
int check_all_ate(t_data *data)
{
    int i;
    int all_ate;

    i = 0;
    all_ate = 1;

    pthread_mutex_lock(&data->meal_lock);
    while (i < data->num_philos)
    {
        if (data->philos[i].times_eaten < data->must_eat_count)
        {
            all_ate = 0;
            break;
        }
        i++;
    }
    pthread_mutex_unlock(&data->meal_lock);

    if (all_ate)
    {
        pthread_mutex_lock(&data->death_lock);
        data->dead_flag = 1; // Usar flag para parar
        pthread_mutex_unlock(&data->death_lock);
    }

    return (all_ate);
}

/*
 * FLUXO DE MONITORAMENTO:
 * 
 * monitor_simulation() [MAIN THREAD]
 *   ↓
 * [LOOP infinito]
 *   ↓
 * check_death()
 *   → Para cada filósofo:
 *     → Pegar last_meal_time (protegido)
 *     → Se (agora - last_meal) > time_to_die:
 *       → print_death()
 *       → dead_flag = 1
 *       → return 1 (morreu!)
 *   ↓
 * check_all_ate()
 *   → Para cada filósofo:
 *     → Se times_eaten < must_eat_count:
 *       → return 0 (ainda não)
 *   → Se todos comeram:
 *     → dead_flag = 1 (usa flag para parar)
 *     → return 1
 *   ↓
 * usleep(1000) → Espera 1ms
 *   ↓
 * [volta ao loop]
 */
```

**Por que em loop separado?**
- Threads dos filósofos não verificam morte (focadas em comer/dormir)
- Thread principal fica verificando constantemente
- Detecta morte em até 1ms (bem abaixo de 10ms requerido)

---

### 🛠️ utils.c - Utilitários

**Responsabilidade**: Funções auxiliares (tempo, print).

```c
/* ************************************************************************** */
/*                                 utils.c                                    */
/* Responsabilidade: Funções utilitárias (tempo, print, conversão)          */
/* ************************************************************************** */

#include "philo.h"

/*
 * Pega timestamp atual em milisegundos
 */
long get_time_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/*
 * Sleep preciso (melhor que usleep sozinho)
 */
void precise_sleep(long milliseconds)
{
    long start;
    long target;

    start = get_time_ms();
    target = start + milliseconds;

    while (get_time_ms() < target)
        usleep(500); // Verifica a cada 0.5ms
}

/*
 * Print thread-safe com timestamp
 */
void print_status(t_philo *philo, char *status)
{
    long timestamp;

    pthread_mutex_lock(&philo->data->write_lock);

    // Verificar se alguém morreu (não printar depois da morte)
    pthread_mutex_lock(&philo->data->death_lock);
    if (philo->data->dead_flag)
    {
        pthread_mutex_unlock(&philo->data->death_lock);
        pthread_mutex_unlock(&philo->data->write_lock);
        return;
    }
    pthread_mutex_unlock(&philo->data->death_lock);

    // Calcular timestamp
    timestamp = get_time_ms() - philo->data->start_time;

    // Printar
    printf("%ld %d %s\n", timestamp, philo->id, status);

    pthread_mutex_unlock(&philo->data->write_lock);
}

/*
 * Print de morte (especial - sempre printa)
 */
void print_death(t_philo *philo)
{
    long timestamp;

    pthread_mutex_lock(&philo->data->write_lock);
    timestamp = get_time_ms() - philo->data->start_time;
    printf("%ld %d died\n", timestamp, philo->id);
    pthread_mutex_unlock(&philo->data->write_lock);
}

/*
 * Converte string para int (com validação)
 */
int ft_atoi_valid(const char *str)
{
    long result;
    int sign;

    result = 0;
    sign = 1;

    // Pular espaços
    while (*str == ' ' || (*str >= 9 && *str <= 13))
        str++;

    // Sinal
    if (*str == '-' || *str == '+')
    {
        if (*str == '-')
            sign = -1;
        str++;
    }

    // Converter dígitos
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + (*str - '0');
        
        // Overflow check
        if (result * sign > INT_MAX || result * sign < INT_MIN)
            return (-1);
        str++;
    }

    // Verificar se sobrou caracteres inválidos
    if (*str != '\0')
        return (-1);

    return ((int)(result * sign));
}

/*
 * Parse de argumentos
 */
int parse_arguments(int argc, char **argv, t_data *data)
{
    // Validar quantidade de argumentos
    if (argc < 5 || argc > 6)
        return (0);

    // Converter e validar cada argumento
    data->num_philos = ft_atoi_valid(argv[1]);
    data->time_to_die = ft_atoi_valid(argv[2]);
    data->time_to_eat = ft_atoi_valid(argv[3]);
    data->time_to_sleep = ft_atoi_valid(argv[4]);

    // Argumento opcional
    if (argc == 6)
        data->must_eat_count = ft_atoi_valid(argv[5]);
    else
        data->must_eat_count = -1;

    // Validar valores
    if (data->num_philos <= 0 || data->time_to_die <= 0 ||
        data->time_to_eat <= 0 || data->time_to_sleep <= 0)
        return (0);

    if (argc == 6 && data->must_eat_count <= 0)
        return (0);

    return (1);
}

/*
 * Mensagem de erro
 */
int error_message(char *msg)
{
    write(2, "Error: ", 7);
    write(2, msg, ft_strlen(msg));
    write(2, "\n", 1);
    return (1);
}
```

**Funções importantes**:
- `get_time_ms()` → Base para todos os timestamps
- `precise_sleep()` → Melhor que `usleep()` sozinho
- `print_status()` → Thread-safe + verifica morte
- `parse_arguments()` → Validação robusta

---

### 🧹 cleanup.c - Limpeza

**Responsabilidade**: Destruir mutexes e liberar memória.

```c
/* ************************************************************************** */
/*                               cleanup.c                                    */
/* Responsabilidade: Destruir mutexes e liberar memória                     */
/* ************************************************************************** */

#include "philo.h"

/*
 * Limpa todos os recursos
 */
void cleanup(t_data *data)
{
    destroy_mutexes(data);
    free_memory(data);
}

/*
 * Destrói todos os mutexes
 */
void destroy_mutexes(t_data *data)
{
    int i;

    // Destruir garfos
    i = 0;
    while (i < data->num_philos)
    {
        pthread_mutex_destroy(&data->forks[i]);
        i++;
    }

    // Destruir mutexes de controle
    pthread_mutex_destroy(&data->write_lock);
    pthread_mutex_destroy(&data->death_lock);
    pthread_mutex_destroy(&data->meal_lock);
}

/*
 * Libera toda memória alocada
 */
void free_memory(t_data *data)
{
    if (data->forks)
        free(data->forks);
    if (data->philos)
        free(data->philos);
}

/*
 * Cleanup com mensagem de erro
 */
int cleanup_and_exit(t_data *data, char *msg)
{
    cleanup(data);
    return (error_message(msg));
}
```

**Ordem de limpeza**:
1. Destruir mutexes (garfos + controle)
2. Liberar memória (arrays)

---

## 🎯 philo.h - Header Principal

```c
/* ************************************************************************** */
/*                                 philo.h                                    */
/* Header com todas as estruturas, includes e protótipos                    */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

// ==================== INCLUDES ====================
# include <stdio.h>          // printf
# include <stdlib.h>         // malloc, free
# include <unistd.h>         // write, usleep
# include <pthread.h>        // threads e mutexes
# include <sys/time.h>       // gettimeofday
# include <limits.h>         // INT_MAX, INT_MIN

// ==================== ESTRUTURAS ====================

// Dados de cada filósofo
typedef struct s_philo
{
    int                 id;              // 1 a N
    int                 times_eaten;     // Contador de refeições
    long                last_meal_time;  // Timestamp última refeição (ms)
    pthread_t           thread;          // Thread do filósofo
    pthread_mutex_t     *left_fork;      // Ponteiro para garfo esquerdo
    pthread_mutex_t     *right_fork;     // Ponteiro para garfo direito
    struct s_data       *data;           // Referência para dados globais
}   t_philo;

// Dados compartilhados (globais da simulação)
typedef struct s_data
{
    int                 num_philos;      // Número de filósofos
    long                time_to_die;     // Tempo máximo sem comer (ms)
    long                time_to_eat;     // Tempo para comer (ms)
    long                time_to_sleep;   // Tempo para dormir (ms)
    int                 must_eat_count;  // Vezes que cada um deve comer (-1 se não especificado)
    long                start_time;      // Timestamp início da simulação (ms)
    int                 dead_flag;       // Flag: alguém morreu? (0/1)
    pthread_mutex_t     *forks;          // Array de mutexes (garfos)
    pthread_mutex_t     write_lock;      // Mutex para printf
    pthread_mutex_t     death_lock;      // Mutex para dead_flag
    pthread_mutex_t     meal_lock;       // Mutex para last_meal_time e times_eaten
    t_philo             *philos;         // Array de filósofos
}   t_data;

// ==================== PROTÓTIPOS ====================

// main.c
int     main(int argc, char **argv);

// init.c
int     init_data(t_data *data);
int     init_mutexes(t_data *data);
int     init_philosophers(t_data *data);

// routine.c
void    *philosopher_routine(void *arg);
int     create_philosophers(t_data *data);
void    join_philosophers(t_data *data);
int     simulation_should_stop(t_philo *philo);

// actions.c
void    philo_think(t_philo *philo);
void    philo_eat(t_philo *philo);
void    philo_sleep(t_philo *philo);
void    take_forks(t_philo *philo);
void    drop_forks(t_philo *philo);

// monitor.c
void    monitor_simulation(t_data *data);
int     check_death(t_data *data);
int     check_all_ate(t_data *data);

// utils.c
long    get_time_ms(void);
void    precise_sleep(long milliseconds);
void    print_status(t_philo *philo, char *status);
void    print_death(t_philo *philo);
int     ft_atoi_valid(const char *str);
int     parse_arguments(int argc, char **argv, t_data *data);
int     error_message(char *msg);
int     ft_strlen(char *str);

// cleanup.c
void    cleanup(t_data *data);
void    destroy_mutexes(t_data *data);
void    free_memory(t_data *data);
int     cleanup_and_exit(t_data *data, char *msg);

#endif
```

---

## 📊 Fluxo de Execução Completo

```
╔════════════════════════════════════════════════════════════════╗
║                        MAIN THREAD                             ║
╚════════════════════════════════════════════════════════════════╝
                              |
                              v
                    ┌─────────────────┐
                    │  parse_args()   │
                    └────────┬────────┘
                              v
                    ┌─────────────────┐
                    │   init_data()   │
                    │   ┌─────────┐   │
                    │   │ mutexes │   │
                    │   └─────────┘   │
                    │   ┌─────────┐   │
                    │   │ philos  │   │
                    │   └─────────┘   │
                    └────────┬────────┘
                              v
              ┌───────────────────────────────────┐
              │   create_philosophers()           │
              └───────────────┬───────────────────┘
                              v
    ┌─────────┬───────────┬───────┬───────┬─────────┐
    v         v           v       v       v         v
┌────────┐ ┌────────┐ ┌────────┐  ...  ┌────────┐
│Thread 1│ │Thread 2│ │Thread 3│       │Thread N│
└───┬────┘ └───┬────┘ └───┬────┘       └───┬────┘
    │          │          │                 │
    v          v          v                 v
[LOOP]     [LOOP]     [LOOP]            [LOOP]
 think      think      think             think
   ↓          ↓          ↓                 ↓
 eat        eat        eat               eat
   ↓          ↓          ↓                 ↓
 sleep      sleep      sleep             sleep
   ↓          ↓          ↓                 ↓
[volta]    [volta]    [volta]           [volta]

                    ENQUANTO ISSO...
                              
                    ┌──────────────────┐
                    │ monitor_simulation│
                    │   (MAIN THREAD)   │
                    └────────┬──────────┘
                             v
                      [LOOP infinito]
                             v
                    ┌────────────────┐
                    │ check_death()  │
                    │   Para cada    │
                    │   filósofo:    │
                    │   verificar    │
                    │   tempo sem    │
                    │   comer        │
                    └────────┬───────┘
                             v
                    ┌────────────────┐
                    │check_all_ate() │
                    │   Todos        │
                    │   comeram N    │
                    │   vezes?       │
                    └────────┬───────┘
                             v
                       usleep(1ms)
                             v
                      [volta ao loop]
                             v
                    ┌────────────────┐
                    │ CONDIÇÃO FIM:  │
                    │ morte OU       │
                    │ todos comeram  │
                    └────────┬───────┘
                             v
                    ┌────────────────┐
                    │ join_philos()  │
                    │ (espera todas  │
                    │  as threads)   │
                    └────────┬───────┘
                             v
                    ┌────────────────┐
                    │   cleanup()    │
                    │ - destroy      │
                    │   mutexes      │
                    │ - free memory  │
                    └────────┬───────┘
                             v
                          [EXIT]
```

---

## 📝 Makefile Sugerido

```makefile
# **************************************************************************** #
#                                 Makefile                                     #
# **************************************************************************** #

NAME        = philo

# Compilador e flags
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread
IFLAGS      = -I includes
DEBUG_FLAGS = -g -fsanitize=thread

# Diretórios
SRC_DIR     = src
OBJ_DIR     = obj
INC_DIR     = includes

# Arquivos
SRCS        = $(SRC_DIR)/main.c \
              $(SRC_DIR)/init.c \
              $(SRC_DIR)/routine.c \
              $(SRC_DIR)/actions.c \
              $(SRC_DIR)/monitor.c \
              $(SRC_DIR)/utils.c \
              $(SRC_DIR)/cleanup.c

OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Cores
GREEN       = \033[0;32m
YELLOW      = \033[0;33m
RED         = \033[0;31m
RESET       = \033[0m

# Regras
all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@
	@echo "$(YELLOW)Compiling: $<$(RESET)"

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(RED)✗ Object files removed$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)✗ $(NAME) removed$(RESET)"

re: fclean all

# Debug com thread sanitizer
debug: CFLAGS += $(DEBUG_FLAGS)
debug: re
	@echo "$(GREEN)✓ Debug version compiled with thread sanitizer$(RESET)"

# Testes rápidos
test: $(NAME)
	@echo "$(YELLOW)Running basic tests...$(RESET)"
	./$(NAME) 5 800 200 200
	@echo ""
	./$(NAME) 4 410 200 200
	@echo ""
	./$(NAME) 4 310 200 100

# Teste com valgrind helgrind (detecta data races)
helgrind: $(NAME)
	valgrind --tool=helgrind ./$(NAME) 5 800 200 200

.PHONY: all clean fclean re debug test helgrind
```

---

## 🎯 Vantagens desta Estrutura

### ✅ Separação de Responsabilidades
Cada arquivo tem UMA função clara:
- `main.c` → Orquestra
- `init.c` → Inicializa
- `routine.c` → Threads
- `actions.c` → Ações
- `monitor.c` → Monitora
- `utils.c` → Utilitários
- `cleanup.c` → Limpa

### ✅ Facilita Debug
```
Problema no monitoramento de morte? → monitor.c
Problema com garfos? → actions.c (take_forks)
Problema com timestamps? → utils.c
```

### ✅ Testável
Cada módulo pode ser testado separadamente:
```c
// Testar só parse
parse_arguments(5, argv, &data);
assert(data.num_philos == 5);

// Testar só init
init_data(&data);
assert(data.forks != NULL);
```

### ✅ Fácil de Entender
Fluxo claro e linear:
1. Parse → 2. Init → 3. Create → 4. Monitor → 5. Join → 6. Cleanup

### ✅ Escalável
Adicionar features:
- Nova ação? → `actions.c`
- Novo tipo de monitoramento? → `monitor.c`
- Nova validação? → `utils.c`

---

## 📚 Ordem de Implementação Sugerida

1. **Fase 1: Estrutura Base**
   - [ ] `philo.h` - Estruturas
   - [ ] `utils.c` - parse, time, print
   - [ ] `main.c` - Skeleton básico

2. **Fase 2: Inicialização**
   - [ ] `init.c` - Criar mutexes
   - [ ] `init.c` - Criar filósofos
   - [ ] `cleanup.c` - Destruir recursos

3. **Fase 3: Threads**
   - [ ] `routine.c` - Criar threads
   - [ ] `routine.c` - Loop básico
   - [ ] Testar com prints simples

4. **Fase 4: Ações**
   - [ ] `actions.c` - take_forks (evitar deadlock!)
   - [ ] `actions.c` - eat, sleep, think
   - [ ] Testar sem morte

5. **Fase 5: Monitoramento**
   - [ ] `monitor.c` - check_death
   - [ ] `monitor.c` - check_all_ate
   - [ ] Testar casos de morte

6. **Fase 6: Refinamento**
   - [ ] Timestamps precisos
   - [ ] Sincronização de printf
   - [ ] Casos edge (1 filósofo, etc)

---

**Esta estrutura garante código limpo, organizado e fácil de debugar! 🚀**
