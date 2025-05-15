# Makefile для сборки проекта в папку build

# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -g -std=c11

# Исходные файлы
SRC_DIR = .
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Директория сборки
BUILD_DIR = build

# Целевой исполняемый файл
TARGET = $(BUILD_DIR)/searchtool

# Объектные файлы
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Основная цель
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Компиляция объектных файлов
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Создание директории сборки
$(BUILD_DIR):
	mkdir -p $@

# Очистка
clean:
	rm -rf $(BUILD_DIR)

# Фейковые цели
.PHONY: all clean
