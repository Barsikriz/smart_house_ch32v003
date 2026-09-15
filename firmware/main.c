#include "ch32v003.h"
#include <stdint.h>

void SystemInit(void) {}

void trap_c(unsigned long mcause, unsigned long mepc) {
  (void)mcause;
  (void)mepc;
}

static void delay(volatile uint32_t n) {
  while (n--) {
    __asm volatile("nop");
  }
}

int main(void) {
  RCC->APB2PCENR |= RCC_IOPCEN;
  GPIOC->CFGLR = (GPIOC->CFGLR & ~0xFUL) | 0x1UL;

  for (;;) {
    /* PC0 = 1 */
    GPIOC->BSHR = 1UL;
    delay(500000);

    GPIOC->BSHR = 1UL << 16;

    delay(500000);
  }
}
