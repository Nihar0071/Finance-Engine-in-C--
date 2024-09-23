#ifndef __VANILLA_OPTION_H
#define __VANILLA_OPTION_H

class Vanilla {
private:
    // Private member variables
    double K;     // Strike price
    double T;     // Time to expiration (years)
    double r;     // Risk-free interest rate
    double S;     // Underlying asset price
    double sigma; // Volatility of the underlying asset

    // Private helper methods
    void init();                      // Initialization method
    void copy(const Vanilla& rhs);    // Copy helper method

public:
    // Constructors and Destructor
    Vanilla();                                      // Default constructor
    Vanilla(double K, double T, double r, double S, double sigma);  // Parameterized constructor
    Vanilla(const Vanilla& rhs);                    // Copy constructor
    Vanilla& operator=(const Vanilla& rhs);         // Assignment operator
    virtual ~Vanilla();                             // Virtual destructor

    // Getter methods
    double getK() const;
    double getT() const;
    double getr() const;
    double getS() const;
    double getSigma() const;

    // Option pricing methods
    double calc_call_price() const;  // Calculate European Call price
    double calc_put_price() const;   // Calculate European Put price
};

// Standard normal cumulative distribution function (to be implemented)
double N(double x);

#endif
