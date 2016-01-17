// $Id: ubigint.cpp,v 1.8 2015-07-03 14:46:41-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <algorithm>    // std::reverse used in constructor
using namespace std;

#include "ubigint.h"
#include "debug.h"

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
      // Ugly logic, but avoids accidentally indexing into an operand that's shorter than the other
      if (i < ubig_value.size()) digitSum += ubig_value.at(i);
      if (i < that.ubig_value.size()) digitSum += that.ubig_value.at(i);
      digitSum += carry;
      carry = 0;

      // Check if a carry needs to happen
      if (digitSum > 9) {
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

   int i = 0;           // The index in the vectors
   int borrow = 0;      // The borrow value (if any) - should be reset to 0 on use
   int thisOperand = 0; // Since we can't directly modify ubig_value, this holds the value of a digit in case we need to apply a borrow

   while (i < that.ubig_value.size()) {
      // Handle any borrows
      thisOperand = ubig_value.at(i) - borrow;
      borrow = 0;

      // Check if a borrow needs to happen next iteration
      if (thisOperand < that.ubig_value.at(i)) {
         thisOperand += 10;
         borrow = 1;
      }

      difference.ubig_value.push_back(thisOperand - that.ubig_value.at(i));
      i++;
   }

   // Should also push_back extra digits from this when this is longer than thatSize
   while (i < ubig_value.size()) {
      // But be sure to handle any unresolved borrows!
      if (borrow > 0) {
         difference.ubig_value.push_back(ubig_value.at(i)-1);
         borrow = 0;
      } else {
         difference.ubig_value.push_back(ubig_value.at(i));
      }

      i++;
   }

   while (difference.ubig_value.size() > 0 and difference.ubig_value.back() == 0) difference.ubig_value.pop_back();
   return difference;
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigint product(0);

   int minSize = (ubig_value.size() < that.ubig_value.size() ? ubig_value.size() : that.ubig_value.size());

   while (product.ubig_value.size() > 0 and product.ubig_value.back() == 0) product.ubig_value.pop_back();
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
   if (ubig_value.size() == that.ubig_value.size()) {
      for (int i = 0; i < ubig_value.size(); i++) {
         if (ubig_value.at(i) != that.ubig_value.at(i)) {
            return 0;
         }
      }

      // If we've made it this far, consider the ubigints to be equal. :)
      return 1;
   } else return 0;
}

bool ubigint::operator< (const ubigint& that) const {
   if (ubig_value.size() < that.ubig_value.size()) {
      return 1;
   } else if (ubig_value.size() == that.ubig_value.size()) {
      for (int i = ubig_value.size()-1; i >= 0; i--) {
         if (ubig_value.at(i) >= that.ubig_value.at(i)) {
            return 0;
         }
      }

      // if we've made it this far, consider this to be smaller than that.
      return 1;
   }

   // If this is longer than that, this cannot be smaller.
   return 0;
}

ostream& operator<< (ostream& out, const ubigint& that) {
   string digitBuffer;

   for (int i = that.ubig_value.size()-1; i >= 0; i--) {
      digitBuffer += (that.ubig_value.at(i) + '0');
   }

   return out << digitBuffer;
}

