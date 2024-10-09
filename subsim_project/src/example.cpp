#include "../include/montecarlo.hpp"
#include <iostream>
#include <random>
#include <cmath>

int main() {
    try {
        // Define variables for simulation
        std::vector<Variable> variables = {
            Variable("position", 0.0),
            Variable("velocity", 0.0),
            Variable("time", 0.0)
        };

        // Create Monte Carlo simulation environment
        int n_subsimulations = 100; // 100 simulations
        int n_steps = 50;           // 50 steps each
        MonteCarloSimulationEnv mc_env(variables, n_subsimulations, n_steps);

        // Define begin function
        mc_env.set_subsim_begin_callback([](Context& ctx) {
            ctx.setState<double>("position", 0.0);
            ctx.setState<double>("velocity", 1.0);
            ctx.setState<double>("time", 0.0);
        });

        // Define step function with random noise
        mc_env.set_subsim_step_callback([](Context& ctx, int step) {
            static thread_local std::mt19937 gen(std::random_device{}());
            static thread_local std::normal_distribution<> noise(0.0, 0.1);

            double dt = 0.1;
            double pos = ctx.getState<double>("position");
            double vel = ctx.getState<double>("velocity");
            double time = ctx.getState<double>("time");

            // Add random noise to velocity
            double noisy_vel = vel + noise(gen);

            ctx.setState<double>("position", pos + noisy_vel * dt);
            ctx.setState<double>("time", time + dt);
        });

        // Run simulations
        std::cout << "Running Monte Carlo simulations..." << std::endl;
        mc_env.run();

        // Analyze results
        auto mean_pos = mc_env.get_variable_mean("position");
        auto std_pos = mc_env.get_variable_stddev("position");

        std::cout << "\nResults Analysis:" << std::endl;
        std::cout << "Final mean position: " << mean_pos.values.back() << std::endl;
        std::cout << "Final position std dev: " << std_pos.values.back() << std::endl;

        // Generate histogram for final positions
        auto hist = mc_env.get_variable_histogram("position", 20, true);

        std::cout << "\nHistogram of final positions:" << std::endl;
        for (size_t i = 0; i < hist.counts.back().size(); ++i) {
            std::cout << "[" << hist.bin_edges[i] << ", " << hist.bin_edges[i+1] << "): "
                      << std::string(static_cast<int>(hist.counts.back()[i] * 50), '*') << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
