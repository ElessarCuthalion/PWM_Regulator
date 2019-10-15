/*
 * evt_mask.h
 *
 *  Created on: 4 февр. 2017
 *      Author: Kreyl
 */

#pragma once

// ==== Event masks ====
// Eternal
#define EVT_UART_NEW_CMD        EVENT_MASK(1)

// Periphery and may-be's
#define EVT_BUTTONS             EVENT_MASK(3)
// Adc
#define EVT_SAMPLING            EVENT_MASK(5)
#define EVT_ADC_DONE            EVENT_MASK(6)


