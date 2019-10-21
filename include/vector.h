#pragma once

class mathVector
{
    public:
        mathVector(); // dummy constructor
        mathVector(double v_x, double v_y); // initialise mathVector with accompanying values
        virtual ~mathVector();

        // position gets/sets
        double getX();
        double getY();
        void setX(double v_x);
        void setY(double v_y);
        void setVector(double v_x, double v_y);
        void setVector(mathVector v);

        // mathematical
        mathVector unit(); // get unit vector
        double magnitude(); // get vector magnitude
        double angle(); // get vector angle

        //operations
        mathVector add(mathVector v);
        mathVector subtract(mathVector v);
        mathVector scale(double s);
        double dot(mathVector v); // dot product
        double zCross(mathVector v); // returns the z component of the cross product this X v

        //C++ operators for the above operations
        mathVector operator + (mathVector v); // operator for add
        mathVector operator - (mathVector v); // operator for subtract
        mathVector operator * (double s); // operator for scale
        double operator & (mathVector v); // operator for dot
        double operator % (mathVector v); // operator for zCross

        //auxiliary
        double getDistTo(mathVector v_j); //get distance from this vector to another vector v_j without considering BCs
        static mathVector getPolarVector(double magnitude, double angle); //generate a cartesian vector using polar coordinates

    private:
        double x;
        double y;
};
