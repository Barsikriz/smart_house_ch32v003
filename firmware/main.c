#include "ch32v003.h"
#include <stdint.h>

#define LED_PIN 0
#define BTN_PIN 4

void SystemInit(void);
void trap_c(unsigned long mcause, unsigned long mepc);

/* ----------  HSI 24 МГц → PLL x2 → 48 МГц ---------- */
void SystemInit(void) {
  FLASH->ACTLR = FLASH_LATENCY_2;
  RCC->CTLR |= RCC_PLLON;
  while (!(RCC->CTLR & RCC_PLLRDY)) {
  }
  RCC->CFGR0 = (RCC->CFGR0 & ~RCC_SW_MASK) | RCC_SW_PLL;
  while ((RCC->CFGR0 & RCC_SWS_MASK) != RCC_SWS_PLL) {
  }
}
