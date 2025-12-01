#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include "mrl_sketch.hpp" 

using namespace std;

vector<int> load_data(const string& filename) {
    vector<int> data;
    string path = "data/" + filename; 
    ifstream file(path);
    
    if (!file.is_open()) {
        cerr << "[ERROR] No se pudo abrir: " << path << endl;
        return data;
    }

    int val;
    while (file >> val) {
        data.push_back(val);
    }

    file.close();
    return data;
}

void run_detailed_experiment(const string& filename, double epsilon) {
    cout << "Generando graficos para " << filename << " (eps=" << epsilon << ")... " << flush;

    // Cargar y ordenar datos
    vector<int> data = load_data(filename);
    if (data.empty()) return;
    long long n = data.size();
    
    vector<int> sorted_data = data;
    sort(sorted_data.begin(), sorted_data.end());

    // Construir sketch
    MRLSketch sketch(n, "data/"+ filename, epsilon);
  
    // Archivo de salida
    string out_name = "log_" + filename + "_" + to_string(epsilon).substr(0,4) + ".csv";
    ofstream outfile(out_name);
    
    // Encabezado del CSV
    outfile << "n_value,rank_val,rank_est,quantil_val,quantil_est,err_rank,err_quantile\n";

    int points = 200;
    long long step = max(1LL, n / points);

    for (long long i = 0; i < n; i += step) {
        
        // Estimar rank y calcular error
        int query_val = sorted_data[i];
        long long est_rank = sketch.rank(query_val);
        
        auto it = lower_bound(sorted_data.begin(), sorted_data.end(), query_val);
        long long true_rank = distance(sorted_data.begin(), it);

        double err_rank = abs((double)est_rank - (double)true_rank) / n;

        // Estimar quantil y calcular error
        double phi = (double)i / n;
        if(phi <= 0) phi = 0.0001; 
        int est_val_q = sketch.quantile(phi);
        
        auto it_q = lower_bound(sorted_data.begin(), sorted_data.end(), est_val_q);
        long long true_rank_of_est = distance(sorted_data.begin(), it_q);
        
        double err_quantile = abs( ((double)true_rank_of_est / n) - phi );

        outfile << i << "," << true_rank << "," << est_rank << "," << true_rank_of_est << "," << est_val_q << "," << err_rank << "," << err_quantile << "\n";
    }

    outfile.close();
    cout << " Guardado en " << out_name << endl;
}

int main() {
    vector<string> files = {
        "Uniform.txt", 
        "Log-normal.txt", 
        "chicago2015.txt", 
        "chicago2016.txt"
    };

    vector<double> epsilon = {0.05, 0.1}; 

    for (double eps: epsilon)
    {
        for (const string& f : files) {
            run_detailed_experiment(f, eps);
        }
    }
    return 0;
}

