#include <iostream>
#include <cstdlib>
#include <cmath>

double myFunction(double x);
double monteCarloEstimate(double lowBound, double upBound, int iterations);

int main()
{

	double lowerBound, upperBound;
	int iterations;

	lowerBound = 1;
	upperBound = 1000;
	iterations = 1000;

	double estimate = monteCarloEstimate(lowerBound, upperBound,iterations);

	printf("Estimate for %.1f -> %.1f is %.2f, (%i iterations)\n",
			lowerBound, upperBound, estimate, iterations);

	return 0;
}


double myFunction(double x)
//Function to integrate
{
	return pow(x,4)*exp(-x);
}

double monteCarloEstimate(double lowBound, double upBound, int iterations)
//Function to execute Monte Carlo integration on predefined function
{

	double totalSum = 0;
	double randNum, functionVal;

	int iter = 0;

	while (iter<iterations-1)
	{

		//Select a random number within the limits of integration
		randNum = lowBound + (float(rand())/RAND_MAX) * (upBound-lowBound);

		//Sample the function's values
		functionVal = myFunction(randNum);

		//Add the f(x) value to the running sum
		totalSum += functionVal;

		iter++;
	}

	double estimate = (upBound-lowBound)*totalSum/iterations;

	return estimate;
}