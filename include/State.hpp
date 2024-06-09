#pragma once

/**
 * @brief Global state, thread safe.
*/
namespace State
{
    // MCU request shutdown of the SBC
    bool getShutdownRequest();
    // MCU request shutdown of the SBC
    void setShutDownRequest(bool req);

    // SBC informed MCU about shutdown
    bool getShutDownFlag();
    // SBC informed MCU about shutdown
    void setShutDownFlag(bool flag);
}