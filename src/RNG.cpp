#include <ctime>

#include "RNG.h"

RNG::RNG() // system time constructor
{
    // get system time and store it to use as a seed
    long int currTimeSeed = static_cast<int>(time(NULL));

    // use that as the seed
    seededInit(currTimeSeed); // use the above as a seed
}

RNG::RNG(unsigned long int seed){
    seededInit(seed);
}

RNG::~RNG()
{
    //dtor
    gsl_rng_free(r); // frees the generator as per GSL docs
}

double RNG::getUniformUnit(){ //returns a uniform double in the closed unit interval
    return gsl_rng_uniform_pos(r);
}

long int RNG::getUniformInt(int minVal, int maxVal){
    int diff = maxVal-minVal;

    return (minVal + gsl_rng_uniform_int(r, diff+1));
}

double RNG::getUniformDouble(double minVal, double maxVal){
    return gsl_ran_flat(r, minVal, maxVal);
}

double RNG::getGauss(double sigma){
    return gsl_ran_gaussian(r, sigma);
}

double RNG::getGauss(double x, double sigma){
    return (x+getGauss(sigma)); // simply apply a transformation
}

void RNG::seededInit(unsigned long int seed){ // initialise the constructor with a specifc
    //first, set up the random number generator as a Taus generator:
    r = gsl_rng_alloc(gsl_rng_taus);

    //now, seed the generator
    gsl_rng_set(r, seed);

    return; // all done
}
