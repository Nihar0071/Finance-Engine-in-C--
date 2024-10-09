#pragma once
#include "subsim.hpp"
#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <map>
#include <optional>
#include <cmath>
#include <algorithm>

// Statistical results container
struct StatisticalResult {
    std::vector<double> values;
    double overall_value;

    StatisticalResult(std::vector<double> v = {}, double o = 0.0) 
        : values(v), overall_value(o) {}
};

class MonteCarloSimulationEnv {
public:
    MonteCarloSimulationEnv(
        const std::vector<Variable>& variables,
        int n_subsimulations,
        int n_steps
    );

    // Direct setters for simulation functions
    void set_subsim_begin_callback(std::function<void(Context&)> f);
    void set_subsim_step_callback(std::function<void(Context&, int)> f);

    // Run simulations
    void run(bool show_progress = true);

    // Get specific subsimulation environment
    SubSimulationEnv& get_subsim_env(int subsim_index);

    // Statistical analysis functions
    StatisticalResult get_variable_mean(const std::string& var_name, const std::string& domain = "step");
    StatisticalResult get_variable_median(const std::string& var_name, const std::string& domain = "step");
    StatisticalResult get_variable_variance(const std::string& var_name, const std::string& domain = "step");
    StatisticalResult get_variable_stddev(const std::string& var_name, const std::string& domain = "step");
    StatisticalResult get_variable_min(const std::string& var_name, const std::string& domain = "step");
    StatisticalResult get_variable_max(const std::string& var_name, const std::string& domain = "step");
    StatisticalResult get_variable_sum(const std::string& var_name, const std::string& domain = "step");

    // Histogram generation
    struct HistogramResult {
        std::vector<std::vector<double>> counts;
        std::vector<double> bin_edges;
    };

    HistogramResult get_variable_histogram(
        const std::string& var_name,
        int n_bins,
        bool density = false,
        std::optional<std::pair<double, double>> range = std::nullopt
    );

    // Get all histories for a variable
    std::vector<std::vector<double>> get_variable_histories(const std::string& var_name);

private:
    std::vector<Variable> variables_;
    int n_subsims_;
    int n_steps_;
    std::function<void(Context&)> begin_function_;
    std::function<void(Context&, int)> step_function_;
    std::vector<std::unique_ptr<SubSimulationEnv>> subsim_envs_;

    // Helper functions
    void validate_variable(const std::string& var_name) const;
    std::vector<std::vector<double>> collect_histories(const std::string& var_name) const;
};

