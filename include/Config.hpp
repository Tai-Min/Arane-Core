#pragma once

#define GAME_SELECTOR_ENCODER

namespace Config
{
    namespace AppTask
    {
        constexpr unsigned long BOOT_TIMEOUT_DURATION = 120000; // If boot time exceeds this time the app goes into error state.
        constexpr unsigned long SHUTDOWN_DURATION = 30000;      // How long to wait for shutdown to turn off SBC.
    }

    namespace MODBUS
    {
        namespace Coils // Coils (inputs to MCU)
        {
            constexpr uint8_t Q_SHUTDOWN_FLAG_ADDR = 0; // Info from SBC that it's shutting down.
        }

        namespace Inputs // Inputs (outputs from MCU)
        {
            constexpr uint8_t I_SHUTDOWN_REQ_ADDR = 0; // Request from MCU to shutdown SBC.
        }

        namespace HoldingRegs // Holding regs (inputs to MCU)
        {
            constexpr uint8_t AQ_SBC_HB_CNTR_ADDR = 0;
        }
        namespace InputRegs // Input regs (outputs from MCU)
        {
            constexpr uint8_t AI_MCU_HB_CNTR_ADDR = 0;
        }
    }
}