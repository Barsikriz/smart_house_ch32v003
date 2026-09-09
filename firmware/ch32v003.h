#ifndef CH32V003_H
#define CH32V003_H

#include <stdint.h>

#define REG32(a) (*(volatile uint32_t *)(a))
#define __IO volatile

/* --- SysTick (core private) 64 bit, HCLK/8= 6Mhz --- */
#define STK_CTLR REG32(0xE000F000)  // System count control register
#define STK_SR REG32(0xE000F004)    // System count status register
#define STK_CNTL REG32(0xE000F008)  // System counter register
#define STK_CMPLR REG32(0xE000F010) // Counting comparison register

#define STK_STE (1UL << 0)  // System counter enable
#define STK_STIE (1UL << 1) // Counter interrupt enable
#define STK_STCLK                                                              \
  (1UL << 2) // Counter clock source selection bit.  1: HCLK for time base.  0:
             // HCLK/8 for time base.
#define STK_STRE (1UL << 3)  // Auto-reload count
#define STK_SWIE (1UL << 31) // Software interrupt trigger

#define STK_CNTIF                                                              \
  (1UL << 0) // Count value comparison flag, write 0 to clear, write 1 to
             // invalidate.

/* -- FLASH -- */
#define FLASH_BASE 0x40022000
typedef struct {
  __IO uint32_t ACTLR;
} FLASH_TypeDef;
#define FLASH ((FLASH_TypeDef *)FLASH_BASE)
#define FLASH_LATENCY_2 0x2 /* 2 wait state */

/* ---- RCC ---- */
#define RCC_BASE 0x40021000
typedef struct {
  __IO uint32_t CTLR;      /* 0x00 */
  __IO uint32_t CFGR0;     /* 0x04 */
  __IO uint32_t INTR;      /* 0x08 */
  __IO uint32_t APB2PRSTR; /* 0x0C */
  __IO uint32_t APB1PRSTR; /* 0x10 */
  __IO uint32_t AHBPCENR;  /* 0x14 */
  __IO uint32_t APB2PCENR; /* 0x18 */
  __IO uint32_t APB1PCENR; /* 0x1C */
  __IO uint32_t RESERVED0; /* 0x20 */
  __IO uint32_t RSTSCKR;   /* 0x24 */
} RCC_Type_Def;

#define RCC ((RCC_Type_Def *)RCC_BASE)

#define RCC_PLLON (1UL << 24)
#define RCC_PLLRDY (1UL << 25)
#define RCC_SW_MASK 0x3UL
#define RCC_SW_PLL 0x2UL // SYSCLK = PLL
#define RCC_SWS_MASK (0x3UL << 2)
#define RCC_SWS_PLL (0x2UL << 2)

/* APB2PCENR */
#define RCC_AFIOEN (1UL << 0)
#define RCC_IOPAEN (1UL << 2)
#define RCC_IOPCEN (1UL << 4)
#define RCC_IOPDEN (1UL << 5)
#define RCC_USART1EN (1UL << 14)
/* ---- GPIO ----- */
#define GPIO_IN_FLOAT 0x4
#define GPIO_IN_PUPD 0x8
#define GPIO_OUT_PP_10 0x1
#define GPIO_AF_PP_50 0xB

#define GPIOA_BASE 0x40010800
#define GPIOC_BASE 0x40011000
#define GPIOD_BASE 0x40011400

typedef struct {
  __IO uint32_t CFGLR; /* 0x00 */
  __IO uint32_t CFGHR; /* 0x04 */
  __IO uint32_t INDR;  /* 0x08 */
  __IO uint32_t OUTDR; /* 0x0C */
  __IO uint32_t BSHR;  /* 0x10 */
  __IO uint32_t BCR;   /* 0x14 */
  __IO uint32_t LCKR;  /* 0x18 */
} GPIO_TypeDef;

#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)

/* ---- USART1 ---- */

#define USART1_BASE 0x40013800
typedef struct {
  __IO uint16_t STATR; /* 0x00: RXNE(5), TC(6), TXE(7), ORE(3) */
  __IO uint16_t RESERVED0;
  __IO uint16_t DATAR; /* 0x04 */
  __IO uint16_t RESERVED1;
  __IO uint16_t BRR; /* 0x08 fCLK/baud */
  __IO uint16_t RESERVED2;
  __IO uint16_t CTLR1; /* 0x0C: UE(13), M(12), TE(3), RE(2) */
  __IO uint16_t RESERVED3;
  __IO uint16_t CTLR2; /* 0x10 */
  __IO uint16_t RESERVED4;
  __IO uint16_t CTLR3; /* 0x14 */
  __IO uint16_t RESERVED5;
} USART_TypeDef;
#define USART1 ((USART_TypeDef *)USART1_BASE)

#define USART_TXE (1UL << 7)
#define USART_RXNE (1UL << 5)
#define USART_UE (1UL << 13)
#define USART_TE (1UL << 3)
#define USART_RE (1UL << 2)
#endif
