#include "ch32v003.h"
#include <stdbool.h>
#include <stdint.h>

#define LED_PIN 0U
#define BTN_PIN 4U
#define UART_TX_PIN 5U
#define UART_RX_PIN 6U

#define F_CPU 48000000UL
#define UART_BAUD 115200UL

#define SYSTICK_HZ (F_CPU / 8UL)
#define MS_TO_TICKS(ms) ((uint32_t)(ms) * (SYSTICK_HZ / 1000UL))

#define RCC_HPRE_MASK (0xFUL << 4)
#define RCC_PLLSRC_MASK (1UL << 16)

#define FLASH_LATENCY_1 0x1UL

#define BUTTON_DEBOUNCE_MS 30U

#define PWM_R_PIN 2U /* PD2 = TIM1_CH1 */
#define PWM_G_PIN 1U /* PA1 = TIM1_CH2 */
#define PWM_B_PIN 3U /* PC3 = TIM1_CH3 */

#define PWM_FREQ 20000UL
#define PWM_COUNTS (F_CPU / PWM_FREQ)

void SystemInit(void);
void trap_c(unsigned long mcause, unsigned long mepc);

/* =========================================================
 * CLOCK
 * HSI 24 MHz -> PLL x2 -> SYSCLK 48 MHz -> HCLK /1
 * ========================================================= */
void SystemInit(void) {
  FLASH->ACTLR = (FLASH->ACTLR & ~0x3UL) | FLASH_LATENCY_1;
  /* HCLK = SYSCLK / 1, PLL source = HSI */
  RCC->CFGR0 &= ~(RCC_HPRE_MASK | RCC_PLLSRC_MASK);

  RCC->CTLR |= RCC_PLLON;
  while (!(RCC->CTLR & RCC_PLLRDY)) {
    RCC->CFGR0 = (RCC->CFGR0 & ~RCC_SW_MASK) | RCC_SW_PLL;

    while ((RCC->CFGR0 & RCC_SWS_MASK) != RCC_SWS_PLL) {
      /*
       * Free-running SysTick.
       * STCLK = 0 -> HCLK / 8 = 6 MHz.
       */
      STK_CNTL = 0;
      STK_CTLR = STK_STE;
    }
  }
}

/* =========================================================
 * TRAP
 * ========================================================= */
void trap_c(unsigned long mcause, unsigned long mepc) {
  (void)mepc;
  (void)mcause;

  for (;;) {
  }
}

/* =========================================================
 * GPIO
 * ========================================================= */
static void gpio_cfg(GPIO_TypeDef *port, int pin, uint32_t mode) {
  {
    const uint32_t shift = pin * 4U;

    port->CFGLR = (port->CFGLR & ~(0xFUL << shift)) | (mode << shift);
  }
}
static void gpio_init(void) {
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPCEN | RCC_IOPDEN;
  gpio_cfg(GPIOC, LED_PIN, GPIO_OUT_PP_10);
  gpio_cfg(GPIOD, BTN_PIN, GPIO_IN_PUPD);
  /* * 1 = pull-up 0 = pull-down */
  GPIOD->OUTDR |= 1UL << BTN_PIN;

  /*
   * TIM1 PWM outputs:
   *
   * PD2 = TIM1_CH1 = Red
   * PA1 = TIM1_CH2 = Green
   * PC3 = TIM1_CH3 = Blue
   */
  gpio_cfg(GPIOD, PWM_R_PIN, GPIO_AF_PP_50);
  gpio_cfg(GPIOA, PWM_G_PIN, GPIO_AF_PP_50);
  gpio_cfg(GPIOC, PWM_B_PIN, GPIO_AF_PP_50);
}

static void led_set(bool on) {
  if (on) {
    GPIOC->BSHR = 1UL << LED_PIN;
  } else {
    GPIOC->BSHR = 1UL << (LED_PIN + 16U);
  }
}

