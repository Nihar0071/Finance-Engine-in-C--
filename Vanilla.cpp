#include "Vanilla.hpp"
#include <cmath>
#include <algorithm> // for std::max

// Standard normal cumulative distribution function (CDF)
double N(double x) {
    return 0.5 * erfc(-x * M_SQRT1_2); // N(x) using error function approximation
}

void Vanilla::init() {
    // Initialize the Vanilla class with default values
    K = 100.0;      // Strike price
    r = 0.05;       // 5% risk-free interest rate
    sigma = 0.2;    // 20% volatility
    T = 1.0;        // 1 year until maturity
    S = 100.0;      // Underlying asset price at-the-money
}

void Vanilla::copy(const Vanilla& rhs) {
    // Copy the Vanilla class data members
    K = rhs.K;
    r = rhs.r;
    sigma = rhs.sigma;
    T = rhs.T;
    S = rhs.S;
}

// Default constructor
Vanilla::Vanilla() {
    init();
}

// Copy constructor
Vanilla::Vanilla(const Vanilla& rhs) {
    copy(rhs);
}

// Parameterized constructor
Vanilla::Vanilla(double _K, double _r, double _T, double _S, double _sigma) {
    K = _K;
    r = _r;
    T = _T;
    S = _S;
    sigma = _sigma;
}

// Assignment operator
Vanilla& Vanilla::operator=(const Vanilla& rhs) {
    if (this == &rhs) return *this;  // Handle self-assignment
    copy(rhs);
    return *this;
}

// Destructor
Vanilla::~Vanilla() {
    // Destructor
}

// Getter methods for private variables
double Vanilla::getK() const { return K; }
double Vanilla::getr() const { return r; }
double Vanilla::getT() const { return T; }
double Vanilla::getS() const { return S; }
double Vanilla::getSigma() const { return sigma; }

// Function to calculate call option price using Black-Scholes formula
double Vanilla::calc_call_price() const {
    double sigma_sqrt_T = sigma * sqrt(T);
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / sigma_sqrt_T;
    double d2 = d1 - sigma_sqrt_T;
    
    return S * N(d1) - K * exp(-r * T) * N(d2);
}

// Function to calculate put option price using Black-Scholes formula
double Vanilla::calc_put_price() const {
    double sigma_sqrt_T = sigma * sqrt(T);
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / sigma_sqrt_T;
    double d2 = d1 - sigma_sqrt_T;
    
    return K * exp(-r * T) * N(-d2) - S * N(-d1);
}
