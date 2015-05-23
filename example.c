#include "fpu_dump.h"

int main(void) {
  double x = 243;
  FPUDUMP;
  x = x / 3.14;
  FPUDUMP;
  return 0;
}
