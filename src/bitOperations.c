#include "bitOperations.h"

bool testBit (int value, int bit) {
   return (value & (1<<bit));
}

void setBit (int *value, int bit) {
   *value |= (1<<bit);
}

void clearBit (int *value, int bit) {
   if (testBit(*value,bit)) {
      *value ^= (1<<bit);
   }
}

