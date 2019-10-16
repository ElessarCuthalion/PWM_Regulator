/*
 * main.cpp
 *
 *  Created on: 22 мая 2017
 *      Author: Elessar
 */

#include "main.h"
#include "SimpleSensors.h"
#include "buttons.h"
#include "kl_adc.h"
#include "kl_i2c.h"
#include "SaveToFlash.h"

//#define SaveToFlash     TRUE

#if 1 // ======================== Variables and defines ========================
App_t App;
TmrKL_t TmrADC {MS2ST(ADC_MEAS_PERIOD_MS), EVT_SAMPLING, tktPeriodic};
PinOutputPWM_t PwmOut {PWM_PIN};


bool AdcFirstConv = true;
#ifdef SaveToFlash
     uint32_t Prof;
#endif

//enum AppState_t {
//    asInitRand, asLEDwork, asGoSleep, asHandheld,
//};
//AppState_t State = asInitRand;

//void BtnHandler(BtnEvt_t BtnEvt, uint8_t BtnID);
void BtnHandler(BtnEvt_t BtnEvt);

#endif

int main(void) {
    // ==== Init Clock system ====
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();
    App.InitThread();

    // ==== Init hardware ====
    Uart.Init(115200, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, BUILD_TIME);
    Clk.PrintFreqs();

    // Lock firmware
    if(!Flash::FirmwareIsLocked()) {
        Uart.Printf("Not locked, locking\r");
#ifdef SaveToFlash
        LEDs.SetProfile(DEF_LEDsProf);
        Prof = DEF_LEDsProf;
        Flash::Save(&Prof, sizeof(Prof));
#endif
        chThdSleepMilliseconds(150);
        Flash::LockFirmware();
    }
#ifdef SaveToFlash
    else {
        Flash::Load(&Prof, sizeof(Prof));
        LEDs.SetProfile((LEDsProfile_t) Prof);
    }
#endif

    // Buttons
    SimpleSensors::Init();

    // Adc
    PinSetupAnalog(ADC_USER_PIN);
    Adc.Init();
    Adc.EnableVRef();
    Adc.StartMeasurement();
    TmrADC.Init();
    TmrADC.StartOrRestart();

    // PWM
    PwmOut.Init();
    PwmOut.SetFrequencyHz(200);

    // Main cycle
    App.ITask();
}

__noreturn
void App_t::ITask() {
    while(true) {
        eventmask_t Evt = chEvtWaitAny(ALL_EVENTS);

        if(Evt & EVT_BUTTONS) {
            BtnEvtInfo_t EInfo;
//            while(BtnGetEvt(&EInfo) == retvOk) BtnHandler(EInfo.Type, EInfo.BtnID);
            while(BtnGetEvt(&EInfo) == retvOk) BtnHandler(EInfo.Type);
        }

#if ADC_REQUIRED
        if(Evt & EVT_SAMPLING) {
            Adc.StartMeasurement();
        }
        if(Evt & EVT_ADC_DONE) {
            if(AdcFirstConv) { AdcFirstConv = false; Adc.StartMeasurement(); }
            else {
                uint16_t Vrand_adc = Adc.GetResult(ADC_USER_CHNL, Filt);
                PwmOut.Set(Vrand_adc);
                Uart.Printf("ADC vaule: %u\r", Vrand_adc);
            } // if not big diff
        } // evt
#endif
#if UART_RX_ENABLED
        if(Evt & EVT_UART_NEW_CMD) {
            OnCmd((Shell_t*)&Uart);
            Uart.SignalCmdProcessed();
        }
#endif
    } // while true
} // App_t::ITask()


void BtnHandler(BtnEvt_t BtnEvt) {
    if(BtnEvt == beShortPress) Uart.Printf("Btn Short\r");
//    if(BtnEvt == beLongPress)  Uart.Printf("Btn Long\r");
//    if(BtnEvt == beRelease)    Uart.Printf("Btn Release\r");
//    if(BtnEvt == beClick)      Uart.Printf("Btn Click\r");

    if (BtnEvt == beLongPress) {
#ifdef SaveToFlash
        Prof = LEDs.GetProfile();
        Flash::Save(&Prof, sizeof(Prof));
#endif
    }
}


#if UART_RX_ENABLED // ================= Command processing ====================
void App_t::OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
	__attribute__((unused)) int32_t Data = 0, Data2 = 0;  // May be unused in some configurations
//    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }
    else if(PCmd->NameIs("Version")) {
        PShell->Printf("%S %S\r", APP_NAME, BUILD_TIME);
    }

    else PShell->Ack(retvCmdUnknown);
}
#endif
