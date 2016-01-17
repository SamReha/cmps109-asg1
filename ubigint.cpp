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

ubigint ubigint::operator+ (const ubigint& that) const {
   //return ubigint (uvalue + that.uvalue);
   return that;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   //return ubigint (uvalue - that.uvalue);
   return that;
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

