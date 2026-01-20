# Philosophers - Guia Didático Completo

```
Uma **thread** é um **fluxo de controle** (ou fluxo de execução) independente dentro de um processo, permitindo que um único programa execute múltiplas tarefas concorrentemente, compartilhando recursos como memória, mas com sua própria pilha de execução e contador de programa. É a **unidade básica** que o sistema operacional agenda para execução na CPU, tornando aplicativos mais responsivos e eficientes.
```

## 📚 O que é o Problema dos Filósofos?

O **Dining Philosophers Problem** (Problema do Jantar dos Filósofos) é um clássico problema de ciência da computação criado por Edsger Dijkstra em 1965. Ele ilustra os desafios de **sincronização** e **concorrência** em sistemas computacionais.

### 🎯 Cenário

Imagine uma mesa redonda com filósofos que:
- **Só fazem 3 coisas**: comer 🍝, pensar 🤔, dormir 😴
- **Não podem fazer duas coisas ao mesmo tempo**
- Precisam de **2 garfos** para comer (um na esquerda, outro na direita)
- Morrem se ficarem muito tempo sem comer ⚰️

```
        Filósofo 1
           🍴
    🍴  👨  👨  🍴
       👨  🍝  👨
    🍴  👨  👨  🍴
           🍴
        Filósofo N
```

---

## 🎮 Regras do Jogo

### Argumentos do Programa
```bash
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

| Argumento | Descrição | Exemplo |
|-----------|-----------|---------|
| `number_of_philosophers` | Quantidade de filósofos (e garfos) | `5` |
| `time_to_die` | Tempo máximo sem comer (ms) | `800` |
| `time_to_eat` | Tempo para comer (ms) | `200` |
| `time_to_sleep` | Tempo para dormir (ms) | `200` |
| `number_of_times_each_philosopher_must_eat` | (Opcional) Vezes que cada um deve comer | `7` |

**Exemplo**: `./philo 5 800 200 200`
- 5 filósofos
- Morrem se não comerem em 800ms
- Levam 200ms para comer
- Dormem por 200ms

---

## 🧠 Conceitos Fundamentais

### 1. **Threads** (Filósofos)
Cada filósofo é uma **thread** independente que executa em paralelo:

```c
// Pseudo-código
void *philosopher_routine(void *arg)
{
    while (simulation_running)
    {
        think();
        take_forks();
        eat();
        put_forks();
        sleep();
    }
}
```

### 2. **Mutexes** (Garfos)
Cada garfo é protegido por um **mutex** para evitar que dois filósofos peguem o mesmo garfo:

```c
pthread_mutex_t fork[5];  // 5 garfos para 5 filósofos

// Pegar garfo = lock
pthread_mutex_lock(&fork[left]);
pthread_mutex_lock(&fork[right]);

// Soltar garfo = unlock
pthread_mutex_unlock(&fork[left]);
pthread_mutex_unlock(&fork[right]);
```

### 3. **Race Conditions** (Condições de Corrida)
**Problema**: Dois filósofos tentam pegar o mesmo garfo ao mesmo tempo!

```
Filósofo 1: "Vou pegar o garfo da esquerda!"
Filósofo 2: "Vou pegar o garfo da direita!" (que é o mesmo!)
💥 CONFLITO!
```

**Solução**: Usar mutexes para garantir acesso exclusivo.

### 4. **Deadlock** (Impasse)
O pior cenário! Todos pegam o garfo da esquerda ao mesmo tempo:

```
Filósofo 1: 🍴 (esquerda) - esperando direita
Filósofo 2: 🍴 (esquerda) - esperando direita
Filósofo 3: 🍴 (esquerda) - esperando direita
Filósofo 4: 🍴 (esquerda) - esperando direita
Filósofo 5: 🍴 (esquerda) - esperando direita
```

**Todos ficam esperando para sempre!** 🔒

---

## 💡 Desafios Principais

### 1. **Evitar Deadlock**
**Soluções possíveis**:
- Filósofos ímpares pegam esquerda primeiro, pares pegam direita primeiro
- Limitar quantos filósofos podem tentar comer ao mesmo tempo
- Fazer último filósofo pegar garfos em ordem inversa

### 2. **Evitar Starvation (Inanição)**
Um filósofo não pode ficar eternamente sem comer enquanto outros comem:
```
Filósofo 1: 🍝🍝🍝🍝🍝 (comendo sempre)
Filósofo 2: ⏳⏳⏳⏳💀 (morrendo de fome)
```

### 3. **Sincronização de Mensagens**
```
142 1 has taken a fork
142 1 has taken a fork
142 1 is eating
342 1 is sleeping
         ↑
    timestamp preciso!
