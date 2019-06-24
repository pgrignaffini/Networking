#include "Big_Int.hpp"
#include <cassert>

//default construtor, set the Big_Int as positive
Big_Int::Big_Int()
{
  positive = true;
}

//insert the digits represented by the integer in the vector num
Big_Int::Big_Int(int _num)
{
	if (_num < 0) positive = false;
	else positive = true;

  _num = abs(_num);

	if(_num == 0) num.push_back(0);

  while(_num > 0)
  {
    num.push_back(_num % 10);
    _num = _num/10;
  }

  reverse(num.begin(), num.end());
}

//remove extra 0's caused by the shifting operations
void remove_padding(Big_Int& bi)
{
  if (bi.num.size() == 1 && bi.num[0] == 0) return;

  reverse(bi.num.begin(), bi.num.end());
  while(!bi.num.empty() && bi.num.back() == 0) bi.num.pop_back();
  reverse(bi.num.begin(), bi.num.end());

}

//copy constructor
Big_Int::Big_Int(const Big_Int& other): num(other.num), positive(other.positive){}

//output operator, prints the vector num, if positive == false, prints also the '-' sign
ostream& operator<<(ostream& out, const Big_Int& bi)
{
  if (bi.positive == false) out<<'-';
  for(int index = 0; index < bi.num.size(); index++)
  {
     out << bi.num.at(index);
  }
  return out;
}

//calculate the addition operation through the "column" method
Big_Int Big_Int::operator+(Big_Int& bi)
{
 int res = 0; //store the result of the single column addition
 int rem = 0; //store the remainder of the addition
 Big_Int result;

 if (num.size() < bi.num.size())
  return bi + (*this);

 else
 {

   int diff = num.size() - bi.num.size();
   vector<int> padding;
   for (int c = 0; c<diff; c++) padding.push_back(0);

   padding.insert(padding.end(), bi.num.begin(), bi.num.end());
   bi.num = padding;
   result.num.resize(padding.size() + 1);

   for(int index = num.size() - 1; index >= 0 ; index--)
   {
     res = this->num.at(index) + bi.num.at(index) + rem;

     if(res >= 10)
     {
       res = res - 10;
       rem = 1;
     }

     else
     {
       rem = 0;
     }

     result.num.at(index+1) = res;
   }

   result.num.at(0) = rem;

   remove_padding(result);

   return result;
  }
}

//static version of operator +
Big_Int Big_Int::sum(Big_Int ba, Big_Int bb)
{
 int res = 0;
 int rem = 0;
 Big_Int result;

 if (ba.num.size() < bb.num.size())
  return sum(bb,ba);

 else
 {

   int diff = ba.num.size() - bb.num.size();
   vector<int> padding;
   for (int c = 0; c<diff; c++) padding.push_back(0);

   padding.insert(padding.end(), bb.num.begin(), bb.num.end());
   bb.num = padding;
   result.num.resize(padding.size() + 1);

   for(int index = ba.num.size() - 1; index >= 0 ; index--)
   {
     res = ba.num.at(index) + bb.num.at(index) + rem;

     if(res >= 10)
     {
       res = res - 10;
       rem = 1;
     }

     else
     {
       rem = 0;
     }

     result.num.at(index+1) = res;
   }

   result.num.at(0) = rem;

   remove_padding(result);

   return result;
  }
}

//calculate the subtraction operation through the "column" method
Big_Int Big_Int::operator-(Big_Int& bi)
{
  Big_Int result(0);
  if (this->isGreater(bi)) //-5 > -7 || 7 > 5
  {
    if (this->isPositive() && bi.isPositive()) result = diff(*this,bi);// 7 - 5
    if (!this->isPositive() && !bi.isPositive()) result = diff(bi, (*this)); // -5 - (-7) = (7 - 5)
    if (this->isPositive() && !bi.isPositive()) result = sum((*this), bi);// 5 - (-7) = 5+7

    result.positive = true;
  }

  if (!this->isGreater(bi))
  {
    if (this->isPositive() && bi.isPositive())  result = diff(bi, (*this));
    if (!this->isPositive() && !bi.isPositive()) result = diff((*this), bi);
    if (!this->isPositive() && bi.isPositive()) result = sum((*this),bi);

    result.positive = false;
  }

  return result;
}

//static version of operator -
Big_Int Big_Int::diff(Big_Int ba, Big_Int bi)
{
  int res = -1; //store the result of the single column addition
  int rem = 0; //store the remainder of the addition
  Big_Int result(0);

  if (ba.isEqual(bi))  return result;

  int diff = ba.num.size() - bi.num.size();
  vector<int> padding;
  for (int c = 0; c<diff; c++) padding.push_back(0);

  padding.insert(padding.end(), bi.num.begin(), bi.num.end());
  bi.num = padding;

  result.num.resize(padding.size());

  for(int index = ba.num.size() - 1; index >= 0 ; index--)
  {
    res = ba.num.at(index) - bi.num.at(index);

    if(res < 0)
    {
      ba.num.at(index) += 10;
      ba.num.at(index - 1) -= 1;
      res = ba.num.at(index) - bi.num.at(index);
    }
    result.num.at(index) = res;
  }

  remove_padding(result);

  return result;
}

//implements multiplication between a Big_Int and a single digit int
Big_Int Big_Int::operator*(int m)
{
  int rem = 0;
  int res = 0;
  Big_Int result;
  result.num.resize(this->num.size() + 1);

  for (int jndex = num.size() - 1; jndex >= 0; jndex--)
  {
      res = num[jndex] * m + rem;

      if (res >= 10)
      {
         rem = res / 10;
         res = res % 10;
      }

      else rem = 0;

      result.num[jndex + 1] = res;
  }

  result.num[0] = rem;
  remove_padding(result);

  return result;
}

