/**
 * @file Encoder.h
 * @author Gustice
 * @brief Software Quadrature-Decoder class
 * @details Relies on two GPIO-Ports for synchronous or interrupt based decoding
 * @version 0.1
 * @date 2020-10-04
 * 
 * @copyright Copyright (c) 2020
 */
#pragma once

#include "GpioPort.h"
#include <stdint.h>

class QuadDecoder {
  public:
    enum PullResistor {
        floating = 0,
        PullUp,
        PullDown,
    };

    enum EncoderEvent {
        Idle,
        Increment,
        Decrement,
        LostStep,
    };

    typedef struct EncoderCallbacks_define {
        void (*EncEvent_CountUp)(void);
        void (*EncEvent_CountDown)(void);
        void (*EncEvent_Switch)(void);
    } EncoderEvents_t;

    QuadDecoder(InputPort &portA, InputPort &portB,
                EncoderEvents_t *AppCB = nullptr);
    ~QuadDecoder(){};

    // EncoderEvent GetEvent(void);
    // int32_t GetValue(void);
    int16_t EvalStepSync(void);

    int GetValue(void){ return _encCnt; }
    int GetErrors(void){ return _errorCnt; }

    bool InitSuccessful(void) { return _initOk; }

  private:
    const char *cModTag = "HAL-Decoder";
    bool _initOk = false;

    InputPort &_portA;
    InputPort &_portB;
    // InputPort &_portSw;

    int16_t _encCnt;
    int16_t _lastCnt;
    uint16_t _errorCnt;
    uint16_t _lastStep;

    EncoderEvents_t Enc_CB;
    int16_t GetLevelsSync(void);
};
