#include "montecarlo.hpp"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <stdexcept>

MonteCarloSimulationEnv::MonteCarloSimulationEnv(
    const std::vector<Variable>& variables,
    int n_subsimulations,
    int n_steps
) : variables_(variables),
    n_subsims_(n_subsimulations),
    n_steps_(n_steps) {
    
    if (n_subsimulations <= 0) 
        throw std::invalid_argument("n_subsimulations must be positive");
    if (n_steps <= 0) 
        throw std::invalid_argument("n_steps must be positive");
    
    // Initialize subsim_begin and subsim_step as lambda functions
    subsim_begin = [this](std::function<void(Context&)> f) {
        begin_function_ = f;
    };
    
    subsim_step = [this](std::function<void(Context&, int)> f) {
        step_function_ = f;
    };
}

void MonteCarloSimulationEnv::set_subsim_begin_callback(std::function<void(Context&)> f) {
    begin_function_ = f;
}

void MonteCarloSimulationEnv::set_subsim_step_callback(std::function<void(Context&, int)> f) {
    step_function_ = f;
}

void MonteCarloSimulationEnv::run(bool show_progress) {
    if (!begin_function_ || !step_function_)
        throw std::runtime_error("Begin and step functions must be set before running");

    subsim_envs_.clear();
    subsim_envs_.reserve(n_subsims_);

    for (int i = 0; i < n_subsims_; ++i) {
        if (show_progress) {
            std::cout << "\rRunning simulation " << (i + 1) << "/" << n_subsims_ << std::flush;
        }

        auto subsim = std::make_unique<SubSimulationEnv>(
            variables_,
            begin_function_,
            step_function_
        );
        subsim->runSteps(n_steps_);
        subsim_envs_.push_back(std::move(subsim));
    }
    
    if (show_progress) std::cout << std::endl;
}

SubSimulationEnv& MonteCarloSimulationEnv::get_subsim_env(int subsim_index) {
    if (subsim_index >= n_subsims_)
        throw std::out_of_range("subsim_index out of range");
    return *subsim_envs_[subsim_index];
}

void MonteCarloSimulationEnv::validate_variable(const std::string& var_name) const {
    auto it = std::find_if(variables_.begin(), variables_.end(),
        [&var_name](const Variable& v) { return v.name == var_name; });
    
    if (it == variables_.end())
        throw std::invalid_argument("Variable " + var_name + " does not exist");
}

std::vector<std::vector<double>> MonteCarloSimulationEnv::collect_histories(
    const std::string& var_name) const {
    std::vector<std::vector<double>> histories;
    histories.reserve(n_subsims_);
    
    for (const auto& subsim : subsim_envs_) {
        histories.push_back(subsim->getVariableHistory<double>(var_name));
    }
    
    return histories;
}

StatisticalResult MonteCarloSimulationEnv::get_variable_mean(
    const std::string& var_name, 
    const std::string& domain) {
    
    validate_variable(var_name);
    auto histories = collect_histories(var_name);
    
    if (domain == "step") {
        std::vector<double> means(n_steps_);
        for (int step = 0; step < n_steps_; ++step) {
            double sum = 0.0;
            for (int sim = 0; sim < n_subsims_; ++sim) {
                sum += histories[sim][step];
            }
            means[step] = sum / n_subsims_;
        }
        
        double overall = std::accumulate(means.begin(), means.end(), 0.0) / means.size();
        return StatisticalResult(means, overall);
    }
    // Implementation for other domains...
    
    return StatisticalResult();
}

// Similar implementations for median, variance, stddev, min, max, sum...

MonteCarloSimulationEnv::HistogramResult MonteCarloSimulationEnv::get_variable_histogram(
    const std::string& var_name,
    int n_bins,
    bool density,
    std::optional<std::pair<double, double>> range) {
    
    validate_variable(var_name);
    auto histories = collect_histories(var_name);
    
    // Find range if not provided
    double min_val = range ? range->first : std::numeric_limits<double>::max();
    double max_val = range ? range->second : std::numeric_limits<double>::lowest();
    
    if (!range) {
        for (const auto& history : histories) {
            for (double value : history) {
                min_val = std::min(min_val, value);
                max_val = std::max(max_val, value);
            }
        }
    }
    
    // Calculate bin edges
    std::vector<double> bin_edges(n_bins + 1);
    double bin_width = (max_val - min_val) / n_bins;
    for (int i = 0; i <= n_bins; ++i) {
        bin_edges[i] = min_val + i * bin_width;
    }
    
    // Calculate histograms for each step
    std::vector<std::vector<double>> counts(n_steps_, std::vector<double>(n_bins));
    
    for (int step = 0; step < n_steps_; ++step) {
        std::vector<double> step_values;
        for (const auto& history : histories) {
            step_values.push_back(history[step]);
        }
        
        // Count values in bins
        for (double value : step_values) {
            int bin = static_cast<int>((value - min_val) / bin_width);
            if (bin >= 0 && bin < n_bins) {
                counts[step][bin]++;
            }
        }
        
        // Normalize if density is requested
        if (density) {
            double total = std::accumulate(counts[step].begin(), counts[step].end(), 0.0);
            for (double& count : counts[step]) {
                count /= (total * bin_width);
            }
        }
    }
    
    return HistogramResult{counts, bin_edges};
}

std::vector<std::vector<double>> MonteCarloSimulationEnv::get_variable_histories(
    const std::string& var_name) {
    validate_variable(var_name);
    return collect_histories(var_name);
}