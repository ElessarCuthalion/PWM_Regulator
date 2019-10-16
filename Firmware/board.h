/*
 * board.h
 *
 *  Created on: 01.02.2017
 *      Author: Kreyl
 */

#pragma once

#include <inttypes.h>

// ==== General ====
#define BOARD_NAME      "Pr v0 (Spruce_HW v1)"

// Default Settings
#define APP_NAME        "PWM_Regulator"

// Backup Registers

// MCU type as defined in the ST header.
#define STM32F030x4

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ     12000000

#define SYS_TIM_CLK         (Clk.APBFreqHz)

#define SIMPLESENSORS_ENABLED   TRUE
#define BUTTONS_ENABLED         TRUE
#define ADC_REQUIRED            TRUE
#define I2C_REQUIRED            FALSE

#if 1 // ========================== GPIO =======================================

// Button
#define BUTTONS_CNT     1
#define BTN_PIN         GPIOA, 4, pudPullUp

// Peripheral power enable
//#define PWR_EN_PIN      GPIOA, 5, omOpenDrain

// Antenna for Random generation
#define ADC_RAND_PIN    GPIOA, 1

// Sensors
#define USER_out        { GPIOB, 1, pudPullDown }

// PWM output
#define PWM_PIN         { GPIOA, 7, TIM3, 2, invNotInverted, omPushPull, 2048 }

// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     2
#define UART_RX_PIN     3
#define UART_AF         AF1 // for USART1 @ GPIOA

#endif // GPIO

#if 1 // ========================= Timer =======================================
#endif // Timer

#if I2C_REQUIRED // ====================== I2C =================================
#define I2C1_ENABLED    TRUE
#define I2C1_GPIO       GPIOA
#define I2C1_SCL        9
#define I2C1_SDA        10
// I2C Alternate Function
#define I2C_AF          AF4

#define I2C_USE_SEMAPHORE       FALSE
#endif

#if 1 // =========================== SPI =======================================

#endif

#if 1 // ========================== USART ======================================
//#define PRINTF_FLOAT_EN FALSE
#define UART            USART1
#define UART_TX_REG     UART->TDR
#define UART_RX_REG     UART->RDR
#endif

#if ADC_REQUIRED // ======================= Inner ADC ==========================
// Clock divider: clock is generated from the APB2
#define ADC_CLK_DIVIDER     adcDiv2

// ADC channels
#define ADC_USER_CHNL       9
#define ADC_USER_PIN        GPIOB, 1
#define ADC_RAND_CHNL       1
#define ADC_MEAS_PERIOD_MS  250

#define ADC_TEMP_CHNL       16
#define ADC_VREFINT_CHNL    17  // All 4xx, F072 and L151 devices. Do not change.
#define ADC_CHANNELS        { ADC_USER_CHNL, ADC_VREFINT_CHNL }
#define ADC_CHANNEL_CNT     2   // Do not use countof(AdcChannels) as preprocessor does not know what is countof => cannot check
#define ADC_SAMPLE_TIME     ast1d5Cycles
#define ADC_SAMPLE_CNT      5   // How many times to measure every channel

#define ADC_MAX_SEQ_LEN     16  // 1...16; Const, see ref man
#define ADC_SEQ_LEN         (ADC_SAMPLE_CNT * ADC_CHANNEL_CNT)
#if (ADC_SEQ_LEN > ADC_MAX_SEQ_LEN) || (ADC_SEQ_LEN == 0)
#error "Wrong ADC channel count and sample count"
#endif

#endif

#if 1 // =========================== DMA =======================================
//STM32F030 Reference Manual s.159,160
//STM32F072 Reference Manual s.184
//STM32L151 Reference Manual s.256,257
#define STM32_DMA_REQUIRED  TRUE
// ==== Uart ====
#define UART_DMA_TX     STM32_DMA1_STREAM4
#define UART_DMA_RX     STM32_DMA1_STREAM5
#define UART_DMA_CHNL   0   // Dummy

// ==== I2C1 ====
#if I2C_REQUIRED
#define I2C1_DMA_TX     STM32_DMA1_STREAM2
#define I2C1_DMA_RX     STM32_DMA1_STREAM3
#define I2C1_DMA_CHNL   0   // Dummy
#endif

// ==== SPI1 ====
//#define LEDWS_DMA       STM32_DMA1_STREAM3

#if ADC_REQUIRED
#define ADC_DMA         STM32_DMA1_STREAM1
#define ADC_DMA_MODE    STM32_DMA_CR_CHSEL(0) |   /* dummy */ \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_HWORD | \
                        STM32_DMA_CR_PSIZE_HWORD | \
                        STM32_DMA_CR_MINC |       /* Memory pointer increase */ \
                        STM32_DMA_CR_DIR_P2M |    /* Direction is peripheral to memory */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */
#endif // ADC

#endif // DMA
