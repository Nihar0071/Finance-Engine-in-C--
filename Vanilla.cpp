#include "Vanilla.hpp";
#include<cmath>;

void Vanilla::init() {
    // Initialize the Vanilla class
    K = 100.0;
    
    r = 0.05; // 5% interest rate
    
    sigma = 0.2; // volatility is 20%
    
    T = 1.0; // 1 year till maturity
    
    S = 100; // option is at money when in strike 

    
}
void Vanilla::copy(const Vanilla& rhs){
        // Copy the Vanilla class
        K = rhs.K;
        r = rhs.r;
        sigma = rhs.sigma;
        T = rhs.T;
        S = rhs.S;
}

Vanilla::Vanilla(){
    init();
}

Vanilla::Vanilla(const Vanilla& rhs){
    copy(rhs);
}
