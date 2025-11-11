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
    unordered_map<int, int> histUrgencias; // peso -> cantidad de celdas con ese peso


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
            histUrgencias[u]++;
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
    bool esRutaFactible(const vector<int>& basesIDs, const vector<vector<int>>& movimientos, int T) const {
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

                posiciones[i] = {nr, nc};
            }

            // Hay colisiones entre los drones?
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
    const unordered_map<int, int>& getHistUrgencias() const { return histUrgencias; }


    
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


ll funcionEvaluacion(const vector<vector<int>>& sol, const Grid& grid, int k, int T, const vector<int>& basesIDs){
    const int F = grid.getFilas();
    const int C = grid.getCols();
    static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};

    // Obtener histograma (peso -> cantidad de celdas)
    const auto& hist = grid.getHistUrgencias();

    // Mapas auxiliares
    // Guardaremos la última visita a cada celda urgente
    unordered_map<long long, int> ultimaVisita; // key -> tick t_últ
    ultimaVisita.reserve(grid.getNUrgencias() * 2);

    // Recorrer todas las rutas de los drones
    for (int d = 0; d < k; ++d) {
        auto [r, c] = grid.getBasePorID(basesIDs[d]);
        for (int t = 0; t < T; ++t) {
            int dir = sol[d][t];
            r += dr[dir];
            c += dc[dir];

            if (r < 0 || c < 0 || r >= F || c >= C)
                break;

            int u = grid.getUrgencia(r, c); // peso base de la celda
            if (u > 0) {
                long long key = (1LL * r << 32) | c;
                ultimaVisita[key] = t + 1; // sobrescribe con la última visita
            }
        }
    }

    // Agrupar últimas visitas por peso
    unordered_map<int, vector<int>> visitasPorPeso; // peso -> {t_últ}
    visitasPorPeso.reserve(hist.size() * 2);

    for (auto& [key, t_ult] : ultimaVisita) {
        int r = key >> 32;
        int c = key & 0xFFFFFFFF;
        int u = grid.getUrgencia(r, c);
        visitasPorPeso[u].push_back(t_ult);
    }

    // Calcular urgencia acumulada total
    // Fórmula: w * [(N_w - N_{v,w}) * T(T+1)/2  +  Σ_{t_v∈visitas(w)} t_v(t_v+1)/2]
    ll total = 0;
    const long long coef_T = 1LL * T * (T + 1) / 2;

    for (auto& [w, N_w] : hist) {
        const auto& visitas = visitasPorPeso[w];
        int N_v = (int)visitas.size();

        // Celdas no visitadas
        total += 1LL * (N_w - N_v) * w * coef_T;

        // Celdas visitadas
        for (int t_v : visitas)
            total += 1LL * w * (1LL * t_v * (t_v + 1) / 2);
    }

    return total; // costo acumulado total de urgencias
}

struct ResultadoTabu {
    vector<vector<int>> mejorSol;
    ll mejorValor;
};

