#include "State.hpp"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

namespace
{
    bool g_shutdownReq = false;  // from MCU to sbc
    bool g_shutdownFlag = false; // SBC informed MCU about shutdown
    SemaphoreHandle_t g_paramSemaphore = xSemaphoreCreateMutex();
}

namespace State
{
    bool getShutdownRequest()
    {
        bool res = false;

        if (xSemaphoreTake(g_paramSemaphore, (TickType_t)5) == pdTRUE)
        {
            res = g_shutdownReq;
            xSemaphoreGive(g_paramSemaphore);
        }

        return res;
    }

    void setShutDownRequest(bool req)
    {
        if (xSemaphoreTake(g_paramSemaphore, (TickType_t)5) == pdTRUE)
        {
            g_shutdownReq = req;
            xSemaphoreGive(g_paramSemaphore);
        }
    }

    bool getShutDownFlag()
    {
        bool res = false;

        if (xSemaphoreTake(g_paramSemaphore, (TickType_t)5) == pdTRUE)
        {
            res = g_shutdownFlag;
            xSemaphoreGive(g_paramSemaphore);
        }

        return res;
    }

    void setShutDownFlag(bool flag)
    {
        if (xSemaphoreTake(g_paramSemaphore, (TickType_t)5) == pdTRUE)
        {
            g_shutdownFlag = flag;
            xSemaphoreGive(g_paramSemaphore);
        }
    }
}