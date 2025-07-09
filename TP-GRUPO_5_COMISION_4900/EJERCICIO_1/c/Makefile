CC = gcc
CFLAGS = -Wall -I./include
SRC_DIR = src
BUILD_DIR = build

# Archivos fuente y objetos
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Nombre del ejecutable
TARGETS = kitchen kitchen_monitor process_monitor
UTILS_OBJ = $(SRC_DIR)/kitchen_utils.o

.PHONY: all clean run

all: $(TARGETS)

$(UTILS_OBJ): $(SRC_DIR)/kitchen_utils.c include/kitchen.h
	$(CC) $(CFLAGS) -c -o $@ $<

kitchen: $(SRC_DIR)/kitchen.c $(UTILS_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

kitchen_monitor: $(SRC_DIR)/kitchen_monitor.c $(UTILS_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

process_monitor: $(SRC_DIR)/process_monitor.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGETS) $(UTILS_OBJ)
	ipcs -m | grep $(USER) | awk '{print $$2}' | xargs -n1 ipcrm -m 2>/dev/null || true
	ipcs -s | grep $(USER) | awk '{print $$2}' | xargs -n1 ipcrm -s 2>/dev/null || true

run: $(TARGETS)
	./kitchen
	./kitchen_monitor
	./process_monitor

# Reglas para monitoreo
.PHONY: monitor-ipcs monitor-ps

monitor-ipcs:
	@echo "=== Memoria Compartida ==="
	@ipcs -m
	@echo "\n=== Semáforos ==="
	@ipcs -s

monitor-ps:
	@echo "=== Procesos del Sistema ==="
	@ps aux | grep kitchen | grep -v grep 