ResultadoTabu tabuSearch(const Grid& grid, vector<vector<int>> solucionInicial, int iterMax, int tabuTenencia, int k, int T, const vector<int>& bases) {
    vector<vector<int>> solActual = solucionInicial;
    vector<vector<int>> mejorSol  = solucionInicial;

    ll valorActual = funcionEvaluacion(solActual, grid, k, T, bases);
    ll mejorValor  = valorActual;

    deque<MovimientoTabu> listaTabu;

    for (int iter = 0; iter < iterMax; ++iter) {

        vector<vector<vector<int>>> vecinos = generarVecinos(solActual, grid, k, T, bases, listaTabu, mejorValor);

        vector<vector<int>> mejorVecino = solActual;
        ll mejorValorVecino = LLONG_MAX;
        int i_cambio=-1, t_cambio=-1, mov_cambio=-1;

        for (auto& vecino : vecinos) {
            ll val = funcionEvaluacion(vecino, grid, k, T, bases);
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

        if (mejorValorVecino == LLONG_MAX) break; // sin vecinos válidos

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

void exportarEscenario(const Grid& g, const vector<vector<int>>& mejorSolucionGlobal, const vector<int>& mejorAsignacion, int k, int T, const string& nombreArchivo)
{
    static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};

    // Crear carpeta output si no existe
    fs::create_directories("output");

    // 1. Exportar mapa
    ofstream mapa("output/mapa.csv");
    mapa << "fila,columna,tipo,urgencia\n";
    for (int i = 0; i < g.getFilas(); ++i)
        for (int j = 0; j < g.getCols(); ++j)
            mapa << i << "," << j << "," << g.getTipo(i,j) << "," << g.getUrgencia(i,j) << "\n";
    mapa.close();

    // 2. Exportar trayectorias
    ofstream tray("output/trayectorias.csv");
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

    cout << "Archivos exportados en carpeta 'output/': mapa.csv y trayectorias.csv\n";
}

// declaradas en tu código
vector<vector<int>> generarSolucionInicial(const Grid& g, int k, int T, const vector<int>& bases);
ResultadoTabu tabuSearch(const Grid& g, vector<vector<int>> solInicial, int iterMax, int tabuTenencia, int k, int T, const vector<int>& asignacion);

// Generar TODAS las combinaciones posibles drones→bases
vector<vector<int>> generarTodasAsignaciones(int k, int b) {
    vector<vector<int>> todas;
    vector<int> cur(k, 0);
    function<void(int)> dfs = [&](int idx) {
        if (idx == k) { todas.push_back(cur); return; }
        for (int base = 0; base < b; ++base) {
            cur[idx] = base;
            dfs(idx + 1);
        }
    };
    dfs(0);
    return todas;
}

// Seleccionar un subconjunto aleatorio sin repetición
vector<vector<int>> seleccionarAsignacionesAleatorias(const vector<vector<int>>& todas, int n) {
    vector<vector<int>> copia = todas;
    random_device rd;
    mt19937 gen(rd());
    shuffle(copia.begin(), copia.end(), gen);
    if (n > (int)copia.size()) n = copia.size();
    copia.resize(n);
    return copia;
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    srand(time(nullptr));

    if (argc != 6) {
        cerr << "Uso: " << argv[0]
             << " <archivo_instancia> <k_drones> <T_ticks> <iterMax> <tabuTenencia>\n";
        return 1;
    }

    // === Leer parámetros desde línea de comandos ===
    string archivoInstancia = argv[1];
    int k = stoi(argv[2]);
    int T = stoi(argv[3]);
    int iterMax = stoi(argv[4]);
    int tabuTenencia = stoi(argv[5]);

    // === Mostrar parámetros ===
    cout << "================= PARÁMETROS DE EJECUCIÓN =================\n";
    cout << "Archivo instancia : " << archivoInstancia << "\n";
    cout << "Cantidad de drones (k): " << k << "\n";
    cout << "Duración misión (T): " << T << "\n";
    cout << "Iteraciones de Tabu Search: " << iterMax << "\n";
    cout << "Tamaño de lista tabú: " << tabuTenencia << "\n";
    cout << "============================================================\n\n";

    // 1. Cargar instancia
    Grid g(archivoInstancia);
    g.print();
    cout << "Cantidad de bases: " << g.getNBases() << endl;

    int b = g.getNBases();       // número de bases
    int N_ASIGNACIONES = 30;     // cuántas combinaciones probar (únicas y random)

    // 2. Generar TODAS las combinaciones y tomar un subset aleatorio
    auto todas = generarTodasAsignaciones(k, b);
    cout << "\nTotal de combinaciones posibles: " << todas.size() << endl;
    auto subset = seleccionarAsignacionesAleatorias(todas, N_ASIGNACIONES);
    cout << "Probando " << subset.size() << " asignaciones aleatorias únicas en paralelo...\n";

    // 3. Ejecutar en paralelo
    vector<future<pair<vector<int>, ResultadoTabu>>> tareas;
    for (auto& asignacion : subset) {
        tareas.push_back(async(launch::async, [&g, asignacion, k, T, iterMax, tabuTenencia]() {
            cout << "\nAsignación: ";
            for (int id : asignacion) cout << id << " ";
            cout << endl;

            vector<vector<int>> solInicial = generarSolucionInicial(g, k, T, asignacion);
            ResultadoTabu res = tabuSearch(g, solInicial, iterMax, tabuTenencia, k, T, asignacion);
            return make_pair(asignacion, res);
        }));
    }

    // 4. Recolectar resultados
    long long mejorValorGlobal = LLONG_MAX;
    vector<vector<int>> mejorSolucionGlobal;
    vector<int> mejorAsignacion;

    for (auto& t : tareas) {
        auto [asign, res] = t.get();
        if (res.mejorValor < mejorValorGlobal) {
            mejorValorGlobal = res.mejorValor;
            mejorSolucionGlobal = res.mejorSol;
            mejorAsignacion = asign;
        }
    }

    // 5. Mostrar resultado final
    cout << "\n==================== RESULTADO FINAL ====================\n";
    cout << "Mejor asignación de drones-bases:\n";
    for (int i = 0; i < k; ++i)
        cout << "  Dron " << i << " -> Base " << mejorAsignacion[i] << "\n";
    cout << "Costo total acumulado: " << mejorValorGlobal << "\n";
    cout << "=========================================================\n";

    // 6. Mostrar trayectorias y movimientos
    static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};
    static const string direcciones[9] = {
        "↑", "↗", "→", "↘", "↓", "↙", "←", "↖", "•"
    };

    cout << "\n==================== TRAYECTORIAS ====================\n";
    for (int d = 0; d < k; ++d) {
        auto [r, c] = g.getBasePorID(mejorAsignacion[d]);
        cout << "Dron " << d << " (Base " << mejorAsignacion[d]
             << ") - posición inicial (" << r << "," << c << ")\n";

        cout << "Movimientos: ";
        for (int t = 0; t < T; ++t) {
            int dir = mejorSolucionGlobal[d][t];
            cout << direcciones[dir] << " ";
        }
        cout << "\nTrayectoria (fila,columna): ";

        for (int t = 0; t < T; ++t) {
            int dir = mejorSolucionGlobal[d][t];
            r += dr[dir];
            c += dc[dir];
            cout << "(" << r << "," << c << ")";
            if (t < T - 1) cout << " -> ";
        }
        cout << "\n-------------------------------------------------------\n";
    }
    cout << "=========================================================\n";

    // 7. Exportar escenario completo para Python
    exportarEscenario(g, mejorSolucionGlobal, mejorAsignacion, k, T, "resultado_final");
    cout << "=========================================================\n";

    return 0;
}