```

**Regras**:
- Mensagens não podem se sobrepor
- Morte deve ser anunciada em até 10ms
- Timestamp deve ser preciso (em ms desde o início)

### 4. **Monitoramento de Morte**
Como saber se um filósofo morreu?

```c
// Verificar constantemente
last_meal_time = get_current_time();
// ...
if (current_time - last_meal_time > time_to_die)
    philosopher_died();
```

---

## 🏗️ Estrutura da Solução

### Estruturas de Dados Típicas

```c
typedef struct s_philo
{
    int             id;                 // 1 a N
    int             times_eaten;        // Contador de refeições
    long            last_meal_time;     // Timestamp última refeição
    pthread_t       thread;             // Thread do filósofo
    pthread_mutex_t *left_fork;         // Ponteiro para garfo esquerdo
    pthread_mutex_t *right_fork;        // Ponteiro para garfo direito
    struct s_data   *data;              // Dados compartilhados
}   t_philo;

typedef struct s_data
{
    int             num_philos;
    long            time_to_die;
    long            time_to_eat;
    long            time_to_sleep;
    int             must_eat_count;     // -1 se não especificado
    long            start_time;
    int             someone_died;       // Flag de morte
    pthread_mutex_t *forks;             // Array de mutexes
    pthread_mutex_t write_mutex;        // Para printf sincronizado
    pthread_mutex_t death_mutex;        // Para flag de morte
    t_philo         *philos;            // Array de filósofos
}   t_data;
```

### Fluxo Principal

```c
int main(int argc, char **argv)
{
    t_data data;
    
    // 1. Validar e parsear argumentos
    parse_arguments(argc, argv, &data);
    
    // 2. Inicializar mutexes (garfos)
    init_forks(&data);
    
    // 3. Criar filósofos e suas threads
    create_philosophers(&data);
    
    // 4. Monitorar morte (thread principal ou separada)
    monitor_death(&data);
    
    // 5. Aguardar threads terminarem
    join_threads(&data);
    
    // 6. Limpar recursos
    cleanup(&data);
    
    return (0);
}
```

---

## ⚠️ Armadilhas Comuns

### 1. **Data Races**
```c
// ❌ ERRADO - não protegido
philosopher->times_eaten++;

// ✅ CORRETO - protegido
pthread_mutex_lock(&data->meal_mutex);
philosopher->times_eaten++;
pthread_mutex_unlock(&data->meal_mutex);
```

### 2. **Printf Não Sincronizado**
```c
// ❌ ERRADO - mensagens embaralhadas
printf("%ld %d is eating\n", timestamp, id);

// ✅ CORRETO - um printf por vez
pthread_mutex_lock(&data->write_mutex);
printf("%ld %d is eating\n", timestamp, id);
pthread_mutex_unlock(&data->write_mutex);
```

### 3. **Tempo Impreciso**
```c
// ❌ ERRADO - sleep() não é preciso
sleep(time_to_eat / 1000);

// ✅ CORRETO - usleep() em microsegundos
usleep(time_to_eat * 1000);

// 🌟 MELHOR - busy waiting para precisão máxima
long start = get_time_ms();
while (get_time_ms() - start < time_to_eat)
    usleep(100);
```

### 4. **Verificação de Morte Tardia**
```c
// ❌ Verificar só de vez em quando
sleep(1); check_death();

// ✅ Verificar frequentemente
while (!should_stop())
{
    check_all_philosophers();
    usleep(1000); // Verifica a cada 1ms
}
```

---

## 🎓 Conceitos de Sincronização

### Mutex (Mutual Exclusion)
```c
pthread_mutex_t mutex;
pthread_mutex_init(&mutex, NULL);

