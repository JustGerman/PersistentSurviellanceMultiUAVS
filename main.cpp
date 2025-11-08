#include <bits/stdc++.h>
using namespace std;

struct Celda {
    char tipo;
    int urgencia;
    vector<int> movimientos;

    Celda() : tipo('.'), urgencia(0), movimientos(8, 0) {}
};


class Grid {
private:
    int filas, cols;
    int n_obstaculos, n_urgencias, n_bases;
    vector<pair<int,int>> obstaculos;
    vector<tuple<int,int,int>> urgencias;
    vector<tuple<int,int,int>> bases;
    vector<vector<Celda>> grid;

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

    // Getters
    int getFilas() const { return filas; }
    int getCols() const { return cols; }
    int getNBases() const { return n_bases; }

    // Accesos
    char getTipo(int i, int j) const { return grid[i][j].tipo; }
    int getUrgencia(int i, int j) const { return grid[i][j].urgencia; }


};


int main() {
    Grid g("instancias_pspuav/PSP-UAV_01_a.txt");
    g.print();

    cout << "Cantidad de bases: " << g.getNBases() << endl;

    

    return 0;
}