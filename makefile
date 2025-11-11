# === Proyecto PSP-UAV ===

# Compilador y opciones
CXX := g++
CXXFLAGS := -std=c++17 -Wall -O2 -pthread

# Archivos fuente (todos los .cpp del directorio actual)
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

# Nombre del ejecutable
TARGET := PSP-UAV

# Regla principal
all: $(TARGET)

# Enlazado final
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilación de cada .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJS) $(TARGET)

# Ejecutar ejemplo rápido
run:
	./$(TARGET) instancias/PSP-UAV_01_a.txt 3 100 1000 50

.PHONY: all clean run
