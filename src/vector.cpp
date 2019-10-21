#include <cmath>

#include "vector.h"

mathVector::mathVector(){
    x=0; //default vector is a zero vector
    y=0;
}

mathVector::mathVector(double v_x, double v_y)
{
    setX(v_x);
    setY(v_y);
}

mathVector::~mathVector()
{
    //dtor
}

double mathVector::getX(){
    return x;
}

double mathVector::getY(){
    return y;
}

void mathVector::setX(double v_x){
    x = v_x;
}

void mathVector::setY(double v_y){
    y = v_y;
}

void mathVector::setVector(double v_x, double v_y){
    setX(v_x);
    setY(v_y);
}

void mathVector::setVector(mathVector v){
    x = v.getX();
    y = v.getY();
}

mathVector mathVector::unit(){
    return mathVector(x,y).scale(1/magnitude()); // divides the vector by the magnitude
}

double mathVector::magnitude(){
    return sqrt(x*x+y*y);
}

double mathVector::angle(){
    return atan2(y,x); //returns the angle in the full range of 0 to 2pi
}

mathVector mathVector::add(mathVector v){
    return mathVector(x + v.getX(), y + v.getY());
}

mathVector mathVector::subtract(mathVector v){
    return mathVector(x-v.getX(), y-v.getY());
}

mathVector mathVector::scale(double s){
    return mathVector(s*x, s*y);
}

double mathVector::dot(mathVector v){
    return x*v.getX() + y*v.getY();
}

double mathVector::zCross(mathVector v){
    return (x*v.getY() - y*v.getX()); //returns the z component of the cross product
}

mathVector mathVector::operator + (mathVector v){
    return add(v);
}

mathVector mathVector::operator - (mathVector v){
    return subtract(v);
}

mathVector mathVector::operator * (double s){
    return scale(s);
}

double mathVector::operator & (mathVector v){
    return dot(v);
}

double mathVector::operator % (mathVector v){
    return zCross(v);
}

double mathVector::getDistTo(mathVector v_j){
    double dx = x - v_j.getX();
    double dy = y - v_j.getY();
    return sqrt(dx*dx + dy*dy);
}

mathVector mathVector::getPolarVector(double magnitude, double angle){ //generate a cartesian vector from polar coords
    //generate a unit vector pointing in the right direction first
    mathVector unit = mathVector(std::cos(angle), std::sin(angle));
    //now scale and return it
    return unit*magnitude;
}