pthread_mutex_lock(&mutex);    // Trava - só uma thread por vez
// SEÇÃO CRÍTICA
pthread_mutex_unlock(&mutex);  // Libera

pthread_mutex_destroy(&mutex);
```

### Timestamp Preciso
```c
long get_time_ms(void)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long get_timestamp(t_data *data)
{
    return (get_time_ms() - data->start_time);
}
```

---

## 🧪 Casos de Teste

### Teste 1: Não deve morrer
```bash
./philo 5 800 200 200
# 5 filósofos, 800ms para morrer, 200ms comer, 200ms dormir
# Ninguém deve morrer!
```

### Teste 2: Deve morrer
```bash
./philo 4 310 200 100
# Muito apertado! 310ms para morrer, mas come por 200ms
# Alguém deve morrer!
```

### Teste 3: Um filósofo
```bash
./philo 1 800 200 200
# Só 1 garfo disponível - deve morrer (não pode comer)
```

### Teste 4: Parar após N refeições
```bash
./philo 5 800 200 200 7
# Para quando todos comerem 7 vezes
```

### Teste 5: Muitos filósofos
```bash
./philo 200 800 200 200
# Teste de performance e deadlock
```

---

## 📋 Checklist de Implementação

- [ ] Parsing de argumentos
- [ ] Validação de entrada (números positivos, etc)
- [ ] Inicialização de estruturas
- [ ] Criação de mutexes para garfos
- [ ] Criação de threads (filósofos)
- [ ] Rotina do filósofo (eat, sleep, think)
- [ ] Pegar/soltar garfos com mutexes
- [ ] Sistema de logging com timestamps
- [ ] Sincronização de printf
- [ ] Monitoramento de morte
- [ ] Condição de parada (morte ou N refeições)
- [ ] Join de threads
- [ ] Destruição de mutexes
- [ ] Liberação de memória
- [ ] Teste sem leaks (valgrind)
- [ ] Teste de data races (valgrind --tool=helgrind)

---

## 🛠️ Funções Autorizadas

```c
// Memória
void *memset(void *s, int c, size_t n);
void *malloc(size_t size);
void free(void *ptr);

// I/O
int printf(const char *format, ...);

// Tempo
int usleep(useconds_t usec);              // Dormir em microsegundos
int gettimeofday(struct timeval *tv, ...); // Pegar timestamp

// Threads
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
int pthread_join(pthread_t thread, void **retval);
int pthread_detach(pthread_t thread);

// Mutexes
int pthread_mutex_init(pthread_mutex_t *mutex, ...);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

// Write (para mensagens de erro)
ssize_t write(int fd, const void *buf, size_t count);
```

---

## 💻 Exemplo de Uso

```bash
# Compilar
make

# Executar
./philo 5 800 200 200

# Saída esperada:
0 1 has taken a fork
0 1 has taken a fork
0 1 is eating
0 3 has taken a fork
0 3 has taken a fork
0 3 is eating
200 1 is sleeping
200 3 is sleeping
200 2 has taken a fork
...
```

---

## 🎯 Objetivo Final

Criar um programa que:
1. ✅ Nunca tenha deadlock
2. ✅ Nunca tenha data races
3. ✅ Filósofos não morram (a menos que devam)
4. ✅ Timestamps sejam precisos
5. ✅ Mensagens não se sobreponham
6. ✅ Morte seja detectada em até 10ms
7. ✅ Não tenha memory leaks

---

## 📚 Recursos Adicionais

- **Man pages**: `man pthread_create`, `man pthread_mutex_init`
- **Visualização**: Desenhe a mesa circular no papel
- **Debug**: Use `printf` com mutexes para debug
- **Valgrind**: `valgrind --leak-check=full ./philo ...`
- **Helgrind**: `valgrind --tool=helgrind ./philo ...` (detecta data races)

---

## 🤔 Perguntas para Reflexão

1. Por que usar threads e não processos?
2. O que acontece se esquecer um `pthread_mutex_unlock()`?
3. Como garantir que a morte seja detectada rapidamente?
4. Por que o último filósofo pega garfos em ordem diferente?
5. Como evitar que printf() misture mensagens?

**Boa sorte, jovem padawan! 🥢**
