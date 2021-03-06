/*
 * SaveToFlash.cpp
 *
 *  Created on: 5 ����. 2017 �.
 *      Author: Kreyl
 */

#include "SaveToFlash.h"
#include <cstring>  // For memcpy
#include "kl_lib.h"
#include "uart.h"


// Data inside the Flash. Init value is dummy.
// For some reason "aligned" attribute does not work, therefore array used here.
__attribute__ ((section("MyFlash")))
const uint32_t IData[(FLASH_PAGE_SIZE/4)] = { 0xCA115EA1 };

#if FLASH_SAVE_PLACES > 1
__attribute__ ((section("MyFlash2")))
const uint32_t IData2[(FLASH_PAGE_SIZE/4)] = { 0xCA115EA1 };
#endif

static inline void Unlock() {
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}
static inline void Lock(void) { FLASH->CR |= FLASH_CR_LOCK_Set; }
static inline void ClearFlag(uint32_t FlashFlag) { FLASH->SR = FlashFlag; }

static uint8_t GetBank1Status(void) {
    if(FLASH->SR & FLASH_SR_BSY) return retvBusy;
    else if(FLASH->SR & FLASH_SR_PGERR) return retvFail;
    else if(FLASH->SR & FLASH_SR_WRPRTERR) return retvFail;
    else return retvOk;
}

static uint8_t WaitForLastOperation(uint32_t Timeout) {
    uint8_t status = retvOk;
    // Wait for a Flash operation to complete or a TIMEOUT to occur
    do {
        status = GetBank1Status();
        Timeout--;
    } while((status == retvBusy) and (Timeout != 0x00));
    if(Timeout == 0x00) status = retvTimeout;
    return status;
}

static uint8_t ErasePage(uint32_t PageAddress) {
    uint8_t status = WaitForLastOperation(FLASH_EraseTimeout);
    if(status == retvOk) {
        FLASH->CR |= FLASH_CR_PER_Set;
        FLASH->AR = PageAddress;
        FLASH->CR |= FLASH_CR_STRT_Set;
        // Wait for last operation to be completed
        status = WaitForLastOperation(FLASH_EraseTimeout);
        // Disable the PER Bit
        FLASH->CR &= FLASH_CR_PER_Reset;
    }
    return status;
}

static uint8_t ProgramWord(uint32_t Address, uint32_t Data) {
    uint8_t status = WaitForLastOperation(FLASH_ProgramTimeout);
    if(status == retvOk) {
        FLASH->CR |= FLASH_CR_PG_Set; // program the new first half word
        *(__IO uint16_t*)Address = (uint16_t)Data;
        /* Wait for last operation to be completed */
        status = WaitForLastOperation(FLASH_ProgramTimeout);
        if(status == retvOk) {
            // program the new second half word
            uint32_t tmp = Address + 2;
            *(__IO uint16_t*)tmp = Data >> 16;
            /* Wait for last operation to be completed */
            status = WaitForLastOperation(FLASH_ProgramTimeout);
            /* Disable the PG Bit */
            FLASH->CR &= FLASH_CR_PG_Reset;
        }
        else FLASH->CR &= FLASH_CR_PG_Reset;  // Disable the PG Bit
    }
    return status;
}

static uint8_t SaveCommon(uint32_t *ptr, uint32_t ByteSz, uint32_t Addr) {
    uint8_t status = retvOk;
//    Uart.PrintfI("F addr: %08X\r", FAddr);
    uint32_t DataWordCount = (ByteSz + 3) / 4;
    chSysLock();
    Unlock();
    // Erase flash
    ClearFlag(FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPRTERR);   // Clear all pending flags
    status = ErasePage(Addr);
    //Uart.Printf("  Flash erase %u: %u\r", i, FLASHStatus);
    if(status != retvOk) {
        Uart.PrintfI("Flash erase error\r");
        goto end;
    }
    // Program flash
    for(uint32_t i=0; i<DataWordCount; i++) {
        status = ProgramWord(Addr, *ptr);
        if(status != retvOk) {
            Uart.PrintfI("Flash write error\r");
            goto end;
        }
        Addr += 4;
        ptr++;
    }
    Lock();
    end:
    chSysUnlock();
    return status;
}

namespace Flash {

void Load(uint32_t *ptr, uint32_t ByteSz) {
    memcpy(ptr, IData, ByteSz);
}
uint8_t Save(uint32_t *ptr, uint32_t ByteSz) {
    return SaveCommon(ptr, ByteSz, (uint32_t)&IData[0]);
}

#if FLASH_SAVE_PLACES > 1
void Load2(uint32_t *ptr, uint32_t ByteSz) {
    memcpy(ptr, IData2, ByteSz);
}

uint8_t Save2(uint32_t *ptr, uint32_t ByteSz) {
    return SaveCommon(ptr, ByteSz, (uint32_t)&IData2[0]);
}
#endif

} // namespace
