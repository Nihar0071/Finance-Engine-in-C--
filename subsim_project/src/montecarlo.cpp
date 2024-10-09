#include "../include/montecarlo.hpp"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <thread>
#include <mutex>

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
    subsim_envs_.resize(n_subsims_);

    // Mutex for thread-safe console output
    std::mutex cout_mutex;

    // Run simulations in parallel
    std::vector<std::thread> threads;
    for (int i = 0; i < n_subsims_; ++i) {
        threads.emplace_back([this, i, show_progress, &cout_mutex]() {
            // Create subsimulation environment
            subsim_envs_[i] = std::make_unique<SubSimulationEnv>(
                variables_, begin_function_, step_function_
            );

            // Run steps
            subsim_envs_[i]->runSteps(n_steps_);

            if (show_progress) {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "\rCompleted simulation " << i + 1 << " of " << n_subsims_;
                std::cout.flush();
            }
        });
    }

    // Wait for all simulations to finish
    for (auto& thread : threads) {
        thread.join();
    }

    if (show_progress) {
        std::cout << std::endl << "All simulations completed." << std::endl;
    }
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

    throw std::invalid_argument("Unsupported domain: " + domain);
}

StatisticalResult MonteCarloSimulationEnv::get_variable_stddev(
    const std::string& var_name, 
    const std::string& domain) {

    validate_variable(var_name);
    auto histories = collect_histories(var_name);

    if (domain == "step") {
        std::vector<double> stddevs(n_steps_);
        for (int step = 0; step < n_steps_; ++step) {
            double mean = 0.0;
            for (int sim = 0; sim < n_subsims_; ++sim) {
                mean += histories[sim][step];
            }
            mean /= n_subsims_;

            double variance = 0.0;
            for (int sim = 0; sim < n_subsims_; ++sim) {
                double diff = histories[sim][step] - mean;
                variance += diff * diff;
            }
            variance /= n_subsims_;
            stddevs[step] = std::sqrt(variance);
        }

        // Overall stddev
        double total_mean = 0.0;
        double total_variance = 0.0;
        int total_values = n_subsims_ * n_steps_;
        for (int sim = 0; sim < n_subsims_; ++sim) {
            for (int step = 0; step < n_steps_; ++step) {
                total_mean += histories[sim][step];
            }
        }
        total_mean /= total_values;
        for (int sim = 0; sim < n_subsims_; ++sim) {
            for (int step = 0; step < n_steps_; ++step) {
                double diff = histories[sim][step] - total_mean;
                total_variance += diff * diff;
            }
        }
        total_variance /= total_values;
        double overall_stddev = std::sqrt(total_variance);

        return StatisticalResult(stddevs, overall_stddev);
    }

    throw std::invalid_argument("Unsupported domain: " + domain);
}

// Implement other statistical methods similarly...

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
            } else if (bin == n_bins) {
                counts[step][n_bins - 1]++;
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

