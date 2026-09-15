#include "ch32v003.h"
#include <stdint.h>

void SystemInit(void) {}

static void delay(volatile uint32_t n) {
  while (n--) {
    __asm volatile("nop");
  }
}

int main(void) {
  RCC->APB2PCENR |= RCC_IOPCEN;
  GPIOC->CFGLR &= ~(0xFUL << (0 * 4));
  GPIOC->CFGLR |= (0xFUL << (0 * 4));

  for (;;) {
    /* PC0 = 1 */
    GPIOC->BSHR = (1UL << 0);
    delay(500000);

    GPIOC->BSHR = (1UL << (0 + 16));

    delay(500000);
  }
}
