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

   DEBUGF ('~', this << " -> " << that)
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

   // Right off the bat, let's handle the case where one or more of the operands == 0:
   if (ubig_value.size() == 0) {
      for (int i = 0; i < that.ubig_value.size(); i++) {
         sum.ubig_value.push_back(that.ubig_value.at(i));
      }

      return sum;
   } else if (that.ubig_value.size() == 0) {
      for (int i = 0; i < ubig_value.size(); i++) {
         sum.ubig_value.push_back(ubig_value.at(i));
      }

      return sum;
   }

   int minSize = (ubig_value.size() < that.ubig_value.size() ? ubig_value.size() : that.ubig_value.size());
   int i = 0;        // The index in the vectors
   int carry = 0;    // The carry value (if any) - should be reset to 0 on use
   int digitSum = 0; // digitSum represents the sum of two digits (eg, ubig_value.at(0) + that.ubig_value.at(0))
   while (i < minSize or carry > 0) {
      // Ugly logic, but avoids accidentally runnig off the end of an operand that's shorter than the other
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

   // Should also push_back extra digits if one operand is longer than the other
   if (ubig_value.size() > that.ubig_value.size()) {
      while (i < ubig_value.size()) {
         sum.ubig_value.push_back(ubig_value.at(i));
         i++;
      }
   } else {
      while (i < that.ubig_value.size()) {
         sum.ubig_value.push_back(that.ubig_value.at(i));
         i++;
      }
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

   for (int i = 0; i < ubig_value.size(); i++) {
      ubigint partialProduct(0);
      int digitProduct = 0;
      int carry = 0;
      for (int k = 0; k < that.ubig_value.size(); k++) {
         digitProduct = (ubig_value.at(i) * that.ubig_value.at(k)) + carry;
         carry = 0;

         if (digitProduct > 9) {
            carry = digitProduct / 10;
            digitProduct = digitProduct % 10;
         }

         partialProduct.ubig_value.push_back(digitProduct);
      }

      // Handle any left-over carries
      if (carry > 0) {
         partialProduct.ubig_value.push_back(carry);
         carry = 0;
      }

      // Do a quicky version of multiplying by 10 to handle the offset when forming partial products
      int offset = 0;
      while (offset < i) {
         partialProduct.ubig_value.insert(partialProduct.ubig_value.begin(), 0); // Appends 0's to base of ubig_value
         offset++;
      }

      product = product + partialProduct;
      partialProduct.ubig_value.clear();
   }

   while (product.ubig_value.size() > 0 and product.ubig_value.back() == 0) product.ubig_value.pop_back();
   return product;
}

/* Handy internal helper functions for division */
void ubigint::multiply_by_2() {
   // Just a simpler case of multiplication
   int carry = 0;
   for (int i = 0; i < ubig_value.size(); i++) {
      int digitProduct = (ubig_value.at(i) * 2) + carry;
      carry = 0;

      // Check if a carry needs to be generated
      if (digitProduct > 9) {
         carry = digitProduct / 10;
         digitProduct = digitProduct % 10;
      }

      ubig_value.at(i) = digitProduct;
   }

   // Handle any left-over carries
   if (carry > 0) {
      ubig_value.push_back(carry);
   }

   while (ubig_value.size() > 0 and ubig_value.back() == 0) ubig_value.pop_back();
}

void ubigint::divide_by_2() {
   int remainder = 0; // because we're only ever dividing by two, remainder can only equal 0 or 5

   // Iterate backwards (since high order bits determine remainders that effect low order bits)
   for (int i = ubig_value.size()-1; i >= 0; i--) {
      int digitQuotient = 0;
      if (remainder > 0) {
         digitQuotient += remainder;
         remainder = 0;
      }

      digitQuotient += ubig_value.at(i) / 2;

      if (ubig_value.at(i) % 2) { // if (ubig_value.at(i) is odd) {
         remainder = 5;
      }

      ubig_value.at(i) = digitQuotient;
   }

   while (ubig_value.size() > 0 and ubig_value.back() == 0) ubig_value.pop_back();
}

ubigint::quot_rem ubigint::divide (const ubigint& that) const {
   static const ubigint zero(0);
   if (that == zero) throw domain_error ("ubigint::divide: by 0");
   ubigint power_of_2 = ubigint(1);
   ubigint divisor = that;    // right operand, divisor
   ubigint quotient = ubigint(0);
   ubigint remainder = *this; // left operand, dividend

   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }

   while (zero < power_of_2) {
      if ((divisor < remainder) or (divisor == remainder)) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }

   while (quotient.ubig_value.size() > 0 and quotient.ubig_value.back() == 0) quotient.ubig_value.pop_back();
   while (remainder.ubig_value.size() > 0 and remainder.ubig_value.back() == 0) remainder.ubig_value.pop_back();
   return {quotient, remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return divide(that).first;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return divide(that).second;
}

/* Comparison operations */
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
   // Handle trivial cases
   if (*this == that) return 0;
   if (ubig_value.size() < that.ubig_value.size()) return 1;

   if (ubig_value.size() == that.ubig_value.size()) {
      // First, check the corner case that we're comparing two ubigints of size 0
      if (ubig_value.size() == 0) return 0;

      for (int i = ubig_value.size()-1; i >= 0; i--) {
         if (ubig_value.at(i) > that.ubig_value.at(i)) {
            return 0;
         }
      }

      // if we've made it this far, consider this to be smaller than that.
      return 1;
   }

   // If this is longer than that, this cannot be smaller.
   return 0;
}

// Print to ostream
ostream& operator<< (ostream& out, const ubigint& that) {
   string digitBuffer;
   int lineLength = 70;
   if (that.ubig_value.size() > 0) {
      int charCount = 0;

      for (int i = that.ubig_value.size()-1; i >= 0; i--) {
         // Wrap lines as necessary (but should probably avoid magic numbers
         if (charCount%(lineLength-1) == 0 and charCount != 0) out << "\\" << endl;
         out << static_cast<char>(that.ubig_value.at(i) + '0');
         charCount++;
      }
   } else { // else if ubig_value is empty, then that represents 0.
      out << '0';
   }

   return out;
}

