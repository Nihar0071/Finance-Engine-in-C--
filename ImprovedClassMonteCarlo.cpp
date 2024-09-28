class MonteCarloAgent {
private:
    std::map<State, std::vector<std::pair<std::vector<double>, double>>> Q;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    double epsilon_start;
    double epsilon_end;
    double epsilon_decay;
    double gamma;
    double learning_rate;
    int num_stocks;
    int total_episodes;
    int current_episode;

public:
    MonteCarloAgent(int num_stocks, int total_episodes, double epsilon_start = 0.5, double epsilon_end = 0.01, 
                    double epsilon_decay = 0.995, double gamma = 0.99, double learning_rate = 0.1)
        : num_stocks(num_stocks), total_episodes(total_episodes), epsilon_start(epsilon_start), epsilon_end(epsilon_end),
          epsilon_decay(epsilon_decay), gamma(gamma), learning_rate(learning_rate), current_episode(0),
          gen(std::random_device{}()), dis(0.0, 1.0) {}

    std::vector<double> getAction(const State& state) {
        double epsilon = std::max(epsilon_end, epsilon_start * std::pow(epsilon_decay, current_episode));
        
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

    std::vector<double> getRandomAction() {
        std::vector<double> actions(num_stocks);
        for (int i = 0; i < num_stocks; i++) {
            actions[i] = dis(gen);  // Continuous action space [0, 1]
        }
        return actions;
    }

    void update(const std::vector<State>& states, const std::vector<std::vector<double>>& actions, const std::vector<double>& rewards) {
        double G = 0.0;
        for (int t = static_cast<int>(states.size()) - 1; t >= 0; t--) {
            G = gamma * G + rewards[t];
            
            auto it = std::find_if(Q[states[t]].begin(), Q[states[t]].end(),
                [&actions, t](const auto& pair) { return pair.first == actions[t]; });
            
            if (it == Q[states[t]].end()) {
                Q[states[t]].push_back({actions[t], G});
            } else {
                it->second += learning_rate * (G - it->second);
            }
        }
        current_episode++;
    }
};

// In the main function, update the agent initialization:
MonteCarloAgent agent(num_stocks, num_episodes);

// In the TradingEnvironment::step function, update the action processing:
std::pair<State, double> step(const std::vector<double>& actions) {
    double prev_portfolio_value = calculatePortfolioValue();
    current_step++;

    if (current_step >= historical_data.size()) {
        return {current_state, 0.0};
    }

    current_state.prices = historical_data[current_step];

    for (int i = 0; i < num_stocks; i++) {
        double action = actions[i];
        if (action > 0.5) {  // Buy
            int shares_to_buy = static_cast<int>((action - 0.5) * 2 * current_state.cash / current_state.prices[i]);
            if (shares_to_buy > 0) {
                current_state.holdings[i] += shares_to_buy;
                current_state.cash -= shares_to_buy * current_state.prices[i];
            }
        } else {  // Sell
            int shares_to_sell = static_cast<int>(action * 2 * current_state.holdings[i]);
            if (shares_to_sell > 0) {
                current_state.holdings[i] -= shares_to_sell;
                current_state.cash += shares_to_sell * current_state.prices[i];
            }
        }
    }

    double new_portfolio_value = calculatePortfolioValue();
    double reward = (new_portfolio_value - prev_portfolio_value) / prev_portfolio_value;

    // Incorporate risk into the reward
    double var = calculateVaR(0.05, {reward});
    double cvar = calculateCVaR(0.05, {reward});
    double risk_adjusted_reward = reward - 0.5 * (std::abs(var) + std::abs(cvar));

    return {current_state, risk_adjusted_reward};
}