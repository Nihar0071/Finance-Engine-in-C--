#pragma once
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <memory>
#include <variant>
#include <any>
#include <stdexcept>
#include <typeinfo>
#include <iostream>

// Forward declarations
class Context;
class SubSimulationEnv;

// Type alias for variant to handle multiple types
using ValueType = std::variant<int, double, bool, std::string>;

// Structure to hold variable information
struct Variable {
    std::string name;
    const std::type_info& type;
    ValueType default_value;
    
    template<typename T>
    Variable(const std::string& n, const T& default_val)
        : name(n), type(typeid(T)), default_value(default_val) {}
};

class Context {
private:
    SubSimulationEnv* env;
    std::map<std::string, std::any> auxiliary;
    bool readonly;

public:
    Context(SubSimulationEnv* e, bool ro = false) : env(e), readonly(ro) {}
    
    template<typename T>
    void setState(const std::string& name, const T& value);
    
    template<typename T>
    T getState(const std::string& name) const;
    
    std::shared_ptr<Context> past(int n) const;
    
    template<typename T>
    void setAuxiliary(const std::string& name, const T& value) {
        if (readonly) throw std::runtime_error("Context is read-only");
        auxiliary[name] = value;
    }
    
    template<typename T>
    T getAuxiliary(const std::string& name) const {
        auto it = auxiliary.find(name);
        if (it == auxiliary.end()) throw std::runtime_error("Auxiliary not found");
        return std::any_cast<T>(it->second);
    }
};

class SubSimulationEnv {
private:
    std::vector<Variable> variables;
    std::map<std::string, std::vector<ValueType>> history;
    std::map<std::string, ValueType> current_states;
    std::function<void(Context&)> begin_function;
    std::function<void(Context&, int)> step_function;
    int steps_taken;

public:
    SubSimulationEnv(
        const std::vector<Variable>& vars,
        std::function<void(Context&)> begin_fn,
        std::function<void(Context&, int)> step_fn
    );

    void runSteps(int n);

    // Get the history of a specific variable
    template<typename T>
    std::vector<T> getVariableHistory(const std::string& var_name) const;

private:
    void logStates();

    friend class Context;
};

// Implementation of Context methods
template<typename T>
void Context::setState(const std::string& name, const T& value) {
    if (readonly) throw std::runtime_error("Context is read-only");
    env->current_states[name] = value;
}

template<typename T>
T Context::getState(const std::string& name) const {
    auto it = env->current_states.find(name);
    if (it == env->current_states.end()) 
        throw std::runtime_error("State not found");
    return std::get<T>(it->second);
}

inline std::shared_ptr<Context> Context::past(int n) const {
    if (n < 1 || n > env->steps_taken)
        throw std::runtime_error("Invalid step number");

    // For simplicity, return a read-only context with no past functionality
    auto context = std::make_shared<Context>(env, true);
    return context;
}

// Implementation of SubSimulationEnv methods
template<typename T>
std::vector<T> SubSimulationEnv::getVariableHistory(const std::string& var_name) const {
    auto it = history.find(var_name);
    if (it == history.end())
        throw std::runtime_error("Variable not found");

    std::vector<T> result;
    for (const auto& value : it->second) {
        result.push_back(std::get<T>(value));
    }
    return result;
}

