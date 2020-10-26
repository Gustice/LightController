/**
 * @file GpioPort.h
 * @author Gustice
 * @brief GPIO-Port classes
 * @details Reading and Setting ports
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 */

#pragma once

#include "PortBase.h"
#include "driver/gpio.h"

/**
 * @brief Digital-Port Baseclass
 */
class GpioPort : public PortBase {
  public:
    enum PullResistor {
        floating = 0,
        PullUp,
        PullDown,
    };

    enum OutputLogic {
        Normal = 0, // Normal logic
        Inverted,   // Inverted logic -> Set low on active state
    };

    enum Interrupt {
        PositiveEdge = GPIO_INTR_POSEDGE,
        NegativeEdge = GPIO_INTR_NEGEDGE,
        AnyEdge = GPIO_INTR_ANYEDGE
    };

    GpioPort(gpio_num_t port);
    ~GpioPort();

    uint64_t GetReserved(void) { return _Reserved; }

  protected:
    gpio_num_t _port;

  private:
    /* data */
    static uint64_t _Reserved;
};

/**
 * @brief Input-Port
 * @details Provides abstract reading functionality
 */
class InputPort : public GpioPort {
  public:
    InputPort(gpio_num_t port, GpioPort::PullResistor resistor = PullResistor::floating);
    ~InputPort(){};

    int ReadPort(void);

  protected:
   gpio_config_t _config;
};

/**
 * @brief Output-Port
 * @details Provies abstract setting functionality
 */
class OutputPort : public GpioPort {
  public:
    OutputPort(gpio_num_t port, GpioPort::OutputLogic mode = OutputLogic::Normal);
    ~OutputPort(){};

    void WritePort(int active);

  private:
    GpioPort::OutputLogic _mode = Normal;
};

class TriggeredInput : public InputPort {
  public:
    TriggeredInput(gpio_num_t port,
                   GpioPort::PullResistor resistor = GpioPort::PullResistor::floating,
                   TriggeredInput::Interrupt edge = TriggeredInput::Interrupt::AnyEdge);
    ~TriggeredInput(){};

    void RegisterIsr(gpio_isr_t pIsrHandle);

  private:
    bool _IsrServiceAvailable;
    static bool _isrServiceInitiated;
};
