#include "ch32v003.h"
#include <stdint.h>

#define LED_PIN 0
#define BTN_PIN 4

void SystemInit(void);
void trap_c(unsigned long mcause, unsigned long mepc);

void SystemInit(void) {
  RCC->CFGR0 &= ~(0xFUL << 4);
  FLASH->ACTLR = 0x1;
  RCC->CFGR0 &= ~(1UL << 16);
  RCC->CTLR |= RCC_PLLON;
  while (!(RCC->CTLR & RCC_PLLRDY)) {
  }

  RCC->CFGR0 = (RCC->CFGR0 & ~RCC_SW_MASK) | RCC_SW_PLL;
}

/* -----trap------ */
void trap_c(unsigned long mcause, unsigned long mepc) {
  (void)mepc;
  (void)mcause;
  for (;;) {
  }
}

static void uart_init(void) {
  RCC->APB2PCENR |= RCC_IOPDEN | RCC_USART1EN;

  GPIOD->CFGLR =
      (GPIOD->CFGLR & ~(0xFUL << (5 * 4))) | (GPIO_AF_PP_50 << (5 * 4));

  USART1->BRR = 24000000UL / 115200UL;

  USART1->CTLR1 = USART_UE | USART_TE;
}

static void uart_putc(uint8_t c) {
  while (!(USART1->STATR & USART_TXE)) {
  }

  USART1->DATAR = c;
}

/*
static void delay(uint32_t n) {
  while (n--) {
    __asm volatile("nop");
  }
}
*/

/* ---------- main ---------- */
int main(void) {
  uart_init();
  for (;;) {
    uart_putc(0x55);
  }
}
