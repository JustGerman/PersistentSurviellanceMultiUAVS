#pragma once
#include <bits/stdc++.h>
#include <iomanip>
#include <filesystem>

using namespace std;
using ll = long long;
namespace fs = std::filesystem;

struct Celda {
    char tipo;
    int urgencia;
    vector<int> movimientos;

    Celda();
};

class Grid {
private:
    int filas, cols;
    int n_obstaculos, n_urgencias, n_bases;
    vector<pair<int,int>> obstaculos;
    vector<tuple<int,int,int>> urgencias;
    vector<tuple<int,int,int>> bases;
    vector<vector<Celda>> grid;
    int urgenciaTotalInicial = 0;
    unordered_map<int, int> histUrgencias; // peso -> cantidad de celdas con ese peso

public:
    Grid(string instancia);

    void print() const;

    bool esRutaFactible(const vector<int>& basesIDs,
                        const vector<vector<int>>& movimientos,
                        int T) const;

    // Getters
    int getFilas() const;
    int getCols() const;
    int getNBases() const;
    int getUrgenciaTotalInicial() const;
    int getNUrgencias() const;
    const unordered_map<int, int>& getHistUrgencias() const;

    vector<pair<int,int>> getBases() const;
    pair<int,int> getBasePorID(int id) const;

    // Accesos
    char getTipo(int i, int j) const;
    int getUrgencia(int i, int j) const;
};
