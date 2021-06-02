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
#include <string.h>

#pragma once

/**
 * @brief Index with automatic revolution back to start position
 * @details Useful for Buffer-Pointer, or similar tasks
 */
class RotatingIndex {
  public:
    /// Value at witch the index rotates back to beginning
    const uint16_t MaxIndex;
    /**
     * @brief Constructor for rotating index
     * @brief output index will be set to 0 if incremented value reaches Maximum
     * @param max Maximum index for overflow-condition
     */
    RotatingIndex(uint16_t max) : MaxIndex(max) { index = 0; }

    /**
     * @brief Get the current index and inkrement index afterwards
     * @return uint16_t current index
     */
    uint16_t GetIndexAndInkrement(void) {
        auto ci = index;
        index++;
        if (index >= MaxIndex) {
            index = 0;
        }
        return ci;
    };

    /**
     * @brief Get the current index without incrementing the index
     * @return uint16_t current index
     */
    uint16_t GetIndex(void) { return index; }

    /**
     * @brief Check if next get will lead to overflow condition
     */
    bool PeekRevolution(void) {
        if (MaxIndex == 0)
            return true;

        if (index >= MaxIndex - 1) {
            return true;
        }
        return false;
    }

    /// Reset Index to 0
    void ResetIndex(void) { index = 0; }

  private:
    /// Current Index
    uint16_t index;
};