#include "../include/subsim.hpp"

// SubSimulationEnv constructor
SubSimulationEnv::SubSimulationEnv(
    const std::vector<Variable>& vars,
    std::function<void(Context&)> begin_fn,
    std::function<void(Context&, int)> step_fn
) : variables(vars), 
    begin_function(begin_fn), 
    step_function(step_fn),
    steps_taken(0) {
    // Initialize states with default values
    for (const auto& var : variables) {
        current_states[var.name] = var.default_value;
        history[var.name] = std::vector<ValueType>();
    }
}

void SubSimulationEnv::runSteps(int n) {
    if (n <= 0) throw std::invalid_argument("Steps must be positive");
    
    Context context(this);
    begin_function(context);
    
    for (int step = 0; step < n; ++step) {
        step_function(context, step);
        logStates();
        steps_taken++;
    }
}

void SubSimulationEnv::logStates() {
    for (const auto& [name, value] : current_states) {
        history[name].push_back(value);
    }
}

// Note: Template methods are defined in the header file (subsim.hpp)
