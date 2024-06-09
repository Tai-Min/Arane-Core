#pragma once

/**
 * @brief State machine that manages SBC.
*/
namespace Arane
{
    /**
     * @brief Main Arane task.
    */
    void task(void *pvParameters __attribute__((unused)));

    /**
     * @brief Get FreeRTOS stack size required to run this task.
    */
    constexpr uint16_t getRequiredStack()
    {
        return 200;
    }
}
