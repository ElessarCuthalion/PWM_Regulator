/*
 * SaveToFlash.h
 *
 *  Created on: 5 ����. 2017 �.
 *      Author: Kreyl
 */

#pragma once

#include "inttypes.h"

/*
 * Linker script (i.e. rules.ld) must contain section aligned to some page address.
 * This section may be placed in the end of sections list (bottom of file).
 * For example:
    MyFlash 0x08005000 :
    {
        *(MyFlash)
    } > flash
 */

#define FLASH_SAVE_PLACES   1

#define FLASH_PAGE_SIZE     32

namespace Flash {

void Load(uint32_t *ptr, uint32_t ByteSz);
uint8_t Save(uint32_t *ptr, uint32_t ByteSz);

#if FLASH_SAVE_PLACES > 1
void Load2(uint32_t *ptr, uint32_t ByteSz);
uint8_t Save2(uint32_t *ptr, uint32_t ByteSz);
#endif

}