static uint32_t button_read(void) { return (GPIOD->INDR >> BTN_PIN) & 1UL; }

/* =========================================================
 * UART
 * ========================================================= */
static void uart_init(void) {
  RCC->APB2PCENR |= RCC_IOPDEN | RCC_USART1EN;

  gpio_cfg(GPIOD, UART_TX_PIN, GPIO_AF_PP_50);
  gpio_cfg(GPIOD, UART_RX_PIN, GPIO_IN_FLOAT);

  USART1->BRR = (F_CPU + UART_BAUD / 2UL) / UART_BAUD;

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

static bool uart_try_getc(uint8_t *c) {
  if (!(USART1->STATR & USART_RXNE)) {
    return false;
  }
  *c = (uint8_t)USART1->DATAR;
  return true;
}

/* =========================================================

 * PWM
 * TIM1:
 *   CH1 -> PD2 -> Red
 *   CH2 -> PA1 -> Green
 *   CH3 -> PC3 -> Blue
 * ========================================================= */

static void pwm_init(void) {
  /* Enable TIM1 peripheral clock. */
  RCC->APB2PCENR |= RCC_TIM1EN;

  /*
   * Stop timer while configuring it.
   *
   * Timer clock = 48 MHz
   * PSC = 0 -> no prescaling
   *
   * 48 MHz / 2400 = 20 kHz
   */

  TIM1->CTLR1 = 0;
  TIM1->PSC = 0;
  TIM1->ATRLR = (uint16_t)(PWM_COUNTS - 1UL);
  TIM1->CH1CVR = 0;
  TIM1->CH2CVR = 0;
  TIM1->CH3CVR = 0;

  /*
   * CH1 and CH2:
   * PWM mode 1 + compare preload.
   */

  TIM1->CHCTLR1 = TIM_OC1_PWM1 | TIM_OC1PE | TIM_OC2_PWM1 | TIM_OC2PE;
  /* CH3:
   * PWM mode 1 + preload
   */
  TIM1->CHCTLR2 = TIM_OC3_PWM1 | TIM_OC3PE;

  TIM1->CCER = TIM_CC1E | TIM_CC2E | TIM_CC3E;

  TIM1->CTLR1 |= TIM_ARPE;

  /*
   * Force registers into active state.
   */
  TIM1->SWEVGR = TIM_UG;

  /*
   * TIM1 is an advanced timer:
   * main output must also be enabled.
   */
  TIM1->BDTR |= TIM_MOE;

  /*
   * Start counter.
   */
  TIM1->CTLR1 |= TIM_CEN;
}

/* =========================================================
 * Application
 * ========================================================= */

static void handle_uart_command(uint8_t c) {
  switch (c) {
  case '1':
    led_set(true);
    break;
  case '0':
    led_set(false);
    break;
  case 'p':
    uart_puts("PONG\n");
    break;

  default:
    break;
  }
}

/* =========================================================
 * MAIN
 * ========================================================= */

int main() {
  gpio_init();
  uart_init();

  uart_puts("CH32V003 ready @ 48 MHz\n");
  /*
   * Debounce state.
   *
   * candidate = most recent raw pin value.
   * stable    = accepted debounced value.
   */

  uint32_t stable = button_read();
  uint32_t candidate = stable;
  uint32_t candidate_since = STK_CNTL;

  for (;;) {

    /* ------- UART ------- */

    uint8_t c;

    if (uart_try_getc(&c)) {
      handle_uart_command(c);
    }

    /* ------- Button ------- */
    const uint32_t now = button_read();
    if (now != candidate) {
      candidate = now;
      candidate_since = STK_CNTL;
    }
    if (candidate != stable) {
      const uint32_t elapsed = (uint32_t)(STK_CNTL - candidate_since);

      if (elapsed >= MS_TO_TICKS(BUTTON_DEBOUNCE_MS)) {
        stable = candidate;

        uart_puts(stable ? "BTN 0\n" : "BTN 1\n");
      }
    }
  }
}
