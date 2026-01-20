# Conceitos Fundamentais - Do Zero ao Philosophers

## 🎯 Índice
1. [Programas Sequenciais vs Concorrentes](#1-programas-sequenciais-vs-concorrentes)
2. [O que são Threads?](#2-o-que-são-threads)
3. [Processos vs Threads](#3-processos-vs-threads)
4. [Problemas da Concorrência](#4-problemas-da-concorrência)
5. [Mutexes - A Solução](#5-mutexes---a-solução)
6. [Race Conditions](#6-race-conditions)
7. [Deadlocks](#7-deadlocks)
8. [Sincronização](#8-sincronização)
9. [Aplicando no Philosophers](#9-aplicando-no-philosophers)

---

## 1. Programas Sequenciais vs Concorrentes

### 🚶 Programa Sequencial (Normal)
É como você sempre programou: uma coisa de cada vez, em ordem.

```c
// Programa sequencial
int main(void)
{
    printf("Passo 1\n");  // Executa primeiro
    printf("Passo 2\n");  // Depois este
    printf("Passo 3\n");  // Por último este
    return (0);
}

// Saída SEMPRE:
// Passo 1
// Passo 2
// Passo 3
```

**Analogia**: Como ler um livro - você lê página 1, depois página 2, depois página 3...

### 🏃🏃🏃 Programa Concorrente (Paralelo)
Várias coisas acontecendo "ao mesmo tempo".

```c
// Programa concorrente (pseudo-código)
int main(void)
{
    criar_thread(tarefa_A);  // Executa em paralelo
    criar_thread(tarefa_B);  // Executa em paralelo
    criar_thread(tarefa_C);  // Executa em paralelo
    
    aguardar_todas_threads();
    return (0);
}

// Saída pode ser QUALQUER ordem:
// Tarefa B iniciou
// Tarefa A iniciou
// Tarefa C iniciou
// Tarefa A terminou
// ...
```

**Analogia**: Como ter 3 pessoas lendo livros diferentes ao mesmo tempo na mesma sala.

---

## 2. O que são Threads?

### 📖 Definição Simples
**Thread** = "Linha de execução" = Um caminho que seu programa segue.

Imagine seu programa como uma estrada:
- **Programa normal**: Uma única estrada, um único carro
- **Programa com threads**: Várias estradas, vários carros ao mesmo tempo

### 🎬 Visualização

```
Programa SEM threads:
    MAIN
     |
     v
  Tarefa 1  (espera terminar)
     |
     v
  Tarefa 2  (espera terminar)
     |
     v
  Tarefa 3  (espera terminar)
     |
     v
    FIM

Tempo total = T1 + T2 + T3


Programa COM threads:
         MAIN
          |
    +-----+-----+
    |     |     |
    v     v     v
  Tar1  Tar2  Tar3   (todos executam JUNTOS!)
    |     |     |
    +-----+-----+
          |
          v
        FIM

Tempo total ≈ MAX(T1, T2, T3)
```

### 💻 Exemplo Real

```c
#include <pthread.h>
#include <stdio.h>

// Função que a thread vai executar
void *minha_thread(void *arg)
{
    printf("Olá da thread!\n");
    return (NULL);
}

int main(void)
{
    pthread_t thread_id;
    
    // Criar a thread
    pthread_create(&thread_id, NULL, minha_thread, NULL);
    
    printf("Olá do main!\n");
    
    // Esperar a thread terminar
    pthread_join(thread_id, NULL);
    
    return (0);
}

// Saída pode ser:
// Olá do main!
// Olá da thread!
// OU
// Olá da thread!
// Olá do main!
// (Depende de qual executa primeiro!)
```

### 🔑 Conceitos-chave

1. **pthread_create()** = "Nascer" - Cria uma nova thread
2. **pthread_join()** = "Esperar" - Espera a thread terminar
3. **Função da thread** = O que a thread vai fazer
4. **Execução paralela** = As threads rodam ao mesmo tempo

---

## 3. Processos vs Threads

### 🏠 Processos = Casas Separadas

```
Processo 1          Processo 2
+---------+         +---------+
| Memória |         | Memória |
| Código  |         | Código  |
| Dados   |         | Dados   |
+---------+         +---------+
   Isolado            Isolado
```

- Cada processo tem sua própria memória
- Não compartilham variáveis
- Comunicação é difícil
- Mais "pesado" (usa mais recursos)

### 🚪 Threads = Quartos na Mesma Casa

```
        Processo
+------------------------+
|  Memória Compartilhada |
|                        |
|  Thread 1   Thread 2   |
|    |          |        |
|    v          v        |
|  Código    Código      |
+------------------------+
```

- Threads do mesmo processo compartilham memória
- Podem acessar as mesmas variáveis
- Comunicação é fácil
- Mais "leve" (usa menos recursos)

### 📊 Comparação

| Aspecto | Processo | Thread |
|---------|----------|--------|
| Memória | Separada | Compartilhada |
| Criação | Lenta (fork) | Rápida (pthread_create) |
| Comunicação | Difícil (pipes, signals) | Fácil (variáveis) |
| Peso | Pesado | Leve |
| Isolamento | Total | Parcial |

**Por que Philosophers usa threads?**
- Filósofos precisam compartilhar os garfos (mesma memória)
- Mais rápido e eficiente
- Mais fácil sincronizar

---

## 4. Problemas da Concorrência

Quando múltiplas threads acessam os mesmos dados, coisas estranhas podem acontecer...

### 🐛 Problema 1: Leitura/Escrita Simultânea

```c
int saldo = 100;  // Variável compartilhada

// Thread 1
void *sacar(void *arg)
{
    int temp = saldo;      // Lê: 100
    temp = temp - 50;      // Calcula: 50
    saldo = temp;          // Escreve: 50
}

// Thread 2
void *depositar(void *arg)
{
    int temp = saldo;      // Lê: 100
    temp = temp + 30;      // Calcula: 130
    saldo = temp;          // Escreve: 130
}
```

**O que deveria acontecer**: 100 - 50 + 30 = 80

**O que PODE acontecer** (execução intercalada):

```
Tempo   Thread 1              Thread 2              saldo
0       -                     -                     100
1       lê saldo (100)        -                     100
2       calcula (50)          -                     100
3       -                     lê saldo (100)        100
4       -                     calcula (130)         100
5       escreve saldo (50)    -                     50
6       -                     escreve saldo (130)   130
```

**Resultado**: 130 (ERRADO! Perdemos o saque de 50!)

Isso se chama **RACE CONDITION** (Condição de Corrida).

### 🐛 Problema 2: Deadlock (Travamento Eterno)

```
Recurso A    Recurso B

Thread 1:
1. Pega A   ✓
2. Pega B   ⏳ (esperando Thread 2 soltar B)

Thread 2:
1. Pega B   ✓
2. Pega A   ⏳ (esperando Thread 1 soltar A)

Resultado: TRAVAMENTO! Ninguém solta, ninguém avança.
```

**Analogia**: Dois carros em uma ponte estreita, cada um esperando o outro recuar.

---

## 5. Mutexes - A Solução

### 🔐 O que é um Mutex?

**Mutex** = **Mut**ual **Ex**clusion (Exclusão Mútua)

É como um **cadeado** para proteger código:
- Só uma thread pode "entrar" por vez
- Outras threads esperam na fila
- Quando a primeira sai, a próxima entra

### 🚪 Analogia: Banheiro

```
        🚪 BANHEIRO
         (Mutex)
           |
    [Pessoa 1 DENTRO] 🔒
           |
    Fila:
    [Pessoa 2] ⏳
    [Pessoa 3] ⏳
    [Pessoa 4] ⏳
```

- Pessoa 1 tranca a porta (lock)
- Outras esperam
- Pessoa 1 sai e destranca (unlock)
- Pessoa 2 entra e tranca

### 💻 Código

```c
pthread_mutex_t mutex;

// Inicializar o mutex
pthread_mutex_init(&mutex, NULL);

// Thread 1
void *funcao1(void *arg)
{
    pthread_mutex_lock(&mutex);      // Tranca (espera se ocupado)
    // ===== SEÇÃO CRÍTICA =====
    printf("Thread 1 na área protegida\n");
    // =========================
    pthread_mutex_unlock(&mutex);    // Destranca
}

// Thread 2
void *funcao2(void *arg)
{
    pthread_mutex_lock(&mutex);      // Tranca (espera se ocupado)
    // ===== SEÇÃO CRÍTICA =====
    printf("Thread 2 na área protegida\n");
    // =========================
    pthread_mutex_unlock(&mutex);    // Destranca
}

// Destruir o mutex
pthread_mutex_destroy(&mutex);
```

### 🎯 Resolvendo o Problema do Saldo

```c
int saldo = 100;
pthread_mutex_t mutex_saldo;

void *sacar(void *arg)
{
    pthread_mutex_lock(&mutex_saldo);    // 🔒 PROTEGE
    int temp = saldo;
    temp = temp - 50;
    saldo = temp;
    pthread_mutex_unlock(&mutex_saldo);  // 🔓 LIBERA
}

void *depositar(void *arg)
{
    pthread_mutex_lock(&mutex_saldo);    // 🔒 PROTEGE
    int temp = saldo;
    temp = temp + 30;
    saldo = temp;
    pthread_mutex_unlock(&mutex_saldo);  // 🔓 LIBERA
}

// Agora: resultado SEMPRE correto! (80)
```

---

## 6. Race Conditions

### 📖 Definição
**Race Condition** = Quando o resultado depende da "corrida" entre threads.

### 🏁 Exemplo: Contador

```c
int contador = 0;  // Compartilhado

void *incrementar(void *arg)
{
    for (int i = 0; i < 1000000; i++)
        contador++;  // ⚠️ NÃO É ATÔMICO!
}

int main(void)
{
    pthread_t t1, t2;
    
    pthread_create(&t1, NULL, incrementar, NULL);
    pthread_create(&t2, NULL, incrementar, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Contador: %d\n", contador);
    // Esperado: 2000000
    // Real: ??? (menor que 2000000!)
}
```

### 🔬 Por que acontece?

O `contador++` na verdade é **3 operações**:

```assembly
1. LER valor de contador (memória → registrador)
2. INCREMENTAR registrador
3. ESCREVER de volta (registrador → memória)
```

Intercalando:

```
Thread 1: LER (0)
Thread 2: LER (0)         ⚠️ Ambas leram 0!
Thread 1: INCREMENTAR (1)
Thread 2: INCREMENTAR (1) ⚠️ Ambas calcularam 1!
Thread 1: ESCREVER (1)
Thread 2: ESCREVER (1)    ⚠️ Resultado: 1 (deveria ser 2!)
```

### ✅ Solução

```c
int contador = 0;
pthread_mutex_t mutex;

void *incrementar(void *arg)
{
    for (int i = 0; i < 1000000; i++)
    {
        pthread_mutex_lock(&mutex);
        contador++;  // Agora é seguro!
        pthread_mutex_unlock(&mutex);
    }
}
```

---

## 7. Deadlocks

### 📖 Definição
**Deadlock** = Travamento permanente onde threads esperam umas pelas outras eternamente.

### 🔒 As 4 Condições para Deadlock

Deadlock só acontece quando **TODAS** estas 4 condições existem:

1. **Exclusão Mútua**: Recursos só podem ser usados por uma thread
2. **Posse e Espera**: Thread segura um recurso e espera outro
3. **Não-Preempção**: Não dá pra "roubar" um recurso de outra thread
4. **Espera Circular**: Thread A espera B, B espera C, C espera A

### 🍴 Exemplo: Philosophers Deadlock

```c
Filósofo 1: pega garfo 1, tenta pegar garfo 2 ⏳
Filósofo 2: pega garfo 2, tenta pegar garfo 3 ⏳
Filósofo 3: pega garfo 3, tenta pegar garfo 4 ⏳
Filósofo 4: pega garfo 4, tenta pegar garfo 5 ⏳
Filósofo 5: pega garfo 5, tenta pegar garfo 1 ⏳

🔁 CICLO FECHADO! Ninguém consegue avançar!
```

### 💡 Soluções para Evitar Deadlock

#### Solução 1: Ordem Global
Sempre pegar recursos na mesma ordem:

```c
// ❌ ERRADO
pega_garfo_esquerdo();
pega_garfo_direito();

// ✅ CORRETO
int primeiro = min(esquerdo, direito);
int segundo = max(esquerdo, direito);
pega_garfo(primeiro);
pega_garfo(segundo);
```

#### Solução 2: Última Thread Diferente
O último filósofo pega na ordem inversa:

```c
if (id == num_philos - 1)
{
    pega_garfo_direito();   // Ordem inversa!
    pega_garfo_esquerdo();
}
else
{
    pega_garfo_esquerdo();
    pega_garfo_direito();
}
```

#### Solução 3: Pares e Ímpares
Pares pegam direita primeiro, ímpares pegam esquerda primeiro:

```c
if (id % 2 == 0)
{
    pega_garfo_direito();
    pega_garfo_esquerdo();
}
else
{
    pega_garfo_esquerdo();
    pega_garfo_direito();
}
```

---

## 8. Sincronização

### 📖 Definição
**Sincronização** = Coordenar threads para trabalharem juntas corretamente.

### 🎼 Tipos de Sincronização

#### 1. **Exclusão Mútua** (Mutexes)
Só um por vez:
```c
pthread_mutex_lock(&fork);
// Só EU uso o garfo agora
pthread_mutex_unlock(&fork);
```

#### 2. **Ordenação**
Garantir que A aconteça antes de B:
```c
// Thread 1
preparar_dados();
flag_pronto = 1;

// Thread 2
while (flag_pronto == 0)
    usleep(100);  // Espera Thread 1
processar_dados();
```

#### 3. **Sincronização de I/O**
Printf não é thread-safe! Precisa de mutex:

```c
pthread_mutex_t print_mutex;

void print_safe(char *msg)
{
    pthread_mutex_lock(&print_mutex);
    printf("%s", msg);
    pthread_mutex_unlock(&print_mutex);
}
```

### ⏰ Funções de Tempo

#### gettimeofday() - Pegar timestamp

```c
#include <sys/time.h>

long get_time_ms(void)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    
    // tv.tv_sec  = segundos desde 1970
    // tv.tv_usec = microsegundos (0-999999)
    
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

// Uso:
long inicio = get_time_ms();
// ... fazer algo ...
long fim = get_time_ms();
long duracao = fim - inicio;  // Em milisegundos
```

#### usleep() - Dormir com precisão

```c
#include <unistd.h>

// Dormir 500 milisegundos
usleep(500 * 1000);  // usleep usa MICROsegundos

// Comparação:
sleep(1);        // 1 segundo (impreciso)
usleep(1000000); // 1 segundo (preciso)
usleep(500000);  // 0.5 segundos
usleep(1000);    // 1 milisegundo
```

#### Espera Ativa (Busy Waiting)

```c
void precise_sleep(long milliseconds)
{
    long start = get_time_ms();
    long target = start + milliseconds;
    
    while (get_time_ms() < target)
    {
        usleep(100);  // Verifica a cada 0.1ms
    }
}
```

---

## 9. Aplicando no Philosophers

### 🎯 Mapeamento de Conceitos

| Conceito Real | No Código |
|---------------|-----------|
| Filósofo | `pthread_t` (thread) |
| Garfo | `pthread_mutex_t` (mutex) |
| Pegar garfo | `pthread_mutex_lock()` |
| Soltar garfo | `pthread_mutex_unlock()` |
| Comer | Loop com sleep |
| Mesa redonda | Array circular |
| Morrer | Flag + timestamp check |

### 🏗️ Estrutura Básica

```c
// 1. Dados de cada filósofo
typedef struct s_philo
{
    int             id;              // 1 a N
    pthread_t       thread;          // A thread dele
    long            last_meal;       // Timestamp última refeição
    int             meals_count;     // Quantas vezes comeu
    pthread_mutex_t *left_fork;      // Ponteiro para garfo esquerdo
    pthread_mutex_t *right_fork;     // Ponteiro para garfo direito
    struct s_data   *data;           // Dados globais
}   t_philo;

// 2. Dados compartilhados
typedef struct s_data
{
    int             num_philos;
    long            time_to_die;
    long            time_to_eat;
    long            time_to_sleep;
    int             must_eat;
    long            start_time;
    int             dead_flag;       // Alguém morreu?
    pthread_mutex_t *forks;          // Array de mutexes
    pthread_mutex_t write_lock;      // Para printf
    pthread_mutex_t death_lock;      // Para dead_flag
    t_philo         *philos;         // Array de filósofos
}   t_data;
```

### 🎬 Fluxo de Execução

```c
// Main thread
int main(int argc, char **argv)
{
    t_data data;
    
    // Setup
    parse_args(argc, argv, &data);
    init_mutexes(&data);
    init_philos(&data);
    
    // Criar todas as threads
    for (int i = 0; i < data.num_philos; i++)
        pthread_create(&data.philos[i].thread, NULL, 
                       philo_routine, &data.philos[i]);
    
    // Monitorar
    while (!check_if_dead(&data) && !check_all_ate(&data))
        usleep(1000);
    
    // Aguardar
    for (int i = 0; i < data.num_philos; i++)
        pthread_join(data.philos[i].thread, NULL);
    
    // Limpar
    cleanup(&data);
    return (0);
}

// Thread de cada filósofo
void *philo_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    
    while (!should_stop(philo))
    {
        think(philo);
        
        take_forks(philo);  // Lock 2 mutexes
        eat(philo);         // Update last_meal
        put_forks(philo);   // Unlock 2 mutexes
        
        sleep_philo(philo);
    }
    return (NULL);
}
```

### 🍴 Pegando Garfos (Evitando Deadlock)

```c
void take_forks(t_philo *philo)
{
    // Filósofo ímpar: esquerda → direita
    // Filósofo par: direita → esquerda
    
    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(philo->right_fork);
        print_status(philo, "has taken a fork");
        
        pthread_mutex_lock(philo->left_fork);
        print_status(philo, "has taken a fork");
    }
    else
    {
        pthread_mutex_lock(philo->left_fork);
        print_status(philo, "has taken a fork");
        
        pthread_mutex_lock(philo->right_fork);
        print_status(philo, "has taken a fork");
    }
}

void put_forks(t_philo *philo)
{
    pthread_mutex_unlock(philo->left_fork);
    pthread_mutex_unlock(philo->right_fork);
}
```

### 🍝 Comendo

```c
void eat(t_philo *philo)
{
    // Atualizar timestamp (protegido)
    pthread_mutex_lock(&philo->data->death_lock);
    philo->last_meal = get_time_ms();
    philo->meals_count++;
    pthread_mutex_unlock(&philo->data->death_lock);
    
    // Imprimir
    print_status(philo, "is eating");
    
    // Dormir pelo tempo de comer
    precise_sleep(philo->data->time_to_eat);
}
```

### 💀 Monitorando Morte

```c
int check_if_dead(t_data *data)
{
    for (int i = 0; i < data->num_philos; i++)
    {
        pthread_mutex_lock(&data->death_lock);
        long time_since_meal = get_time_ms() - data->philos[i].last_meal;
        pthread_mutex_unlock(&data->death_lock);
        
        if (time_since_meal > data->time_to_die)
        {
            print_death(&data->philos[i]);
            
            pthread_mutex_lock(&data->death_lock);
            data->dead_flag = 1;
            pthread_mutex_unlock(&data->death_lock);
            
            return (1);
        }
    }
    return (0);
}
```

### 🖨️ Printf Thread-Safe

```c
void print_status(t_philo *philo, char *status)
{
    pthread_mutex_lock(&philo->data->write_lock);
    
    // Verificar se alguém morreu
    pthread_mutex_lock(&philo->data->death_lock);
    int dead = philo->data->dead_flag;
    pthread_mutex_unlock(&philo->data->death_lock);
    
    if (!dead)
    {
        long timestamp = get_time_ms() - philo->data->start_time;
        printf("%ld %d %s\n", timestamp, philo->id, status);
    }
    
    pthread_mutex_unlock(&philo->data->write_lock);
}
```

---

## 🎓 Resumo dos Conceitos

### Threads
- ✅ Múltiplas linhas de execução paralelas
- ✅ Compartilham memória do processo
- ✅ Mais leves que processos
- ✅ `pthread_create()` e `pthread_join()`

### Mutexes
- ✅ Protegem seções críticas
- ✅ Garantem exclusão mútua
- ✅ `pthread_mutex_lock()` e `unlock()`
- ✅ Um mutex por recurso compartilhado

### Race Conditions
- ⚠️ Resultado depende da ordem de execução
- ⚠️ Acontece sem mutexes
- ✅ Solução: proteger com mutexes

### Deadlocks
- ⚠️ Threads travam esperando umas pelas outras
- ⚠️ Ciclo de espera
- ✅ Solução: ordenar recursos, pares/ímpares

### Sincronização
- ✅ Coordenar threads
- ✅ Timestamps precisos (`gettimeofday`)
- ✅ Sleep preciso (`usleep`)
- ✅ Printf protegido

---

## 📚 Próximos Passos

1. **Experimente** criar threads simples
2. **Teste** race conditions (contador sem mutex)
3. **Implemente** um mutex simples
4. **Crie** um deadlock proposital (para entender)
5. **Comece** o philosophers aos poucos

### 🧪 Experimento Sugerido

Crie um arquivo `test_threads.c`:

```c
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *print_numbers(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < 5; i++)
    {
        printf("Thread %d: %d\n", id, i);
        usleep(100000);  // 100ms
    }
    return (NULL);
}

int main(void)
{
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    
    pthread_create(&t1, NULL, print_numbers, &id1);
    pthread_create(&t2, NULL, print_numbers, &id2);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Todas as threads terminaram!\n");
    return (0);
}

// Compile: gcc -pthread test_threads.c -o test
// Execute: ./test
```

Veja como as mensagens se intercalam! Isso é concorrência. 🎉

---

**Agora você entende os conceitos! Hora de aplicar no Philosophers! 🥢🍝**