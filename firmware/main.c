#include "ch32v003.h"
#include <stdint.h>

#define LED_PIN 0
#define BTN_PIN 4

void SystemInit(void);
void trap_c(unsigned long mcause, unsigned long mepc);

static void delay(uint32_t n) {
  while (n--) {
    __asm volatile("nop");
  }
}

/* -----trap------ */
void trap_c(unsigned long mcause, unsigned long mepc) {
  (void)mepc;
  (void)mcause;
  for (;;) {
  }
}

/* ---------- main ---------- */
int main(void) {
  RCC->APB2PCENR |= RCC_IOPCEN | RCC_IOPDEN;

  GPIOC->CFGLR =
      (GPIOD->CFGLR & ~(0xFUL << (5 * 4))) | (GPIO_OUT_PP_10 << (5 * 4));

  for (;;) {
    GPIOC->BSHR = 1UL << 0;
    GPIOD->BSHR = 1UL << 5;

    delay(200000);

    GPIOC->BSHR = 1UL << 16;
    GPIOD->BSHR = 1UL << (5 + 16);
    delay(200000);
  }
}
