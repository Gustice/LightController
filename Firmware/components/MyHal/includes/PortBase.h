/**
 * @file PortBase.h
 * @author Gustice
 * @brief Port-Baseclass
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 */

#pragma once

#include <stdint.h>
#include <string.h>

/**
 * @brief Port-Baseclass
 * @details Basic diagnosis functions for port classes
 */
class PortBase {
  public:
    const char *cModTag;
    bool GetInitOkState() { return InitOk; }

  protected:
    PortBase(const char *tag) : cModTag(tag) {}
    ~PortBase(){};

    bool InitOk = false;
};