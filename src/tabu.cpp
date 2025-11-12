#include "tabu.hpp"
#include <random>
#include <cmath>
#include <algorithm>
#include <iostream>
using namespace std;

// ===============================
//     Constructor principal
// ===============================
TabuSearch::TabuSearch(const Grid& g, int iterMax_, int tabuTenencia_, int k_, int T_, const vector<int>& bases_)
    : grid(g), iterMax(iterMax_), tabuTenencia(tabuTenencia_), k(k_), T(T_), basesIDs(bases_) {}


// ===============================
//     Hash de solución completa
// ===============================
size_t TabuSearch::hashSolucion(const vector<vector<int>>& matriz) const {
    std::hash<int> h;
    size_t total = 0;
    for (const auto& fila : matriz)
        for (int x : fila)
            total ^= h(x) + 0x9e3779b9 + (total << 6) + (total >> 2);
    return total;
}


// ===============================
//     Control de lista tabú
// ===============================
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


// ===============================
//     Generación de vecinos con diversificación integrada
// ===============================
vector<vector<vector<int>>> TabuSearch::generarVecinos(const vector<vector<int>>& sol) {
    vector<vector<vector<int>>> vecinos;

    // --- Parámetros ---
    const int vecinos_base = 800;   // vecinos con un único cambio
    const int vecinos_extra = 1000;   // vecinos agresivos
    const double p_penal = 0.5;      // probabilidad de usar posiciones penalizadas

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_int_distribution<int> movRand(0, 8);
    std::uniform_int_distribution<int> tickRand(0, T - 1);
    std::uniform_int_distribution<int> dronRand(0, k - 1);

    vecinos.reserve(vecinos_base + vecinos_extra);

    // === 1️⃣ Vecinos base: un único cambio aleatorio ===
    for (int i = 0; i < vecinos_base; ++i) {
        vector<vector<int>> nuevo = sol;

        // 50% chance de alterar una posición penalizada (si existen)
        if (!posicionesPenalizadas.empty() && prob(rng) < p_penal) {
            auto [d, t] = posicionesPenalizadas[rng() % posicionesPenalizadas.size()];
            nuevo[d][t] = movRand(rng);
        } else {
            int d = dronRand(rng);
            int t = tickRand(rng);
            nuevo[d][t] = movRand(rng);
        }

        if (!esTabu(nuevo))
            vecinos.push_back(move(nuevo));
    }

    // === 2️⃣ Vecinos agresivos y diversos ===
    for (int i = 0; i < vecinos_extra; ++i) {
        vector<vector<int>> nuevo = sol;
        double r = prob(rng);

        if (r < 0.33 && k > 1) {
            // swap entre drones
            int a = dronRand(rng);
            int b = dronRand(rng);
            if (a != b) {
                int t = tickRand(rng);
                swap(nuevo[a][t], nuevo[b][t]);
            }
        } 
        else if (r < 0.66) {
            // múltiples cambios
            int d = dronRand(rng);
            int cambios = 2 + rng() % 6;
            for (int c = 0; c < cambios; ++c) {
                int t = tickRand(rng);
                nuevo[d][t] = movRand(rng);
            }
        } 
        else {
            // perturbación global leve priorizando penalizaciones
            for (int d = 0; d < k; ++d) {
                if (prob(rng) < 0.25) {
                    int t;
                    if (!posicionesPenalizadas.empty() && prob(rng) < 0.6)
                        t = posicionesPenalizadas[rng() % posicionesPenalizadas.size()].second;
                    else
                        t = tickRand(rng);
                    nuevo[d][t] = movRand(rng);
                }
            }
        }

        if (!esTabu(nuevo))
            vecinos.push_back(move(nuevo));
    }

    // === 3️⃣ Mezclar todo el vecindario ===
    std::shuffle(vecinos.begin(), vecinos.end(), rng);

    return vecinos;
}


