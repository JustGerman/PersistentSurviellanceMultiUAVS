# === Proyecto PSP-UAV (versión modular) ===

# Compilador y opciones
CXX := g++
CXXFLAGS := -std=c++17 -Wall -O2 -pthread
INCLUDE := -Iinclude

# Directorios
SRC_DIR := src
OBJ_DIR := obj
TARGET := PSP-UAV

# Buscar todos los .cpp dentro de src/
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# =========================================
#                Reglas
# =========================================

all: $(TARGET)

# Enlazado final
$(TARGET): $(OBJS)
	@echo "🔗 Enlazando ejecutable $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilar cada .cpp a .o dentro de obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🧩 Compilando $< ..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# Crear carpeta obj si no existe
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# =========================================
#              Reglas útiles
# =========================================

run: $(TARGET)
	@echo "🚀 Ejecutando ejemplo..."
	./$(TARGET) instancias/PSP-UAV_03_a.txt 5 200 300 30

clean:
	@echo "🧹 Limpiando compilación..."
	rm -rf $(OBJ_DIR) $(TARGET)

rebuild: clean all

.PHONY: all clean run rebuild
