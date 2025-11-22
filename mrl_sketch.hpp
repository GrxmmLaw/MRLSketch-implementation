#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;

class MRLSketch {
private:
    int n;          
    double epsilon; 
    int k;         
    int L;         
    vector<vector<int>> compactors; 
public:

    MRLSketch(int n_val, const string& flow, double eps_val) : epsilon(eps_val), n(n_val) {
        
        // Cálculo del valor de k
        double raw_k = (1.0 / epsilon) * log2(epsilon * n);
        k = (int)ceil(raw_k);
        if (k % 2 != 0) k++; 
        k = max(k, 2); 

        // Cálculo del valor de L y del número de compactadores:
        L = (int)ceil(log2((double)n / k));
        compactors.resize(L+1); 
    
        // Parsear texto e insertar a los compactadores:
        ifstream file(flow);
        if (!file.is_open()) {
            cerr << "No se pudo abrir el archivo " << flow << endl;
            return;
        }
    
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                insert(stoi(line));
            }
        }
        file.close();

        // Impresion de información
        cout << "------------------------" << endl;
        cout << "N: " << n << ", Epsilon: " << epsilon << endl;
        cout << "k: " << k << endl;
        cout << "L: " << L << endl;
        cout << "Cantidad de compactadores: " << compactors.size() << endl;
        cout << "------------------------" << endl;
}

    void insert(int val) {
        compactors[0].push_back(val);
        int j = 0;
        while (compactors[j].size() >= k) {
            sort(compactors[j].begin(), compactors[j].end());
            for (size_t i = 0; i < compactors[j].size(); i += 2) {
                if (j + 1 >= compactors.size()) {
                    compactors.resize(j + 2); 
                }
                compactors[j+1].push_back(compactors[j][i]);
            }
            compactors[j].clear();
            j++;
        }
        n++;
    }

    long long rank(int x) {
        int ans = 0;
        for (int j = 0; j < compactors.size(); j++) {
            for (int z : compactors[j]) {
                if (z < x) {
                    ans += (1LL << j); 
                }
            }
        }
        return ans;
    }

    int quantile(double phi) {
        int r = (int)floor(phi * n);
        return select_query(r);
    }

    int select_query(int r) {
        if (r <= 0) return -1;
        vector<pair<int, int>> B;
        for (size_t j = 0; j < compactors.size(); j++) {
            int weight = (1 << j);
            for (int z : compactors[j]) {
                B.push_back({z, weight});
            }
        }
        sort(B.begin(), B.end());
        long long current_weight_sum = 0;
        for (auto& p : B) {
            current_weight_sum += p.second; 
            if (current_weight_sum >= r) {
                return p.first; 
            }
        }
        
        if (!B.empty()) return B.back().first;
        return -1;
    }

    void print_state() {
        cout << "Estado del Sketch:" << endl;
        for (int j = 0; j < compactors.size(); j++) {
            cout << "  Nivel A_" << j << " (Peso " << (1 << j) << "): [ ";
            for (int val : compactors[j]) cout << val << " ";
            cout << "]" << endl;
        }
    }
};
