# TP - Sistemas Operativos (2025)

## 💻 Requisitos
- Docker instalado (y opcionalmente Colima si usás macOS)
- GCC (para compilar programas en C)
- Java (para ejecutar clientes desde el host en el Ejercicio 2)

---

## 🧪 Ejercicio 1 – C (Procesos, semáforos, memoria compartida)

### 📂 Ubicación
`SIS_OPS/EJ_1`

### 🚀 Build y ejecución

```bash
cd SIS_OPS/EJ_1
docker build -t tp-so .
docker run -it --rm -v $(pwd):/workspace tp-so
```

### ⚙️ Dentro del contenedor

Compilar y ejecutar los programas:

```bash
# Comparación entre ejecución secuencial y con hilos
gcc vector_benchmark.c -o vector_benchmark -pthread
./vector_benchmark

# Simulación de llamadas API con espera (threads paralelos)
gcc simulador_api.c -o api -pthread
./api

# Otros ejemplos opcionales
gcc vector_paralelo.c -o vector_paralelo -pthread
./vector_paralelo

gcc tp_debug.c -o tp_debug -pthread
./tp_debug
```

### 🧪 Comandos útiles dentro del contenedor

```bash
ps aux           # Ver procesos activos
htop             # Visor interactivo (si está instalado)
ipcs -m          # Ver memoria compartida
ipcs -s          # Ver semáforos
nproc            # Ver cantidad de núcleos disponibles
exit             # Salir del contenedor
```

---

## 🌐 Ejercicio 2 – Java (Sockets, hilos, semáforo de conteo)

### 📂 Ubicación
`SIS_OPS/EJ_2`

### 🚀 Build y ejecución del servidor

```bash
cd SIS_OPS/EJ_2
docker build -t servidor-java .
docker run -it --rm -p 4000:4000 servidor-java
```

### 💬 Ejecutar clientes (desde el host)

```bash
javac Cliente.java
```

Abrí múltiples terminales y ejecutá:

```bash
java Cliente
```

📌 Solo 4 clientes se conectan al mismo tiempo. El resto queda en espera hasta que haya lugar (controlado por un `Semaphore` en el servidor).

---

## 🧼 Limpieza (opcional)

```bash
docker ps -a               # Ver contenedores activos
docker image ls            # Ver imágenes
docker rmi <imagen>        # Eliminar imagen
docker builder prune -f    # Limpiar cache de Docker
```

---

## ✍️ Autor
Eliseo S. – Trabajo Práctico Sistemas Operativos 2025
