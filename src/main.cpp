#include <bits/stdc++.h>
#include "grid.hpp"
#include "tabu.hpp"
#include "utils.hpp"

using namespace std;

// ===========================================================
//                 PROGRAMA PRINCIPAL PSP-UAV
// ===========================================================
int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // =======================================================
    //               Validación de parámetros
    // =======================================================
    if (argc != 6) {
        cerr << "Uso: " << argv[0]
             << " <archivo_instancia> <k_drones> <T_ticks> <iterMax> <tabuTenencia>\n";
        return 1;
    }

    string archivoInstancia = argv[1];
    int k = stoi(argv[2]);
    int T = stoi(argv[3]);
    int iterMax = stoi(argv[4]);
    int tabuTenencia = stoi(argv[5]);

    cout << "================= PARÁMETROS DE EJECUCIÓN =================\n";
    cout << "Archivo instancia      : " << archivoInstancia << "\n";
    cout << "Cantidad de drones (k) : " << k << "\n";
    cout << "Duración misión (T)    : " << T << "\n";
    cout << "Iteraciones TabuSearch : " << iterMax << "\n";
    cout << "Tamaño lista tabú      : " << tabuTenencia << "\n";
    cout << "============================================================\n\n";

    // =======================================================
    //                  Cargar instancia
    // =======================================================
    auto start_total = chrono::high_resolution_clock::now();

    Grid g(archivoInstancia);
    g.print();
    cout << "Cantidad de bases: " << g.getNBases() << endl;

    int b = g.getNBases();
    if (k > b)
        cerr << "Advertencia: hay más drones (" << k << ") que bases (" << b << ").\n";

    // =======================================================
    //       Generar combinaciones y seleccionar aleatorias
    // =======================================================
    int N_ASIGNACIONES = 30;  // número de combinaciones a evaluar
    auto todas = generarTodasAsignaciones(k, b);
    cout << "\nTotal combinaciones posibles: " << todas.size() << endl;

    auto subset = seleccionarAsignacionesAleatorias(todas, N_ASIGNACIONES);
    cout << "Probando " << subset.size()
         << " asignaciones aleatorias únicas (ejecución paralela)...\n";

    // =======================================================
    //                   Ejecutar en paralelo
    // =======================================================
    vector<future<pair<vector<int>, ResultadoTabu>>> tareas;

    for (auto& asignacion : subset) {
        tareas.push_back(async(launch::async, [&g, asignacion, k, T, iterMax, tabuTenencia]() {
            TabuSearch ts(g, iterMax, tabuTenencia, k, T, asignacion);
            auto solInicial = generarSolucionInicial(g, k, T, asignacion);
            auto res = ts.ejecutar(solInicial);
            return make_pair(asignacion, res);
        }));
    }

    // =======================================================
    //                   Recolectar resultados
    // =======================================================
    ll mejorValorGlobal = LLONG_MAX;
    vector<vector<int>> mejorSolucionGlobal;
    vector<int> mejorAsignacion;

    int idx = 1;
    for (auto& t : tareas) {
        auto [asign, res] = t.get();
        cout << "[Combinación " << idx++ << "] Valor: " << res.mejorValor << endl;
        if (res.mejorValor < mejorValorGlobal) {
            mejorValorGlobal = res.mejorValor;
            mejorSolucionGlobal = res.mejorSol;
            mejorAsignacion = asign;
        }
    }

    // =======================================================
    //                   Resultado final
    // =======================================================
    auto end_total = chrono::high_resolution_clock::now();
    double tiempo_total = chrono::duration<double>(end_total - start_total).count();

    cout << "\n==================== RESULTADO FINAL ====================\n";
    for (int i = 0; i < k; ++i)
        cout << "Dron " << i << " -> Base " << mejorAsignacion[i] << "\n";
    cout << "Costo total acumulado: " << mejorValorGlobal << "\n";
    cout << "Tiempo total ejecución: " << fixed << setprecision(2)
         << tiempo_total << " s\n";
    cout << "=========================================================\n";

    // =======================================================
    //                  Mostrar trayectorias
    // =======================================================
    static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};
    static const string direcciones[9] = {"↑","↗","→","↘","↓","↙","←","↖","•"};

    cout << "\n==================== TRAYECTORIAS ====================\n";
    for (int d = 0; d < k; ++d) {
        auto [r, c] = g.getBasePorID(mejorAsignacion[d]);
        cout << "Dron " << d << " (Base " << mejorAsignacion[d]
             << ") - posición inicial (" << r << "," << c << ")\n";

        cout << "Movimientos: ";
        for (int t = 0; t < T; ++t)
            cout << direcciones[mejorSolucionGlobal[d][t]] << " ";
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

    // =======================================================
    //              Exportar escenario fijo (para Python)
    // =======================================================
    exportarEscenario(g, mejorSolucionGlobal, mejorAsignacion, k, T, "resultado_final");

    cout << "Archivos exportados correctamente en carpeta 'output/'.\n";
    cout << "=========================================================\n";

    return 0;
}