//pushes 0's at the end of num, the number of 0's pushed is equat to n_zeros
Big_Int Big_Int::shift(int n_zeros)
{
  for (int c = 0; c<n_zeros; c++) num.push_back(0);
  return *this;
}

//implements multiplication between two Big_Int's
Big_Int Big_Int::operator*(Big_Int& bi)
{

   if (bi.num.size() > num.size())
   {
     return bi * (*this);
   }

   else
   {
     Big_Int result, partial_res;
     result.shift(num.size()*2);

     for (int index = bi.num.size() - 1; index >= 0; index--)
     {
       int m = bi.num[index];
       partial_res = (*this) * m;
       partial_res.shift(bi.num.size() - 1 - index);
       //cout << result << " " << partial_res << endl;
       result = result + partial_res;
     }

     remove_padding(result);

     return result;

    }
}

//static version of operator *
Big_Int Big_Int::multiply(Big_Int ba, Big_Int bi)
{
  if (bi.num.size() > ba.num.size())
  {
    return bi * ba;
  }

  else
  {
    Big_Int result, partial_res;
    result.shift(ba.num.size()*2);

    for (int index = bi.num.size() - 1; index >= 0; index--)
    {
      int m = bi.num[index];
      partial_res = (ba) * m;
      partial_res.shift(bi.num.size() - 1 - index);
      //cout << result << " " << partial_res << endl;
      result = result + partial_res;
    }

    remove_padding(result);

    return result;
   }
}

//imlements division between two Big_Int's (naive implementation)
Big_Int Big_Int::operator/(Big_Int& divi)
{
  Big_Int count(0);
  Big_Int one(1);
  while ((*this).isGreater(divi) || (*this).isEqual(divi))
  {
    *this = *this - divi;
    count = count + one;
  }

  return count;
}

//return true iff the object contains just one 0
bool Big_Int::isZero()
{
  remove_padding(*this);
  if (num.size() == 1 && num.at(0) == 0) return true;
  return false;
}

//return true iff the object is positive (namely represents a positive number)
bool Big_Int::isPositive()
{
  return positive;
}

//convert a Big_Int into an Int, if Big_Int is too large, returns 0;
int Big_Int::Big2Int (Big_Int bi)
{
  int decimal = 1;
  int total = 0;
  try
  {
    for (int index = bi.num.size() - 1; index >= 0 ; index--)
    {
      total += bi.num[index] * decimal;
      decimal *= 10;
    }
  }catch(...){ return 0;}

  return total;
}

//return true iff the Big_Int's considered are equal
bool Big_Int::isEqual(Big_Int& bi)
{
  remove_padding(*this);
  remove_padding(bi);

  if (num.size() != bi.num.size()) return false;
  else
  {
    if (this->isPositive() == bi.isPositive())
    {
      for (int index = 0; index < bi.num.size(); index++)
      {
        if (num[index] != bi.num[index]) return false;
      }
    }
    else return false;
  }

  return true;
}

//return true iff the object is greater than the parameter
bool Big_Int::isGreater(Big_Int& bi)
{
  remove_padding(*this);
  remove_padding(bi);

  if (this->isPositive() && !bi.isPositive()) return true;
  if (!this->isPositive() && bi.isPositive()) return false;
  if ((*this).isEqual(bi)) return false;

  if (this->isPositive() && bi.isPositive())
  {
    if (this->num.size() > bi.num.size()) return true;
    if (this->num.size() < bi.num.size()) return false;
  }

  if (!this->isPositive() && !bi.isPositive())
  {
    if (this->num.size() < bi.num.size()) return true;
    if (this->num.size() > bi.num.size()) return false;
  }

  if (this->num.size() == bi.num.size())
  {
    for (int index = 0; index < num.size(); index++)
    {
      if (num.at(index) != bi.num.at(index))
      {
        if (this->isPositive() == 1 && bi.isPositive() == 1) return num.at(index) > bi.num.at(index);
        if (this->isPositive() == 0 && bi.isPositive() == 0) return num.at(index) < bi.num.at(index);
      }
    }
  }

  return false; //stop warning
}

//recursive implementation to calculate the binomial coefficient, it is slow
Big_Int Big_Int::binomial(Big_Int n, Big_Int k)
{
  Big_Int one(1);

  if (k.isZero() || n.isEqual(k)) return Big_Int(1);

  return Big_Int::sum(Big_Int::binomial(n - one, k - one), Big_Int::binomial(n - one, k));
}

//dynamic programming implementation for binomial coefficient
Big_Int Big_Int::binomialCoeff(int n, int k)
{
    Big_Int C[n+1][k+1];
    int i,j;
    // Caculate value of Binomial Coefficient in bottom up manner
    for (i = 0; i <= n; i++)
    {
        for (j = 0; j <= min(i, k); j++)
        {
            // Base Cases
            if (j == 0 || j == i)
                C[i][j] = Big_Int(1);

            // Calculate value using previosly stored values
            else
                C[i][j] = C[i-1][j-1] + C[i-1][j];
        }
    }

    return C[n][k];
}

//compute the factorial of a Big_Int
Big_Int Big_Int::factorial(Big_Int bi)
{
  Big_Int fact(1);
  Big_Int multiplier(1);
  Big_Int one(1);
  Big_Int zero(0);

  while (!bi.isEqual(zero))
  {
    fact = Big_Int::multiply(fact, multiplier);
    multiplier = Big_Int::sum(multiplier,one);
    bi = Big_Int::diff(bi,one);
  }

  return fact;
}
