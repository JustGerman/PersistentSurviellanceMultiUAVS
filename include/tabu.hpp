#pragma once
#include "grid.hpp"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

//Estructura del resultado
struct ResultadoTabu {
    vector<vector<int>> mejorSol;  // mejor matriz k×T
    ll mejorValor;                 // costo mínimo alcanzado
};

//         Clase TabuSearch
class TabuSearch {
private:
    const Grid& grid;          // referencia al entorno
    int iterMax;               // máximo de iteraciones
    int tabuTenencia;          // tamaño de lista tabú
    int k;                     // cantidad de drones
    int T;                     // duración total
    vector<int> basesIDs;      // bases iniciales de los drones

    // Lista tabú que guarda hashes de soluciones completas
    deque<size_t> listaTabu;

    //     Métodos internos
    size_t hashSolucion(const vector<vector<int>>& matriz) const;
    bool esTabu(const vector<vector<int>>& matriz) const;
    void agregarTabu(const vector<vector<int>>& matriz);

    vector<vector<vector<int>>> generarVecinos(const vector<vector<int>>& sol);
    ll funcionEvaluacion(const vector<vector<int>>& sol) const;

public:
    TabuSearch(const Grid& g, int iterMax, int tabuTenencia, int k, int T, const vector<int>& basesIDs);
    ResultadoTabu ejecutar(const vector<vector<int>>& solInicial);
};
