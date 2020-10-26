/**
 * @file RotatingIndex.h
 * @author Gustice
 * @brief Classes for typical index manipulation
 * @version 0.1
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020
 */

#include <stdint.h>

class RotatingIndex {
  public:
    const uint16_t MaxIndex;
    RotatingIndex(uint16_t max) : MaxIndex(max){ _index = 0; }

    uint16_t GetIndexAndInkrement(void) {
        auto retVal = _index;
        _index++;
        if (_index >= MaxIndex) {
            _index = 0;
        }
        return retVal;
    };

    bool PeekRevolution(void) {
        if (MaxIndex == 0)
            return true;

        if (_index >= MaxIndex-1) {
            return true;
        }
        return false;
    }

    uint16_t GetIndex(void) { return _index; }
    void ResetIndex(void) { _index = 0; }

  private:
    uint16_t _index;
};