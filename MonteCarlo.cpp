#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <numeric>
#include <iomanip>

enum class Action { Buy, Sell, Hold };

struct State {
    std::vector<double> prices;
    std::vector<int> holdings;
    double cash;

    bool operator<(const State& other) const {
        if (cash != other.cash) return cash < other.cash;
        if (holdings != other.holdings) return holdings < other.holdings;
        return prices < other.prices;
    }
};

class TradingEnvironment {
private:
    std::vector<std::vector<double>> historical_data;
    int current_step;
    double initial_cash;
    State current_state;
    int num_stocks;

public:
    TradingEnvironment(const std::string& filename, double initial_cash, int num_stocks)
        : current_step(0), initial_cash(initial_cash), num_stocks(num_stocks) {
        loadData(filename);
        reset();
    }

    void loadData(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            std::vector<double> row;

            std::getline(iss, token, ','); // Skip date

            while (std::getline(iss, token, ',')) {
                row.push_back(std::stod(token));
            }
            historical_data.push_back(row);
        }
    }

    State reset() {
        current_step = 0;
        current_state.prices = historical_data[current_step];
        current_state.holdings = std::vector<int>(num_stocks, 0);
        current_state.cash = initial_cash;
        return current_state;
    }

    std::pair<State, double> step(const std::vector<Action>& actions) {
        double prev_portfolio_value = calculatePortfolioValue();
        current_step++;

        if (current_step >= historical_data.size()) {
            return {current_state, 0.0};
        }

        current_state.prices = historical_data[current_step];

        for (int i = 0; i < num_stocks; i++) {
            switch (actions[i]) {
                case Action::Buy:
                    if (current_state.cash >= current_state.prices[i]) {
                        current_state.holdings[i]++;
                        current_state.cash -= current_state.prices[i];
                    }
                    break;
                case Action::Sell:
                    if (current_state.holdings[i] > 0) {
                        current_state.holdings[i]--;
                        current_state.cash += current_state.prices[i];
                    }
                    break;
                case Action::Hold:
                    break;
            }
        }

        double new_portfolio_value = calculatePortfolioValue();
        double reward = (new_portfolio_value - prev_portfolio_value) / prev_portfolio_value;

        return {current_state, reward};
    }

    bool isTerminal() const {
        return current_step >= historical_data.size() - 1;
    }

    double calculatePortfolioValue() const {
        double value = current_state.cash;
        for (int i = 0; i < num_stocks; i++) {
            value += current_state.holdings[i] * current_state.prices[i];
        }
        return value;
    }

    double calculateVaR(double alpha, const std::vector<double>& returns) const {
        std::vector<double> sorted_returns = returns;
        std::sort(sorted_returns.begin(), sorted_returns.end());
        int index = static_cast<int>(alpha * sorted_returns.size());
        return sorted_returns[index];
    }

    double calculateCVaR(double alpha, const std::vector<double>& returns) const {
        double var = calculateVaR(alpha, returns);
        std::vector<double> tail_losses;
        std::copy_if(returns.begin(), returns.end(), std::back_inserter(tail_losses),
                     [var](double r) { return r <= var; });
        return std::accumulate(tail_losses.begin(), tail_losses.end(), 0.0) / tail_losses.size();
    }
};

class MonteCarloAgent {
private:
    std::map<State, std::vector<std::pair<std::vector<Action>, double>>> Q;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    double epsilon;
    double gamma;
    int num_stocks;

public:
    MonteCarloAgent(int num_stocks, double epsilon = 0.1, double gamma = 0.99)
        : num_stocks(num_stocks), epsilon(epsilon), gamma(gamma), gen(std::random_device{}()), dis(0.0, 1.0) {}

    std::vector<Action> getAction(const State& state) {
        if (dis(gen) < epsilon) {
            return getRandomAction();
        } else {
            if (Q.find(state) != Q.end() && !Q[state].empty()) {
                auto best_action = std::max_element(Q[state].begin(), Q[state].end(),
                    [](const auto& a, const auto& b) { return a.second < b.second; });
                return best_action->first;
            } else {
                return getRandomAction();
            }
        }
    }

    std::vector<Action> getRandomAction() {
        std::vector<Action> actions(num_stocks);
        for (int i = 0; i < num_stocks; i++) {
            actions[i] = static_cast<Action>(dis(gen) * 3);
        }
        return actions;
    }

    void update(const std::vector<State>& states, const std::vector<std::vector<Action>>& actions, const std::vector<double>& rewards) {
        double G = 0.0;
        for (int t = static_cast<int>(states.size()) - 1; t >= 0; t--) {
            G = gamma * G + rewards[t];
            
            auto it = std::find_if(Q[states[t]].begin(), Q[states[t]].end(),
                [&actions, t](const auto& pair) { return pair.first == actions[t]; });
            
            if (it == Q[states[t]].end()) {
                Q[states[t]].push_back({actions[t], G});
            } else {
                it->second += (G - it->second) / (std::count_if(Q[states[t]].begin(), Q[states[t]].end(),
                    [&actions, t](const auto& pair) { return pair.first == actions[t]; }) + 1);
            }
        }
    }
};

int main() {
    const std::string data_file = "AAPL_1y_1d.csv";
    const double initial_cash = 100000.0;
    const int num_episodes = 10000;
    const int num_stocks = 25;

    TradingEnvironment env(data_file, initial_cash, num_stocks);
    MonteCarloAgent agent(num_stocks);

    for (int episode = 0; episode < num_episodes; episode++) {
        State state = env.reset();
        std::vector<State> states;
        std::vector<std::vector<Action>> actions;
        std::vector<double> rewards;

        while (!env.isTerminal()) {
            std::vector<Action> action = agent.getAction(state);
            auto [next_state, reward] = env.step(action);

            states.push_back(state);
            actions.push_back(action);
            rewards.push_back(reward);

            state = next_state;
        }

        agent.update(states, actions, rewards);

        if (episode % 100 == 0) {
            double final_value = env.calculatePortfolioValue();
            double var = env.calculateVaR(0.05, rewards);
            double cvar = env.calculateCVaR(0.05, rewards);
            std::cout << "Episode " << episode << " completed. Final portfolio value: " << std::fixed << std::setprecision(2) << final_value
                      << ", VaR(5%): " << std::setprecision(4) << var << ", CVaR(5%): " << cvar << std::endl;
        }
    }

    std::cout << "Training completed." << std::endl;

    return 0;
}