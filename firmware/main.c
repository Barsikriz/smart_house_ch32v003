#include "ch32v003.h"
#include <stdint.h>

#define LED_PIN 0
#define BTN_PIN 4

void SystemInit(void);
void trap_c(unsigned long mcause, unsigned long mepc)

    /* ----------  HSI 24 МГц → PLL x2 → 48 МГц ---------- */
    void SystemInit(void) {
  FLASH->ACTLR = FLASH_LATENCY_2;
  RCC->CTLR |= RCC_PLLON
};
