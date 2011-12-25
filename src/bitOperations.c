#include <assert.h>
#include "bitOperations.h"

bool testBit (byte value, byte bit) {
   assert (bit < 8);
   return (value & (1<<bit));
}

void setBit (byte *value, byte bit) {
   assert (bit < 8);
   *value |= (1<<bit);
}

void clearBit (byte *value, byte bit) {
   assert (bit < 8);
   if (testBit(*value,bit)) {
      *value ^= (1<<bit);
   }
}

