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
  STK_CNTL = 0;
  STK_CTLR = STK_STE;
}

/* Delay */
void Delay_Ms(uint32_t ms) {
  uint32_t start = STK_CNTL;
  uint32_t ticks = ms * 6000;
  while ((uint32_t)(STK_CNTL - start) < ticks) {
  }
}

/* -----trap------ */
void trap_c(unsigned long mcause, unsigned long mepc) {
  (void)mepc;
  for (;;) {
    GPIOC->BSHR = 1UL << (LED_PIN + 16);
    Delay_Ms(80);
    GPIOC->BSHR = 1UL << LED_PIN;
    Delay_Ms(80);
  }
}

/* ------ GPIO ----- */
static void gpio_cfg(GPIO_TypeDef, *port, int pin, uint32_t mode) {
  {
    port->CFGLR = (port->CFGLR & ~(0xFUL << (pin * 4))) | (mode << (pin * 4));
  }
}
