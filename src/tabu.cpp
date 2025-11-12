#include "tabu.hpp"

//     Constructor principal
TabuSearch::TabuSearch(const Grid& g, int iterMax_, int tabuTenencia_, int k_, int T_, const vector<int>& bases_)
    : grid(g), iterMax(iterMax_), tabuTenencia(tabuTenencia_), k(k_), T(T_), basesIDs(bases_) {}

//     Hash de solución completa
size_t TabuSearch::hashSolucion(const vector<vector<int>>& matriz) const {
    std::hash<int> h;
    size_t total = 0;
    for (const auto& fila : matriz)
        for (int x : fila)
            total ^= h(x) + 0x9e3779b9 + (total << 6) + (total >> 2);
    return total;
}


//     Control de lista tabú
bool TabuSearch::esTabu(const vector<vector<int>>& matriz) const {
    size_t h = hashSolucion(matriz);
    return std::find(listaTabu.begin(), listaTabu.end(), h) != listaTabu.end();
}

void TabuSearch::agregarTabu(const vector<vector<int>>& matriz) {
    size_t h = hashSolucion(matriz);
    listaTabu.push_back(h);
    if ((int)listaTabu.size() > tabuTenencia)
        listaTabu.pop_front();
}

//     Generación de vecinos
vector<vector<vector<int>>> TabuSearch::generarVecinos(const vector<vector<int>>& sol) {
    vector<vector<vector<int>>> vecinos;
    vecinos.reserve(k * T * 8);

    for (int i = 0; i < k; ++i) {
        for (int t = 0; t < T; ++t) {
            int movActual = sol[i][t];
            for (int d = 0; d < 9; ++d) {
                if (d == movActual) continue;

                vector<vector<int>> nuevo = sol;
                nuevo[i][t] = d;

                // Si la ruta no es factible, descartar
                if (!grid.esRutaFactible(basesIDs, nuevo, T))
                    continue;

                // Si ya está en lista tabú, descartar
                if (esTabu(nuevo))
                    continue;

                vecinos.push_back(move(nuevo));
            }
        }
    }
    return vecinos;
}

//     Función de evaluación
ll TabuSearch::funcionEvaluacion(const vector<vector<int>>& sol) const {
    const int F = grid.getFilas();
    const int C = grid.getCols();
    static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};

    const auto& hist = grid.getHistUrgencias();
    unordered_map<long long, int> ultimaVisita;
    ultimaVisita.reserve(grid.getNUrgencias() * 2);

    for (int d = 0; d < k; ++d) {
        auto [r, c] = grid.getBasePorID(basesIDs[d]);
        for (int t = 0; t < T; ++t) {
            int dir = sol[d][t];
            r += dr[dir];
            c += dc[dir];
            if (r < 0 || c < 0 || r >= F || c >= C) break;

            int u = grid.getUrgencia(r, c);
            if (u > 0) {
                long long key = (1LL * r << 32) | c;
                ultimaVisita[key] = t + 1;
            }
        }
    }

    unordered_map<int, vector<int>> visitasPorPeso;
    visitasPorPeso.reserve(hist.size() * 2);
    for (auto& [key, t_ult] : ultimaVisita) {
        int r = key >> 32;
        int c = key & 0xFFFFFFFF;
        int u = grid.getUrgencia(r, c);
        visitasPorPeso[u].push_back(t_ult);
    }

    ll total = 0;
    const long long coef_T = 1LL * T * (T + 1) / 2;

    for (auto& [w, N_w] : hist) {
        const auto& visitas = visitasPorPeso[w];
        int N_v = (int)visitas.size();
        total += 1LL * (N_w - N_v) * w * coef_T;
        for (int t_v : visitas)
            total += 1LL * w * (1LL * t_v * (t_v + 1) / 2);
    }

    return total;
}

//     Ejecución principal=
ResultadoTabu TabuSearch::ejecutar(const vector<vector<int>>& solInicial) { 
    vector<vector<int>> solActual = solInicial;
    vector<vector<int>> mejorSol  = solInicial;

    ll valorActual = funcionEvaluacion(solActual);
    ll mejorValor  = valorActual;

    agregarTabu(solActual); // guardar la inicial

    for (int iter = 0; iter < iterMax; ++iter) {
        auto vecinos = generarVecinos(solActual);
        if (vecinos.empty()) {
            cout << "[Iter " << iter << "] Sin vecinos válidos, fin de búsqueda.\n";
            break;
        }

        vector<vector<int>> mejorVecino = vecinos.front();
        ll mejorValorVecino = funcionEvaluacion(mejorVecino);

        for (auto& vecino : vecinos) {
            ll val = funcionEvaluacion(vecino);
            if (val < mejorValorVecino) {
                mejorValorVecino = val;
                mejorVecino = vecino;
            }
        }

        // Actualizar estado actual y lista tabú
        solActual = mejorVecino;
        valorActual = mejorValorVecino;
        agregarTabu(mejorVecino);

        // Actualizar mejor global
        if (valorActual < mejorValor) {
            mejorValor = valorActual;
            mejorSol = mejorVecino;
        }

        cout << "[Iter " << iter << "] Actual: " << valorActual
             << " | Mejor: " << mejorValor
             << " | TabuSize: " << listaTabu.size() << endl;
    }

    return {mejorSol, mejorValor};
}
