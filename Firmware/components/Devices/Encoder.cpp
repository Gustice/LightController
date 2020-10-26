/**
 * @file Encoder.cpp
 * @author Gustice
 * @brief Encoder-class implementation
 * @version 0.1
 * @date 2020-10-11
 * 
 * @copyright Copyright (c) 2020
 */

#include "Encoder.h"
#include "esp_log.h"

QuadDecoder::QuadDecoder(InputPort &portA, InputPort &portB,
                         QuadDecoder::EncoderEvents_t *AppCB)
    : _portA(portA), _portB(portB) {
    if (AppCB != nullptr) {
        Enc_CB.EncEvent_CountUp = AppCB->EncEvent_CountUp;
        Enc_CB.EncEvent_CountDown = AppCB->EncEvent_CountDown;
    }

    _lastStep = GetLevelsSync();
    _encCnt = 0;
    _errorCnt = 0;
}

int16_t QuadDecoder::GetLevelsSync(void) {
    uint16_t value = (_portA.ReadPort() & 0x01);
    value |= ((_portB.ReadPort() << 1) & 0x02);
    return value;
}

int16_t QuadDecoder::EvalStepSync(void) {
    uint16_t levels = GetLevelsSync();
    // combines last step and this to bitpattern: xxxxLLTT
    uint16_t thisStep = ((_lastStep << 2) | levels) & 0x0F;
    _lastStep = levels;

    switch (thisStep) { //	    xxxx LL TT
    case 0x0:           // 00 00
    case 0x5:           // 01 01
    case 0xA:           // 10 10
    case 0xF:           // 11 11
        // No Changes
        break;

    case 0x1: // 00 01
    case 0x7: // 01 11
    case 0xE: // 11 10
    case 0x8: // 10 00
    {
        _encCnt++;
        // where are the callbacks
    } break;

    case 0x2: // 00 10
    case 0xB: // 10 11
    case 0xD: // 11 01
    case 0x4: // 01 00
    {
        _encCnt--;
    } break;

    case 0x3: // 00 11
    case 0x6: // 01 10
    case 0x9: // 10 01
    case 0xC: // 11 00
        _errorCnt++;
        break;

    default:
        break;
    }

    return _encCnt;
}
// uint8_t ENC_Value(void)
// {
// 	uint8_t tempCnt = u8_EncCnt;

// 	// Ceil on .5 or higher
// 	switch (tempCnt & 0x0003)
// 	{
// 		// Ceil value in case of midstates
// 		case 0x3:
// 		case 0x2:
// 		tempCnt += 0x0004;
// 		break;

// 		default:
// 		break;
// 	}

// 	tempCnt = tempCnt >> 2;
// 	return tempCnt;
// }

// e_EncEvent ENC_GetEvent(void)
// {
// 	e_EncEvent returnEvent = Enc_Idle;

// 	uint8_t cnt = ENC_Value();

// 	if (cnt == (u8_LastCnt +1) )
// 		returnEvent = Enc_Increment;
// 	if (cnt == (u8_LastCnt +2) )
// 		returnEvent = Enc_Increment;

// 	if (cnt == (u8_LastCnt -1) )
// 		returnEvent = Enc_Decrement;
// 	if (cnt == (u8_LastCnt -2) )
// 		returnEvent = Enc_Decrement;

// 	u8_LastCnt = cnt;

// 	return returnEvent;
// }