// ===============================
//     Función de evaluación (infactible con penalización)
// ===============================
ll TabuSearch::funcionEvaluacionInfactibles(const vector<vector<int>>& sol) const {
    const int F = grid.getFilas();
    const int C = grid.getCols();
    static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};

    posicionesPenalizadas.clear(); // limpiar cada evaluación

    const auto& hist = grid.getHistUrgencias();
    unordered_map<long long, int> ultimaVisita;
    ultimaVisita.reserve(grid.getNUrgencias() * 2);

    // === Urgencia acumulada ===
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

    ll urgenciaTotal = 0;
    const long long coef_T = 1LL * T * (T + 1) / 2;

    for (auto& [w, N_w] : hist) {
        const auto& visitas = visitasPorPeso[w];
        int N_v = (int)visitas.size();
        urgenciaTotal += 1LL * (N_w - N_v) * w * coef_T;
        for (int t_v : visitas)
            urgenciaTotal += 1LL * w * (1LL * t_v * (t_v + 1) / 2);
    }

    // === Penalizaciones ===
    ll penalizacion = 0;
    const ll PENAL_COLISION   = 5 * grid.getUrgenciaTotalInicial() * grid.getCols() * grid.getFilas();
    const ll PENAL_OBSTACULO  = 3 * grid.getUrgenciaTotalInicial() * grid.getCols() * grid.getFilas();
    const ll PENAL_FUERA_MAPA = 10 * grid.getUrgenciaTotalInicial() * grid.getCols() * grid.getFilas();

    vector<vector<vector<int>>> ocupadas(T, vector<vector<int>>(F, vector<int>(C, 0)));

    for (size_t i = 0; i < sol.size(); ++i) {
        auto [fila, col] = grid.getBasePorID(basesIDs[i]);
        for (int t = 0; t < T; ++t) {
            int mov = sol[i][t];
            fila += dr[mov];
            col  += dc[mov];

            if (fila < 0 || fila >= F || col < 0 || col >= C) {
                penalizacion += PENAL_FUERA_MAPA;
                posicionesPenalizadas.emplace_back(i, max(0, t-1));
                continue;
            }

            if (grid.getTipo(fila, col) == 'O') {
                penalizacion += PENAL_OBSTACULO;
                posicionesPenalizadas.emplace_back(i, max(0, t-1));
            }

            ocupadas[t][fila][col]++;
            if (ocupadas[t][fila][col] > 1) {
                penalizacion += PENAL_COLISION;
                posicionesPenalizadas.emplace_back(i, max(0, t-1));
            }
        }
    }

    return urgenciaTotal + penalizacion;
}


// ===============================
//     Ejecución principal (Tabu Search)
// ===============================
ResultadoTabu TabuSearch::ejecutar(const vector<vector<int>>& solInicial) {
    vector<vector<int>> solActual = solInicial;
    vector<vector<int>> mejorSol  = solInicial;

    ll valorActual = funcionEvaluacionInfactibles(solActual);
    ll mejorValor  = valorActual;

    agregarTabu(solActual);

    deque<ll> historial;
    const int ventanaECM = 4;

    for (int iter = 0; iter < iterMax; ++iter) {
        auto vecinos = generarVecinos(solActual);

        // Calcular ECM (variabilidad)
        historial.push_back(valorActual);
        if ((int)historial.size() > ventanaECM) historial.pop_front();
        double ecm = 0;
        if (historial.size() > 2) {
            double mean = 0;
            for (ll v : historial) mean += v;
            mean /= historial.size();
            for (ll v : historial) ecm += (v - mean) * (v - mean);
            ecm = sqrt(ecm / historial.size()) / (mean + 1e-9);
        }

        if (vecinos.empty()) {
            cout << "[Iter " << iter << "] Sin vecinos válidos.\n";
            break;
        }

        vector<vector<int>> mejorVecino = vecinos.front();
        ll mejorValorVecino = funcionEvaluacionInfactibles(mejorVecino);

        for (auto& vecino : vecinos) {
            ll val = funcionEvaluacionInfactibles(vecino);
            bool tabu = esTabu(vecino);
            if (tabu && val >= mejorValor) continue; // aspiración
            if (val < mejorValorVecino) {
                mejorValorVecino = val;
                mejorVecino = vecino;
            }
        }

        solActual = mejorVecino;
        valorActual = mejorValorVecino;
        agregarTabu(mejorVecino);

        if (valorActual < mejorValor) {
            mejorValor = valorActual;
            mejorSol = mejorVecino;
            cout << " → Aspiración activada en iter " << iter << "\n";
        }

        cout << "[Iter " << iter << "] Actual: " << valorActual
             << " | Mejor: " << mejorValor
             << " | ECM: " << ecm
             << " | Vecinos: " << vecinos.size()
             << " | PenalPos: " << posicionesPenalizadas.size()
             << " | TabuSize: " << listaTabu.size() << endl;
    }

    return {mejorSol, mejorValor};
}
