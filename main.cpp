#include <bits/stdc++.h>
using namespace std;

struct Celda {
    char tipo;
    int urgencia;
    vector<int> movimientos;

    Celda() : tipo('.'), urgencia(0), movimientos(9, 0) {
        movimientos[8] = 1; 
    }
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

public:
    
    Grid(string instancia) {
        ifstream file(instancia);
        if (!file.is_open()) {
            cerr << "Error: no se pudo abrir el archivo " << instancia << endl;
            exit(1);
        }

        string word;
        file >> word >> filas;       // GRID_ROWS
        file >> word >> cols;        // GRID_COLS

        grid = vector<vector<Celda>>(filas, vector<Celda>(cols));

        // Obstáculos
        file >> word >> n_obstaculos;  // N_OBSTACLES
        for (int i = 0; i < n_obstaculos; ++i) {
            int r, c;
            file >> r >> c;
            obstaculos.push_back({r, c});
            grid[r][c].tipo = 'O';  // Obstáculo
        }

        // Urgencias
        file >> word >> n_urgencias;   // N_URGENCIES
        for (int i = 0; i < n_urgencias; ++i) {
            int r, c, u;
            file >> r >> c >> u;
            urgencias.push_back({r, c, u});
            grid[r][c].tipo = 'U';
            grid[r][c].urgencia = u;
            urgenciaTotalInicial += u;
        }

        // Bases
        file >> word >> n_bases;       // N_BASES
        for (int i = 0; i < n_bases; ++i) {
            int id, r, c;
            file >> id >> r >> c;
            bases.push_back({id, r, c});
            grid[r][c].tipo = 'B';
        }

        file.close();

        //CONSTRUIR EL ARREGLO DE MOVIMIENTOS

        int dr[9] = {-1, -1,  0,  1,  1,  1,  0, -1, 0}; // movimiento fila
        int dc[9] = { 0,  1,  1,  1,  0, -1, -1, -1, 0}; //movimiento columna

        for (int i = 0; i < filas; ++i) {
            for (int j = 0; j < cols; ++j) {

                for (int d = 0; d < 9; ++d) {
                    int ni = i + dr[d];
                    int nj = j + dc[d];

                    // verificar si está dentro de la grilla
                    if (ni < 0 || nj < 0 || ni >= filas || nj >= cols)
                        continue;

                    // si no es obstáculo, se puede mover
                    if (grid[ni][nj].tipo != 'O')
                        grid[i][j].movimientos[d] = 1;
                }

                // las bases siempre tienen urgencia 0
                if (grid[i][j].tipo == 'B')
                    grid[i][j].urgencia = 0;
            }
        }

    }

    // Mostrar la grilla en consola
    void print() const {
        cout << "Grid (" << filas << "x" << cols << ")\n";
        for (int i = 0; i < filas; ++i) {
            for (int j = 0; j < cols; ++j) {
                const Celda& c = grid[i][j];

                if (c.tipo == 'O') {
                    cout << " O "; // obstáculo
                }
                else if (c.tipo == 'B') {
                    cout << " B "; // base
                }
                else {
                    // mostrar urgencia para celdas libres o con urgencia
                    cout << setw(2) << c.urgencia << " ";
                }
            }
            cout << endl;
        }
    }

    // Indica si una ruta es factible (sin salir del mapa, sin obstáculos y sin colisiones)
    bool esRutaFactible(const vector<int>& basesIDs,
                        const vector<vector<int>>& movimientos,
                        int T) const
    {
        static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
        static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};

        int k = (int)basesIDs.size();
        vector<pair<int,int>> posiciones(k);

        // Posición inicial de cada dron (desde su base)
        for (int i = 0; i < k; ++i) {
            posiciones[i] = getBasePorID(basesIDs[i]);
        }

        // Simular tick por tick
        for (int t = 0; t < T; ++t) {
            // Mover cada dron
            for (int i = 0; i < k; ++i) {
                int dir = movimientos[i][t];
                auto [r, c] = posiciones[i];

                // Verificar que el movimiento sea permitido desde la celda actual
                if (dir < 0 || dir > 8 || grid[r][c].movimientos[dir] == 0)
                    return false;

                int nr = r + dr[dir];
                int nc = c + dc[dir];

                // Fuera del mapa
                if (nr < 0 || nc < 0 || nr >= filas || nc >= cols)
                    return false;

                // Obstáculo
                if (grid[nr][nc].tipo == 'O')
                    return false;

                posiciones[i] = {nr, nc};
            }

            // === Comprobación de colisiones entre drones ===
            // Ordenamos las posiciones para detectar duplicados
            vector<pair<int,int>> snapshot = posiciones;
            sort(snapshot.begin(), snapshot.end());
            for (int i = 1; i < k; ++i) {
                if (snapshot[i] == snapshot[i - 1])
                    return false; // dos drones en la misma celda
            }
        }

        // Si todos los ticks son válidos, la ruta es factible
        return true;
    }

    // Getters
    int getFilas() const { return filas; }
    int getCols() const { return cols; }
    int getNBases() const { return n_bases; }
    int getUrgenciaTotalInicial() const { return urgenciaTotalInicial; }
    int getNUrgencias() const {return n_urgencias;}

    
    vector<pair<int,int>> getBases() const {
        vector<pair<int,int>> res;
        for (auto& b : bases) {
            int id, r, c;
            tie(id, r, c) = b;
            res.push_back({r, c});
        }
        return res;
    }

    pair<int,int> getBasePorID(int id) const {
    for (auto& b : bases) {
        int bid, r, c;
        tie(bid, r, c) = b;
        if (bid == id)
            return {r, c};
    }
    cerr << "Error: no se encontró la base con ID " << id << endl;
    exit(1);
}


    // Accesos
    char getTipo(int i, int j) const { return grid[i][j].tipo; }
    int getUrgencia(int i, int j) const { return grid[i][j].urgencia; }


};

