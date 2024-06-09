#pragma once

/**
 * @brief Simple SBC control.
 */
class SBC
{
    bool OFF = false;

    const uint8_t m_pin;

public:
    /**
     * @brief Initialize hardware to control SBC.
     *
     * @param pin Pin to use.
     * @param activeLow Whether digital logic is inversed.
     */
    SBC(uint8_t pin, bool activeLow) : m_pin(pin)
    {
        if (activeLow)
        {
            OFF = true;
        }
    }

    void init()
    {
        pinMode(m_pin, OUTPUT);

        off();
    }

    void on()
    {
        digitalWrite(m_pin, !OFF);
    }

    void off()
    {
        digitalWrite(m_pin, OFF);
    }
};