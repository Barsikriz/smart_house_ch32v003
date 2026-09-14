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
static void gpio_cfg(GPIO_TypeDef *port, int pin, uint32_t mode) {
  {
    port->CFGLR = (port->CFGLR & ~(0xFUL << (pin * 4))) | (mode << (pin * 4));
  }
}
static void gpio_init(void) {
  RCC->APB2PCENR |= RCC_IOPCEN | RCC_IOPDEN;

  gpio_cfg(GPIOC, LED_PIN, GPIO_OUT_PP_10);

  gpio_cfg(GPIOD, BTN_PIN, GPIO_IN_PUPD);
  GPIOD->OUTDR |= 1UL << BTN_PIN;

  gpio_cfg(GPIOD, 5, GPIO_AF_PP_50);
  gpio_cfg(GPIOD, 6, GPIO_IN_FLOAT);
}

/* ---------- UART ---------- */
static void uart_init(void) {
  RCC->APB2PCENR |= RCC_USART1EN;
  USART1->BRR = 48000000 / 115200;
  USART1->CTLR1 = USART_UE | USART_TE | USART_RE;
}

static void uart_putc(char c) {
  while (!(USART1->STATR & USART_TXE)) {
  }
  USART1->DATAR = (uint8_t)c;
}

static void uart_puts(const char *s) {
  while (*s) {
    if (*s == '\n')
      uart_putc('\r');
    uart_putc(*s++);
  }
}

/* ---------- main ---------- */
int main(void) {
  uint32_t prev = 1;

  gpio_init();
  uart_init();
  uart_puts("boot 48MHz\n");

  for (;;) {
    if (USART1->STATR & USART_RXNE) {
      uint8_t c = USART1->DATAR;
      if (c == '1')
        GPIOC->BSHR = 1UL << LED_PIN;
      if (c == '0')
        GPIOC->BSHR = 1UL << (LED_PIN + 16);
      if (c == 'p')
        uart_puts("PONG\n");
    }

    uint32_t now = (GPIOD->INDR >> BTN_PIN) & 1;
    if (now != prev) {
      Delay_Ms(30);
      uint32_t stable = (GPIOD->INDR >> BTN_PIN) & 1;
      if (stable == now) {
        prev = stable;
        uart_puts(stable ? "BTN 0\n" : "BTN 1\n");
      }
    }
  }
}