// Estructura de movimiento tabú (mismo tipo que en tabuSearch)
struct MovimientoTabu { int i, t, mov; };

vector<vector<vector<int>>> generarVecinos(const vector<vector<int>>& sol, const Grid& grid, int k, int T, const vector<int>& basesIDs, const deque<MovimientoTabu>& listaTabu, int mejorValor) {
    vector<vector<vector<int>>> vecinos;

    for (int i = 0; i < k; ++i) {
        for (int t = 0; t < T; ++t) {
            int movActual = sol[i][t];
            for (int d = 0; d < 9; ++d) {
                if (d == movActual) continue;

                bool esTabu = false;
                for (auto& m : listaTabu)
                    if (m.i == i && m.t == t && m.mov == d) {
                        esTabu = true;
                        break;
                    }
                if (esTabu) continue;

                vector<vector<int>> nuevo = sol;
                nuevo[i][t] = d;

                if (grid.esRutaFactible(basesIDs, nuevo, T))
                    vecinos.push_back(move(nuevo));
            }
        }
    }
    return vecinos;
}


int funcionEvaluacion(const vector<vector<int>>& sol, const Grid& grid, int k, int T, const vector<int>& basesIDs) {
    const int F = grid.getFilas();
    const int C = grid.getCols();

    int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};

    // Registrar última visita y urgencia inicial solo para las celdas visitadas
    unordered_map<long long, pair<int,int>> visitadas; // key -> {u0, t_ultima}
    visitadas.reserve(grid.getNUrgencias() * 2);

    for (int d = 0; d < k; ++d) {
        auto [r, c] = grid.getBasePorID(basesIDs[d]);
        for (int t = 0; t < T; ++t) {
            int dir = sol[d][t];
            r += dr[dir];
            c += dc[dir];
            if (r < 0 || c < 0 || r >= F || c >= C)
                break;

            int u0 = grid.getUrgencia(r, c);
            if (u0 > 0) {
                long long key = 1LL * r * C + c;
                visitadas[key] = {u0, t + 1}; // tick t+1 = 1..T
            }
        }
    }

    int nUrgencias = grid.getNUrgencias();
    int nVisitadas = (int)visitadas.size();

    long long sumaUltimas = 0; // Σ (T - t_ult)
    long long sumaU0tUlt  = 0; // Σ (u0 + t_ult) para equivalencia
    for (auto& kv : visitadas) {
        int u0 = kv.second.first;
        int tUlt = kv.second.second;
        sumaUltimas += (T - tUlt);
        sumaU0tUlt  += (u0 + tUlt);
    }

    // === Tu fórmula directa ===
    int f1 = grid.getUrgenciaTotalInicial()
                 + (long long)T * (nUrgencias - nVisitadas)
                 + sumaUltimas;

    // === O la forma algebraicamente equivalente ===
    int f2 = grid.getUrgenciaTotalInicial()
                 + (long long)T * nUrgencias
                 - sumaU0tUlt;

    // Ambas deben coincidir; puedes usar una sola.
    return (int)f2;
}

struct ResultadoTabu {
    vector<vector<int>> mejorSolucion;
    int mejorValor;
};

ResultadoTabu tabuSearch(const Grid& grid, vector<vector<int>> solucionInicial, int iterMax, int tabuTenencia, int k, int T, const vector<int>& bases) {
    vector<vector<int>> solActual = solucionInicial;
    vector<vector<int>> mejorSol  = solucionInicial;

    int valorActual = funcionEvaluacion(solActual, grid, k, T, bases);
    int mejorValor  = valorActual;

    deque<MovimientoTabu> listaTabu;

    for (int iter = 0; iter < iterMax; ++iter) {

        vector<vector<vector<int>>> vecinos =
            generarVecinos(solActual, grid, k, T, bases, listaTabu, mejorValor);

        vector<vector<int>> mejorVecino = solActual;
        int mejorValorVecino = INT_MAX;
        int i_cambio=-1, t_cambio=-1, mov_cambio=-1;

        for (auto& vecino : vecinos) {
            int val = funcionEvaluacion(vecino, grid, k, T, bases);
            if (val < mejorValorVecino) {
                mejorValorVecino = val;
                mejorVecino = vecino;

                for (int i = 0; i < k; ++i)
                    for (int t = 0; t < T; ++t)
                        if (vecino[i][t] != solActual[i][t]) {
                            i_cambio = i;
                            t_cambio = t;
                            mov_cambio = vecino[i][t];
                            goto encontrado;
                        }
                encontrado:;
            }
        }

        if (mejorValorVecino == INT_MAX) break; // sin vecinos válidos

        auto solAnterior = solActual;
        solActual = mejorVecino;
        valorActual = mejorValorVecino;

        if (i_cambio != -1) {
            listaTabu.push_back({i_cambio, t_cambio, mov_cambio});
            if ((int)listaTabu.size() > tabuTenencia)
                listaTabu.pop_front();
        }

        if (valorActual < mejorValor) {
            mejorValor = valorActual;
            mejorSol = solActual;
            cout << "Aspiración activada en iter " << iter << endl;
        }

        cout << "[Iter " << iter << "] Valor actual: " << valorActual
             << " | Mejor: " << mejorValor << endl;
    }

    return {mejorSol, mejorValor};
}


int main() {
    Grid g("instancias/PSP-UAV_01_a.txt");
    g.print();

    cout << "Cantidad de bases: " << g.getNBases() << endl;

    

    return 0;
}