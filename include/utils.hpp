#pragma once
#include "grid.hpp"
#include "tabu.hpp"
#include <vector>
#include <string>
using namespace std;

// ==============================
//     Generación de asignaciones
// ==============================
vector<vector<int>> generarTodasAsignaciones(int k, int b);
vector<vector<int>> seleccionarAsignacionesAleatorias(const vector<vector<int>>& todas, int n);

#pragma once
#include "grid.hpp"
#include "tabu.hpp"
#include <vector>
#include <string>
using namespace std;

// ==============================
//     Generación de asignaciones
// ==============================
vector<vector<int>> generarTodasAsignaciones(int k, int b);
vector<vector<int>> seleccionarAsignacionesAleatorias(const vector<vector<int>>& todas, int n);

// ==============================
//     Generación de solución inicial
// ==============================
vector<vector<int>> generarSolucionInicial(const Grid& grid, int k, int T, const vector<int>& basesIDs);
vector<vector<int>> generarSolucionInicialInfactible(const Grid& g, int k, int T, const vector<int>& basesIDs);

// ==============================
//     Exportación de resultados
// ==============================
void exportarEscenario(const Grid& g,
                       const vector<vector<int>>& mejorSolucionGlobal,
                       const vector<int>& mejorAsignacion,
                       int k, int T,
                       const string& nombreArchivo);


// ==============================
//     Exportación de resultados
// ==============================
void exportarEscenario(const Grid& g, const vector<vector<int>>& mejorSolucionGlobal, const vector<int>& mejorAsignacion, int k, int T, const string& nombreArchivo);
