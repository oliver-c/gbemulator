#include "bitOperations.h"

bool testBit (byte value, byte bit) {
   return (value & (1<<bit));
}

void setBit (byte *value, byte bit) {
   *value |= (1<<bit);
}

void clearBit (byte *value, byte bit) {
   if (testBit(*value,bit)) {
      *value ^= (1<<bit);
   }
}

