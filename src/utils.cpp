#include "utils.hpp"
#include <bits/stdc++.h>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

//   Generar todas las combinaciones k×b
vector<vector<int>> generarTodasAsignaciones(int k, int b) {
    vector<vector<int>> todas;
    vector<int> cur(k, 0);

    function<void(int)> dfs = [&](int idx) {
        if (idx == k) {
            todas.push_back(cur);
            return;
        }
        for (int base = 0; base < b; ++base) {
            cur[idx] = base;
            dfs(idx + 1);
        }
    };
    dfs(0);
    return todas;
}

//   Seleccionar subset aleatorio
vector<vector<int>> seleccionarAsignacionesAleatorias(const vector<vector<int>>& todas, int n) {
    vector<vector<int>> copia = todas;
    random_device rd;
    mt19937 gen(rd());
    shuffle(copia.begin(), copia.end(), gen);
    if (n > (int)copia.size()) n = copia.size();
    copia.resize(n);
    return copia;
}

vector<vector<int>> generarSolucionInicial(const Grid& grid, int k, int T, const vector<int>& basesIDs) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<int> distDir(0, 8);

    const int F = grid.getFilas();
    const int C = grid.getCols();

    static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};

    vector<vector<int>> sol(k, vector<int>(T, 8));

    bool factible = false;
    int intentos = 0;
    const int MAX_INTENTOS = 300000;

    while (!factible && intentos++ < MAX_INTENTOS) {
        // Reiniciar solución
        for (int d = 0; d < k; ++d) {
            auto [r, c] = grid.getBasePorID(basesIDs[d]);
            for (int t = 0; t < T; ++t) {
                int dir;
                int ni, nj;
                int tries = 0;

                do {
                    dir = distDir(gen);
                    ni = r + dr[dir];
                    nj = c + dc[dir];
                    tries++;
                } while ((dir < 0 || dir > 8 ||
                          ni < 0 || nj < 0 || ni >= F || nj >= C ||
                          grid.getTipo(ni, nj) == 'O') &&
                          tries < 20);

                sol[d][t] = dir;
                r = ni;
                c = nj;
            }
        }

        // Validar ruta
        factible = grid.esRutaFactible(basesIDs, sol, T);
    }

    if (!factible) {
        cerr << "WARNING: No se pudo generar una solución inicial factible tras " 
             << MAX_INTENTOS << " intentos. Se usará la quieta.\n";
        sol.assign(k, vector<int>(T, 8));
    }

    return sol;
}

//   Exportar mapa y trayectorias a CSV
void exportarEscenario(const Grid& g, const vector<vector<int>>& mejorSolucionGlobal, const vector<int>& mejorAsignacion, int k, int T, const string& nombreArchivo)
{
    static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};

    // Crear carpeta output si no existe
    fs::create_directories("output");

    // 1. Exportar mapa
    string pathMapa = "output/" + nombreArchivo + "_mapa.csv";
    ofstream mapa(pathMapa);
    mapa << "fila,columna,tipo,urgencia\n";
    for (int i = 0; i < g.getFilas(); ++i)
        for (int j = 0; j < g.getCols(); ++j)
            mapa << i << "," << j << "," << g.getTipo(i,j) << "," << g.getUrgencia(i,j) << "\n";
    mapa.close();

    // 2. Exportar trayectorias
    string pathTray = "output/" + nombreArchivo + "_trayectorias.csv";
    ofstream tray(pathTray);
    tray << "dron,tick,fila,columna\n";

    for (int d = 0; d < k; ++d) {
        auto [r, c] = g.getBasePorID(mejorAsignacion[d]);
        for (int t = 0; t < T; ++t) {
            int dir = mejorSolucionGlobal[d][t];
            r += dr[dir];
            c += dc[dir];
            tray << d << "," << (t+1) << "," << r << "," << c << "\n";
        }
    }
    tray.close();

    cout << "Archivos exportados en carpeta 'output/': "
         << nombreArchivo << "_mapa.csv y "
         << nombreArchivo << "_trayectorias.csv\n";
}
