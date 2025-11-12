#include "grid.hpp"


Celda::Celda() : tipo('.'), urgencia(0), movimientos(9, 0) {
    movimientos[8] = 1;
}

Grid::Grid(string instancia) {
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
        grid[r][c].tipo = 'O';
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

    // Movimientos válidos
    int dr[9] = {-1, -1,  0,  1,  1,  1,  0, -1, 0};
    int dc[9] = { 0,  1,  1,  1,  0, -1, -1, -1, 0};

    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < cols; ++j) {
            for (int d = 0; d < 9; ++d) {
                int ni = i + dr[d];
                int nj = j + dc[d];
                if (ni < 0 || nj < 0 || ni >= filas || nj >= cols)
                    continue;
                if (grid[ni][nj].tipo != 'O')
                    grid[i][j].movimientos[d] = 1;
            }
            if (grid[i][j].tipo == 'B')
                grid[i][j].urgencia = 0;
        }
    }
}

void Grid::print() const {
    cout << "Grid (" << filas << "x" << cols << ")\n";
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < cols; ++j) {
            const Celda& c = grid[i][j];
            if (c.tipo == 'O') cout << " O ";
            else if (c.tipo == 'B') cout << " B ";
            else cout << setw(2) << c.urgencia << " ";
        }
        cout << endl;
    }
}

bool Grid::esRutaFactible(const vector<int>& basesIDs, const vector<vector<int>>& movimientos, int T) const {
    static const int dr[9] = {-1,-1,0,1,1,1,0,-1,0};
    static const int dc[9] = { 0, 1,1,1,0,-1,-1,-1,0};

    int k = (int)basesIDs.size();
    vector<pair<int,int>> posiciones(k);

    for (int i = 0; i < k; ++i)
        posiciones[i] = getBasePorID(basesIDs[i]);

    for (int t = 0; t < T; ++t) {
        for (int i = 0; i < k; ++i) {
            int dir = movimientos[i][t];
            auto [r, c] = posiciones[i];
            if (dir < 0 || dir > 8 || grid[r][c].movimientos[dir] == 0)
                return false;
            int nr = r + dr[dir];
            int nc = c + dc[dir];
            posiciones[i] = {nr, nc};
        }
        vector<pair<int,int>> snapshot = posiciones;
        sort(snapshot.begin(), snapshot.end());
        for (int i = 1; i < k; ++i)
            if (snapshot[i] == snapshot[i - 1])
                return false;
    }
    return true;
}

// Getters

int Grid::getFilas() const { return filas; }
int Grid::getCols() const { return cols; }
int Grid::getNBases() const { return n_bases; }
int Grid::getUrgenciaTotalInicial() const { return urgenciaTotalInicial; }
int Grid::getNUrgencias() const { return n_urgencias; }
const unordered_map<int, int>& Grid::getHistUrgencias() const { return histUrgencias; }

vector<pair<int,int>> Grid::getBases() const {
    vector<pair<int,int>> res;
    for (auto& b : bases) {
        int id, r, c;
        tie(id, r, c) = b;
        res.push_back({r, c});
    }
    return res;
}

pair<int,int> Grid::getBasePorID(int id) const {
    for (auto& b : bases) {
        int bid, r, c;
        tie(bid, r, c) = b;
        if (bid == id) return {r, c};
    }
    cerr << "Error: no se encontró la base con ID " << id << endl;
    exit(1);
}

char Grid::getTipo(int i, int j) const { return grid[i][j].tipo; }
int Grid::getUrgencia(int i, int j) const { return grid[i][j].urgencia; }
