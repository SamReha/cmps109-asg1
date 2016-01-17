// $Id: ubigint.cpp,v 1.8 2015-07-03 14:46:41-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <algorithm>    // std::reverse used in constructor
using namespace std;

#include "ubigint.h"
#include "debug.h"

//ubigint::ubigint (unsigned long that): uvalue (that) {
//   DEBUGF ('~', this << " -> " << uvalue)
//}

/* Constructors */
ubigint::ubigint (unsigned long that) {
   while (that > 0) {
      ubig_value.push_back(that % 10);
      that = that/10;
   }

   //DEBUGF ('~', this << " -> " << that) - commented out only for use in ubiginttester
}

ubigint::ubigint (const string& that) {
   for (char digit : that) {
      ubig_value.push_back(digit - '0');
   }

   reverse(ubig_value.begin(), ubig_value.end());
}

/* Arithmetic Operations */
ubigint ubigint::operator+ (const ubigint& that) const {
   ubigint sum(0);

   int minSize = (ubig_value.size() < that.ubig_value.size() ? ubig_value.size() : that.ubig_value.size());
   int i = 0;        // The index in the vectors
   int carry = 0;    // The carry value (if any) - should be reset to 0 on use
   int digitSum = 0; // digitSum represents the sum of two digits (eg, ubig_value.at(0) + that.ubig_value.at(0))
   while (i < minSize or carry > 0) {
      if (i < ubig_value.size()) digitSum += ubig_value.at(i);
      if (i < that.ubig_value.size()) digitSum += that.ubig_value.at(i);
      digitSum += carry;
      carry = 0;

      if (digitSum > 9) { // Which is to say, if digitSum is larger than 2 digits in a base 10 counting system
         carry = 1;
         digitSum = digitSum % 10;
      }

      sum.ubig_value.push_back(digitSum);
      digitSum = 0;
      i++;
   }

   while (sum.ubig_value.size() > 0 and sum.ubig_value.back() == 0) sum.ubig_value.pop_back();
   return sum;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigint difference(0);

   int thatSize = that.ubig_value.size();
   int i = 0;           // The index in the vectors
   int borrow = 0;      // The borrow value (if any) - should be reset to 0 on use
   int digitDiff = 0;   // The difference of two digits
   int thisOperand = 0; // Since we can't directly modify ubig_value, this holds the value of a digit in case we need to apply a borrow

   while (i < thatSize) {
      ((borrow == 0) ? thisOperand = ubig_value.at(i) : ubig_value.at(i) - 1); // This may have issues when dealing with borrows that extend beyond the reach of thatSize
      borrow = 0;

      if (thisOperand < that.ubig_value.at(i)) {
         borrow = 10;
      }

      digitDiff = (thisOperand + borrow) - that.ubig_value.at(i);
      difference.ubig_value.push_back(digitDiff);

      i++;
   }

   // Should also push_back extra digits from this when this is longer than thatSize
   if (i < ubigint_value.size()) {
   }

   while (difference.ubig_value.size() > 0 and difference.ubig_value.back() == 0) difference.ubig_value.pop_back();
   return difference;
}

ubigint ubigint::operator* (const ubigint& that) const {
   //return ubigint (uvalue * that.uvalue);
   return that;
}

void ubigint::multiply_by_2() {
   //uvalue *= 2;
}

void ubigint::divide_by_2() {
   //uvalue /= 2;
}

ubigint::quot_rem ubigint::divide (const ubigint& that) const {
   static const ubigint zero = 0;
   if (that == zero) throw domain_error ("ubigint::divide: by 0");
   ubigint power_of_2 = 1;
   ubigint divisor = that; // right operand, divisor
   ubigint quotient = 0;
   ubigint remainder = *this; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {quotient, remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return divide (that).first;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return divide (that).second;
}

bool ubigint::operator== (const ubigint& that) const {
   //return uvalue == that.uvalue;
   return 1;
}

bool ubigint::operator< (const ubigint& that) const {
   //return uvalue < that.uvalue;
   return 0;
}

ostream& operator<< (ostream& out, const ubigint& that) {
   string digitBuffer;

   for (int i = that.ubig_value.size()-1; i >= 0; i--) {
      digitBuffer += (that.ubig_value.at(i) + '0');
   }

   return out << digitBuffer;
}

