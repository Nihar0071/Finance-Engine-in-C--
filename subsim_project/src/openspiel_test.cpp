// src/openspiel_test.cpp
#include "openspiel_adapter.hpp"
#include <iostream>
#include <iomanip>

void printGameStats(const std::string& label, const OpenSpielAdapter::GameStats& stats) {
    std::cout << "\n" << label << " Statistics:\n"
              << "  Win Rate: " << (stats.win_rate * 100) << "%\n"
              << "  Average Reward: " << stats.avg_reward << "\n"
              << "  Reward Std Dev: " << stats.std_reward << "\n"
              << "  Execution Time: " << stats.execution_time.count() << "ms\n"
              << "  Step Rewards: ";
              
    for (size_t i = 0; i < std::min(size_t(5), stats.step_rewards.size()); ++i) {
        std::cout << stats.step_rewards[i] << " ";
    }
    if (stats.step_rewards.size() > 5) std::cout << "...";
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    try {
        std::string game_name = "tic_tac_toe";  // Default game
        if (argc > 1) game_name = argv[1];
        
        const int num_simulations = 1000;
        const int num_steps = 50;
        
        std::cout << "Testing Monte Carlo simulation with OpenSpiel\n"
                  << "Game: " << game_name << "\n"
                  << "Simulations: " << num_simulations << "\n"
                  << "Steps per simulation: " << num_steps << "\n";

        OpenSpielAdapter adapter(game_name, num_simulations, num_steps);

        // Run your Monte Carlo implementation
        auto your_stats = adapter.runSimulations(true);
        printGameStats("Your Implementation", your_stats);

        // Compare with OpenSpiel's MCTS
        std::cout << "\nComparing with OpenSpiel's MCTS implementation...\n";
        auto openspiel_stats = adapter.compareWithOpenSpiel(100);
        printGameStats("OpenSpiel MCTS", openspiel_stats);

        // Print comparison summary
        double speedup = static_cast<double>(openspiel_stats.execution_time.count()) /
                        your_stats.execution_time.count();
                        
        std::cout << "\nComparison Summary:\n"
                  << "  Your implementation is " << std::fixed << std::setprecision(2)
                  << speedup << "x " << (speedup > 1 ? "faster" : "slower")
                  << " than OpenSpiel's MCTS\n"
                  << "  Win rate difference: "
                  << ((your_stats.win_rate - openspiel_stats.win_rate) * 100)
                  << " percentage points\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}