#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

class Big_Int
{

public:

   vector<int> num; //num contains our big number, it is stored in standard order
   bool positive; //flag for positive or negative numbers

   //basic constructor, Big_Int is positive by default
   Big_Int();
   //takes and integer and convert it into a Big_Int
   Big_Int(int _num);
   //copy constructor
   Big_Int(const Big_Int& a);

   //implements addition between two Big_Int
   Big_Int operator+(Big_Int& a);
   //implements multiplication between two Big_Int
   Big_Int operator*(Big_Int& a);
   //implements multiplication between a Big_Int and a single digit int, it is a method
   //that's called inside the above function
   Big_Int operator*(int m);
   //implements substraction between two Big_Int, negative Big_Int are also considered
   Big_Int operator-(Big_Int& a);
   //naive implementation of division between two Big_Int
   Big_Int operator/(Big_Int& bi);

  //binomial of Big_Int's, it has a recursive implementation so it is pretty slow
   static Big_Int binomial(Big_Int n, Big_Int k);
  //static implementation of operator +
   static Big_Int sum(Big_Int ba, Big_Int bb);
  //static implementation of operator -
   static Big_Int diff(Big_Int ba, Big_Int bb);
  //calculates the factorial of the given Big_Int
   static Big_Int factorial(Big_Int bi);
  //static implementation of operator *
   static Big_Int multiply(Big_Int ba, Big_Int bb);
   //uses dynamic programming paradigm, the element inside the table used to store the partial results are Big_Int
   static Big_Int binomialCoeff(int n, int k);
   //utility for conversion
   static int Big2Int (Big_Int bi);

   //return true iff the object contains just one 0
   bool isZero();
   //return true iff the Big_Int's considered are equal
   bool isEqual(Big_Int& bi);
   //return true iff the object is positive (namely represents a positive number)
   bool isPositive();
   //return true iff the object is greater than the parameter
   bool isGreater(Big_Int& bi);
   //method used for the implementation of multiplication, it pushes 0's on vector num depending on the parameter
   Big_Int shift(int n_zeros);
   //output function
   friend ostream& operator<<(ostream& out, const Big_Int& a);

};
