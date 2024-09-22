#ifndef __VANILLA_OPTION_H
#define __VANILLA_OPTION_H
class Vanilla
{
private:
    /* data */
    void init();
    void copy(const Vanilla& rhs);

    double K; // Strike Rate
    double T; // Expiration Time
    double r; // risk free rate 
    double S; // underlying price rate 
    double sigma;    // Volatility of underlying asset
public:
    // Contains one default constructor and one parameter based constructor and always set a virtual distructor 
    Vanilla(/* args */);  // Default Constructor

    Vanilla(double K, double T, double r, double S, double sigma);  //  Parameterised Constructor 
    
    Vanilla(const Vanilla& rhs); // Copy Constructor 
    
    Vanilla& operator = (const Vanilla& rhs); // Assignment Operator 
    
    virtual ~Vanilla(); //  Distructor 

    //getter methods to access private methods 
    double getK() const;
    double getT() const;
    double getr() const;
    double getS() const;
    double getSigma() const;
    double calc_call_price() const;
    double calc_put_price() const;
};

Vanilla::Vanilla(/* args */)
{
}

Vanilla::~Vanilla()
{
}


#endif