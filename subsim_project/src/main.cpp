#include "subsim.hpp"
#include <iostream>
#include <iomanip>

int main() {
    try {
        // Define variables
        std::vector<Variable> vars = {
            Variable("position", 0.0),
            Variable("velocity", 0.0),
            Variable("time", 0.0)
        };

        // Define begin function
        auto begin_fn = [](Context& ctx) {
            ctx.setState<double>("position", 0.0);
            ctx.setState<double>("velocity", 1.0);
            ctx.setState<double>("time", 0.0);
        };

        // Define step function
        auto step_fn = [](Context& ctx, int step) {
            double dt = 0.1;
            double pos = ctx.getState<double>("position");
            double vel = ctx.getState<double>("velocity");
            double time = ctx.getState<double>("time");

            ctx.setState<double>("position", pos + vel * dt);
            ctx.setState<double>("time", time + dt);
        };

        // Create simulation environment
        SubSimulationEnv env(vars, begin_fn, step_fn);

        std::cout << "Running simulation..." << std::endl;
        
        // Run simulation
        env.runSteps(10);

        // Get history
        auto position_history = env.getVariableHistory<double>("position");
        auto time_history = env.getVariableHistory<double>("time");
        
        // Print results
        std::cout << "\nSimulation Results:" << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\nTime\tPosition" << std::endl;
        std::cout << "--------------------" << std::endl;
        
        for (size_t i = 0; i < position_history.size(); ++i) {
            std::cout << time_history[i] << "\t" << position_history[i] << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}