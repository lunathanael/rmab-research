#include "utils.h"

#include <bits/stdc++.h>

using namespace std;

class transition_matrix_t : public vector<vector<double>> {
public:
    friend istream& operator>>(istream& is, transition_matrix_t& matrix) {
        int n_states = matrix.size();
        for(int s_before = 0; s_before < n_states; ++s_before) {
            for(int s_after = 0; s_after < n_states; ++s_after) {
                is >> matrix[s_before][s_after];
            }
        }
        return is;
    }
    friend ostream& operator<<(ostream& os, const transition_matrix_t& matrix) {
        int n_states = matrix.size();
        os << fixed << setprecision(4);
        for(int s_before = 0; s_before < n_states; ++s_before) {
            for(int s_after = 0; s_after < n_states; ++s_after) {
                os << setw(8) << matrix[s_before][s_after];
            }
            os << '\n';
        }
        return os;
    }
};

class RMAB {
    vector<double> initial_state;
    vector<vector<pair<double, double>>> rewards;
    vector<pair<transition_matrix_t, transition_matrix_t>> transition_probabilities;
    double alpha;
    int n_arms, n_steps, n_states;
public:
    void parse() {
        cin >> n_arms >> n_steps >> n_states >> alpha;
        initial_state.resize(n_states);
        for(int i = 0; i < n_states; ++i)
            cin >> initial_state[i];
        rewards.resize(n_steps, vector<pair<double, double>>(n_states));
        for(int t = 0; t < n_steps; ++t) {
            for(int s = 0; s < n_states; ++s)
                cin >> rewards[t][s].first;
            for(int s = 0; s < n_states; ++s)
                cin >> rewards[t][s].second;
        }
        transition_probabilities.resize(n_steps);
        for(int t = 0; t < n_steps; ++t) {
            transition_probabilities[t].first.resize(n_states, vector<double>(n_states));
            transition_probabilities[t].second.resize(n_states, vector<double>(n_states));
            cin >> transition_probabilities[t].first >> transition_probabilities[t].second;
        }
    }
    void print_args() {
        cout << format("Number of arms: {}, Number of time steps: {}, Number of states: {}, Alpha: \n", n_arms, n_steps, n_states, alpha);
        cout << "Initial state:\n";
        for(int i = 0; i < n_arms; ++i)
            cout << setw(4) << initial_state[i] << ' ';
        cout << '\n';

        cout << "Rewards:\n";
        for(int t = 0; t < n_steps; ++t) {
            cout << format("r({},0)=\n", t);
            for(int s = 0; s < n_states; ++s) {
                cout << setw(5) << rewards[t][s].first;
            }
            cout << '\n';
            cout << format("r({},1)=\n", t);
            for(int s = 0; s < n_states; ++s) {
                cout << setw(5) << rewards[t][s].second;
            }
            cout << '\n';
        }

        cout << "Transition Probabilities:\n";
        for(int t = 0; t < n_steps; ++t) {
            cout << format("P{}(· | ·, 0)=\n", t);
            cout << transition_probabilities[t].first;
            cout << format("P{}(· | ·, 1)=\n", t);
            cout << transition_probabilities[t].second;
        }
        cout << '\n';
    }
};


int main() {
    RMAB rmab;
    rmab.parse();
    rmab.print_args();
    return 0;
}