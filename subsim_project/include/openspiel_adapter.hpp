// include/openspiel_adapter.hpp
#pragma once
#include "montecarlo.hpp"
#include "subsim.hpp"
#include "open_spiel/spiel.h"
#include "open_spiel/algorithms/mcts.h"
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>

class OpenSpielAdapter {
public:
    struct GameStats {
        double win_rate;
        double avg_reward;
        double std_reward;
        std::vector<double> step_rewards;
        std::chrono::milliseconds execution_time;
    };

    OpenSpielAdapter(const std::string& game_name, int num_simulations, int num_steps)
        : game_(open_spiel::LoadGame(game_name)),
          num_simulations_(num_simulations),
          num_steps_(num_steps) {
        setupVariables();
        setupMonteCarlo();
    }

    GameStats runSimulations(bool show_progress = true) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        mc_env_->run(show_progress);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time
        );

        return collectStats(duration);
    }

    // Compare with OpenSpiel's MCTS
    GameStats compareWithOpenSpiel(int num_comparison_games = 100) {
    // Create OpenSpiel's MCTS bot
    auto evaluator = std::make_shared<open_spiel::algorithms::RandomRolloutEvaluator>(num_simulations_, 42); // Using random rollout evaluator
    open_spiel::algorithms::MCTSBot openspiel_bot(
        *game_,
        evaluator,              // Pass the evaluator
        /*uct_c=*/1.0,          // Exploration parameter (default)
        num_simulations_,       // Use same number of simulations
        /*max_memory_mb=*/1000, // Max memory
        /*solve=*/true,         // Whether to solve states
        /*seed=*/42,            // Random seed
        /*verbose=*/false,      // Verbose output
        open_spiel::algorithms::ChildSelectionPolicy::UCT // Child selection policy
    );

    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::vector<double> rewards;
    int wins = 0;
    
    for (int i = 0; i < num_comparison_games; ++i) {
        auto state = game_->NewInitialState();
        while (!state->IsTerminal()) {
            auto legal_actions = state->LegalActions();
            if (legal_actions.empty()) break;
            
            auto action = openspiel_bot.Step(*state);
            state->ApplyAction(action);
        }
        
        double reward = state->Returns()[0];
        rewards.push_back(reward);
        if (reward > 0) wins++;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time
    );

    GameStats stats;
    stats.win_rate = static_cast<double>(wins) / num_comparison_games;
    stats.avg_reward = std::accumulate(rewards.begin(), rewards.end(), 0.0) / rewards.size();
    stats.execution_time = duration;

    return stats;
}

private:
    std::shared_ptr<const open_spiel::Game> game_;
    std::unique_ptr<MonteCarloSimulationEnv> mc_env_;
    std::vector<Variable> variables_;
    int num_simulations_;
    int num_steps_;

    void setupVariables() {
        variables_ = {
            Variable("current_player", 0),
            Variable("legal_actions", std::vector<int>{}),
            Variable("reward", 0.0),
            Variable("terminal", false)
        };

        // Add game-specific variables
        if (game_->GetType().utility == open_spiel::GameType::Utility::kZeroSum) {
            variables_.emplace_back("is_zero_sum", true);
        }
    }

    void setupMonteCarlo() {
        mc_env_ = std::make_unique<MonteCarloSimulationEnv>(
            variables_,
            num_simulations_,
            num_steps_
        );

        // Set callbacks
        mc_env_->set_subsim_begin_callback(
            [this](Context& ctx) {
                auto state = game_->NewInitialState();
                updateContext(ctx, *state);
            }
        );

        mc_env_->set_subsim_step_callback(
            [this](Context& ctx, int step) {
                // Reconstruct game state from context
                auto state = reconstructState(ctx);
                if (!state->IsTerminal()) {
                    auto legal_actions = state->LegalActions();
                    if (!legal_actions.empty()) {
                        // Use Monte Carlo selection logic here
                        size_t action_idx = selectAction(ctx, *state);
                        if (action_idx < legal_actions.size()) {
                            state->ApplyAction(legal_actions[action_idx]);
                        }
                    }
                }
                updateContext(ctx, *state);
            }
        );
    }

    void updateContext(Context& ctx, const open_spiel::State& state) {
        ctx.setState("current_player", state.CurrentPlayer());
        ctx.setState("reward", state.Returns()[state.CurrentPlayer()]);
        ctx.setState("terminal", state.IsTerminal());
        
        auto legal_actions = state.LegalActions();
        ctx.setState("legal_actions", legal_actions);
    }

    std::unique_ptr<open_spiel::State> reconstructState(const Context& ctx) {
        auto state = game_->NewInitialState();
        // Reconstruct state based on context history
        // This will vary based on game type
        return state;
    }

    size_t selectAction(const Context& ctx, const open_spiel::State& state) {
        auto legal_actions = state.LegalActions();
        if (legal_actions.empty()) return 0;
        
        // Implement your action selection logic here
        // You can use ctx to access your Monte Carlo statistics
        return std::rand() % legal_actions.size();  // Placeholder
    }

    GameStats collectStats(std::chrono::milliseconds duration) {
        GameStats stats;
        
        // Collect rewards across all simulations
        auto reward_stats = mc_env_->get_variable_mean("reward");
        stats.avg_reward = reward_stats.overall_value;
        stats.step_rewards = reward_stats.values;
        
        // Calculate standard deviation
        auto stddev_stats = mc_env_->get_variable_stddev("reward");
        stats.std_reward = stddev_stats.overall_value;
        
        // Calculate win rate
        auto terminal_states = mc_env_->get_variable_histories("terminal");
        int wins = 0;
        int total_games = 0;
        for (const auto& history : terminal_states) {
            if (!history.empty() && history.back() > 0) {
                wins++;
            }
            total_games++;
        }
        stats.win_rate = static_cast<double>(wins) / total_games;
        
        stats.execution_time = duration;
        return stats;
    }
};