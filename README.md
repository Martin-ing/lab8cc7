# Laboratorio de Scheduling en C

## Descripción

Este laboratorio implementa y simula distintos algoritmos de planificación de procesos (CPU Scheduling) utilizando el lenguaje C.

El programa genera procesos aleatorios con tiempos de llegada (`arrival_time`) y ráfagas de CPU (`burst_time`), luego ejecuta múltiples algoritmos de scheduling y muestra:

- Orden de ejecución de procesos
- Eventos de llegada, inicio y finalización
- Métricas de rendimiento
- Diagramas de Gantt en ASCII

---

## Algoritmos Implementados

### FIFO (First In First Out)

Ejecuta los procesos en el orden en que llegan.

### Round Robin (RR)

Asigna CPU por turnos usando un **quantum** fijo.

### SJF (Shortest Job First)

Selecciona primero el proceso con menor ráfaga total.

### SRTF (Shortest Remaining Time First)

Versión expropiativa de SJF. Siempre ejecuta el proceso con menor tiempo restante.

---

## Estructura del Proyecto

```text
.
├── main.c
├── algorithms.c
├── algorithms.h
├── process.h
├── queue.c
├── queue.h
├── utils.c
├── utils.h
└── Makefile
```

---

## Explicación de Archivos

### `main.c`

Archivo principal del programa.

Responsabilidades:

- Genera entre 5 y 15 procesos aleatorios.
- Inicializa datos de cada proceso.
- Ejecuta todos los algoritmos.
- Reserva memoria para el arreglo del Gantt.
- Imprime diagramas de Gantt ASCII.

### `process.h`

Define la estructura `Process`, que almacena toda la información necesaria para simular cada proceso.

### `algorithms.c`

Contiene la implementación de los algoritmos de scheduling.

### `algorithms.h`

Contiene los prototipos de las funciones de scheduling.

### `queue.c / queue.h`

Implementa una cola simple usada principalmente por Round Robin.

### `utils.c / utils.h`

Contiene funciones auxiliares para copiar procesos, ordenar, registrar eventos y calcular métricas.

---

## Funciones del Proyecto

### Funciones en `main.c`

| Función | Descripción |
|---|---|
| `dummy_thread(void *arg)` | Función vacía usada para crear hilos asociados a los procesos. |
| `total_burst_time(Process *p, int n)` | Calcula la suma total de los tiempos de ráfaga de todos los procesos. |
| `print_gantt_chart(Process *p, int n, int *gantt, int gantt_len, const char *title)` | Imprime en consola un diagrama de Gantt en ASCII para el algoritmo ejecutado. |
| `main()` | Genera los procesos, ejecuta los algoritmos, imprime resultados y libera memoria. |

### Funciones en `algorithms.c`

| Función | Descripción |
|---|---|
| `simulate_fifo(Process *p, int n, time_t base, int *gantt, int *gantt_len)` | Simula el algoritmo FIFO y llena el arreglo de Gantt según el orden de llegada. |
| `simulate_rr(Process *p, int n, time_t base, int quantum, int *gantt, int *gantt_len)` | Simula Round Robin usando un quantum fijo y una cola de procesos listos. |
| `simulate_sjf(Process *p, int n, time_t base, int *gantt, int *gantt_len)` | Simula SJF seleccionando el proceso disponible con menor tiempo de ráfaga. |
| `simulate_srtf(Process *p, int n, time_t base, int *gantt, int *gantt_len)` | Simula SRTF seleccionando en cada instante el proceso con menor tiempo restante. |

### Funciones en `queue.c`

| Función | Descripción |
|---|---|
| `queue_init(Queue *q)` | Inicializa la cola colocando el frente y el final en la posición inicial. |
| `queue_empty(Queue *q)` | Verifica si la cola está vacía. |
| `enqueue(Queue *q, int value)` | Inserta un valor al final de la cola. |
| `dequeue(Queue *q)` | Extrae y retorna el valor ubicado al frente de la cola. |

### Funciones en `utils.c`

| Función | Descripción |
|---|---|
| `copy_processes(Process *dest, Process *src, int n)` | Copia los procesos originales y reinicia sus campos de simulación. |
| `sort_by_arrival(Process *p, int n)` | Ordena los procesos por tiempo de llegada y, en empate, por PID. |
| `format_virtual_time(time_t base_time, int sec, char *buf)` | Convierte el tiempo virtual de la simulación a un formato legible. |
| `log_arrival(Process *p, time_t base, int t)` | Imprime el evento de llegada de un proceso. |
| `log_arrivals(Process *p, int n, int current_time, time_t base)` | Registra todos los procesos que ya llegaron en el tiempo actual. |
| `log_start(Process *p, time_t base, int t)` | Imprime el evento de inicio de ejecución de un proceso. |
| `log_complete(Process *p, time_t base, int t)` | Imprime el evento de finalización de un proceso. |
| `compute_metrics(Process *p, int n)` | Calcula el waiting time y turnaround time de cada proceso. |
| `print_metrics(Process *p, int n)` | Imprime las métricas individuales y los promedios de rendimiento. |

---

## Compilación

Usando `make`:

```bash
make
```

O manualmente:

```bash
gcc main.c algorithms.c queue.c utils.c -o scheduler -lpthread
```

---

## Ejecución

```bash
./scheduler
```

---

## Salida Esperada

### Procesos Generados

```text
PID   Arrival   Burst
0     2         5
1     1         3
2     4         2
```

### Diagrama de Gantt

```text
--- Gantt Chart: FIFO ---

      1  2  3  4  5  6  7  8  9  10
p0:            *  *  *  *  *
p1:   *  *  *
p2:                           *  *
```

### Métricas

```text
Waiting Times: [0.00, 2.00, 4.00]
Avg Waiting Time: 2.00 seconds

Turnaround Times: [3.00, 7.00, 6.00]
Avg Turnaround Time: 5.33 seconds
```

---
