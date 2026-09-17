#include "ch32v003.h"
#include <stdint.h>

#define LED_PIN 0U
#define BTN_PIN 4U
#define UART_TX_PIN 5U
#define UART_RX_PIN 6U

#define F_CPU 48000000UL
#define UART_BAUD 115200UL

#define RCC_HPRE_MASK (0xFUL << 4)
#define RCC_PLLSRC_MASK (1UL << 16)

#define FLASH_LATENCY_1 0x1UL

void SystemInit(void);
void trap_c(unsigned long mcause, unsigned long mepc);

/* =========================================================
 * CLOCK
 * HSI 24 MHz -> PLL x2 -> SYSCLK 48 MHz -> HCLK /1
 * ========================================================= */
void SystemInit(void) {
  /*
   * Flash latency = 1 for 48 MHz.
   */
  FLASH->ACTLR = (FLASH->ACTLR & ~0x3UL) | FLASH_LATENCY_1;

  /*
   * HPRE = 0000
   * HCLK = SYSCLK / 1
   */

  RCC->CFGR0 &= ~RCC_HPRE_MASK;
  /*
   * PLL boot.
   */
  /* PLL source = HSI */
  RCC->CFGR0 &= ~RCC_PLLSRC_MASK;
  RCC->CTLR |= RCC_PLLON;
  /*
   *  waiting for pll stabilization
   *  */
  while (!(RCC->CTLR & RCC_PLLRDY)) {
  }
  /*
   *SYSCLK <- PLL
   * */

  RCC->CFGR0 = (RCC->CFGR0 & ~RCC_SW_MASK) | RCC_SW_PLL;

  /*
   *waiting..
   * */
  while ((RCC->CFGR0 & RCC_SWS_MASK) != RCC_SWS_PLL) {
  }

  /*
   * SysTick enabled.
   * STCLK=0 -> HCLK/8.
   *
   * 48 MHz / 8 = 6 MHz.
   */
  STK_CNTL = 0;
  STK_CTLR = 0;
  STK_SR = 0;
}

/* =========================================================
 * DELAY
 * ========================================================= */
void Delay_Ms(uint32_t ms) {
  /*
   * SysTick = 48 MHz / 8 = 6 MHz
   *
   * 6 000 000 ticks/sec
   * 6000 ticks/ms
   */

  uint32_t ticks = ms * (F_CPU / 8UL / 1000UL);

  STK_CTLR = 0;
  STK_CNTL = 0;
  STK_SR = 0;
  STK_CMPLR = ticks;
  STK_CTLR = STK_STE;
  while (!(STK_SR & STK_CNTIF)) {
  }
  STK_CTLR = 0;
  STK_SR = 0;
}

/* =========================================================
 * TRAP
 * ========================================================= */
void trap_c(unsigned long mcause, unsigned long mepc) {
  (void)mepc;
  (void)mcause;

  /* inf loop if trap  */
  for (;;) {
  }
}

/* =========================================================
 * GPIO
 * ========================================================= */
static void gpio_cfg(GPIO_TypeDef *port, int pin, uint32_t mode) {
  {
    port->CFGLR = (port->CFGLR & ~(0xFUL << (pin * 4))) | (mode << (pin * 4));
  }
}
static void gpio_init(void) {
  /*
   * Enabling clock GPIOC и GPIOD.
   */
  RCC->APB2PCENR |= RCC_IOPCEN | RCC_IOPDEN;

  /*
   * PC0 = LED output push-pull.
   */
  gpio_cfg(GPIOC, LED_PIN, GPIO_OUT_PP_10);

  /*
   * PD4 = input pull-up/pull-down.
   */
  gpio_cfg(GPIOD, BTN_PIN, GPIO_IN_PUPD);
  /*
   * 1 = pull-up
   * 0 = pull-down
   */
  GPIOD->OUTDR |= 1UL << BTN_PIN;
}

/* =========================================================
 * UART
 * ========================================================= */
static void uart_init(void) {
  RCC->APB2PCENR |= RCC_IOPDEN | RCC_USART1EN;
  /*
   * PD5 = USART1 TX
   * Alternate Function Push-Pull
   */
  gpio_cfg(GPIOD, UART_TX_PIN, GPIO_AF_PP_50);
  /*
   * PD6 = USART1 RX
   * floating input
   */
  gpio_cfg(GPIOD, UART_RX_PIN, GPIO_IN_FLOAT);
  /*
   * 48 MHz / 115200 ~= 416.67
   * round to 417.
   */
  USART1->BRR = (F_CPU + UART_BAUD / 2UL) / UART_BAUD;
  /*
   * UE = USART enable
   * TE = transmitter enable
   * RE = receiver enable
   */
  USART1->CTLR1 = USART_UE | USART_TE | USART_RE;
}

static void uart_putc(uint8_t c) {
  while (!(USART1->STATR & USART_TXE)) {
  }
  USART1->DATAR = c;
}

static void uart_puts(const char *s) {
  while (*s) {
    if (*s == '\n')
      uart_putc('\r');
    uart_putc((uint8_t)*s++);
  }
}

/* =========================================================
 * MAIN
 * ========================================================= */
int main(void) {
  uint32_t prev = 1;

  gpio_init();
  uart_init();
  for (;;) {
    uart_puts("tick\n");
    Delay_Ms(500);
  }
  uart_puts("boot 48MHz\n");

  for (;;) {
    /* ---------- UART RX ---------- */
    if (USART1->STATR & USART_RXNE) {
      uint8_t c = (uint8_t)USART1->DATAR;
      if (c == '1')
        GPIOC->BSHR = 1UL << LED_PIN;
      if (c == '0')
        GPIOC->BSHR = 1UL << (LED_PIN + 16);
      if (c == 'p')
        uart_puts("PONG\n");
    }

    /* ---------- BUTTON ---------- */
    uint32_t now = (GPIOD->INDR >> BTN_PIN) & 1UL;
    if (now != prev) {
      Delay_Ms(30);
      uint32_t stable = (GPIOD->INDR >> BTN_PIN) & 1UL;
      if (stable == now) {
        prev = stable;
        /*
         * pull-up:
         *
         * released = 1
         * pressed  = 0
         */
        uart_puts(stable ? "BTN 0\n" : "BTN 1\n");
      }
    }
  }
}
