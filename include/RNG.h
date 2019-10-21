#ifndef RNG_H
#define RNG_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

class RNG
{
    public:
        RNG(); // init with a seed based on the current system time
        RNG(unsigned long int seed); // init with a specific seed
        virtual ~RNG();

        // uniform generators
        double getUniformUnit(); // returns a uniform double in the interval (0,1)
        long int getUniformInt(int minVal, int maxVal); // returns a uniform integer in [minVal,maxVal]
        double getUniformDouble(double minVal, double maxVal); // returns a uniform double in [minVal, maxVal)

        // gauss generators
        double getGauss(double sigma); // gets a random gaussian distribution with mean 0, SD sigma
        double getGauss(double x, double sigma); // gets a random gaussian distribution with mean x, SD sigma

    private:
        gsl_rng *r; // the random number generator itself

        void seededInit(unsigned long int seed); //initialise the generator with specified seed
};

#endif // RNG_H
