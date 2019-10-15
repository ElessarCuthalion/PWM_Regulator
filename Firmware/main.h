/*
 * main.h
 *
 *  Created on: 22 мая 2017
 *      Author: Elessar
 */

#pragma once

#include "ch.h"
#include "kl_lib.h"
#include "uart.h"
#include "evt_mask.h"
#include "board.h"

class App_t {
private:
    thread_t *PThread;
public:
    // Eternal
    void InitThread() { PThread = chThdGetSelfX(); }
    void SignalEvt(eventmask_t Evt) {
        chSysLock();
        chEvtSignalI(PThread, Evt);
        chSysUnlock();
    }
    void SignalEvtI(eventmask_t Evt) { chEvtSignalI(PThread, Evt); }
#if UART_RX_ENABLED
    void OnCmd(Shell_t *PShell);
#endif
    // Inner use
    void ITask();
};

extern App_t App;

#if 0
class Periphy_t {
private:

public:
    void Init(){
        PinSetupOut(DC_DC_PIN, DC_DC_PinMode);
        PinSetupOut(UserEN_PIN, UserEN_PinMode);
    }
    void DC_DC_On(){ PinSetLo(DC_DC_PIN); }
    void DC_DC_Off(){ PinSetHi(DC_DC_PIN); }
    void UserEN_On(){ PinSetLo(UserEN_PIN); }
    void UserEN_Off(){ PinSetHi(UserEN_PIN); }
};
extern Periphy_t Periphy;
#endif
