#include "Arane.hpp"
#include "SBC.hpp"
#include "State.hpp"
#include "Log.hpp"
#include "Comm.hpp"
#include "Config.hpp"

namespace
{
    SBC g_sbc(Config::SBC::GPIO::RPI_PWR_CTRL,
              Config::SBC::GPIO::LOGIC_INVERTED);

    enum class AppState
    {
        OFF,
        BOOTING,
        CONNECTED,
        SHUTTING_DOWN,
        ERROR
    } g_state;

    unsigned long g_bootAndShutDownStamp;
}

namespace Weaks
{
    void onTaskInit() __attribute__((weak)) {}
    void onCommUpdate() __attribute__((weak)) {}
    bool doForceShutDown() __attribute__((weak)) { return false; }
    void afterStateHandle() __attribute__((weak)) {}

    void onOffStateSet() __attribute__((weak)) {}
    void onBootStateSet() __attribute__((weak)) {}
    void onConnectedStateSet() __attribute__((weak)) {}
    void onShutDownStateSet() __attribute__((weak)) {}
    void onErrorStateSet() __attribute__((weak)) {}

    void onForceShutDown() __attribute__((weak)) {}
    void onOffState() __attribute__((weak)) {}
    void onBootState() __attribute__((weak)) {}
    void onConnectedState() __attribute__((weak)) {}
    void onShutDownState() __attribute__((weak)) {}
    void onErrorState() __attribute__((weak)) {}

    bool isOffError() __attribute__((weak)) { return false; }
    bool isBootError() __attribute__((weak)) { return false; }
    bool isConnectedError() __attribute__((weak)) { return false; }
    bool isShutDownError() __attribute__((weak)) { return false; }

    bool shouldBoot() __attribute__((weak)) { return false; }
    bool shouldShutDown() __attribute__((weak)) { return false; }
    bool clearErrorState() __attribute__((weak)) { return false; }
}

namespace Arane
{
    void task(void *pvParameters __attribute__((unused)));

    void setAppState(AppState s);

    void handleForceShutDown();
    void handleOffState();
    void handleBootState();
    void handleConnectedState();
    void handleShutDownState();
    void handleErrorState();

    void logHighwater();

    void task(void *pvParameters __attribute__((unused)))
    {
        bool forceShutDown = false;

        LOG_INFO("Arane start");

        g_sbc.init();
        Weaks::onTaskInit();
        Comm::init();

        setAppState(AppState::OFF);
        while (true)
        {
            Comm::update();

            Weaks::onCommUpdate();

            bool forceShutDownFlag = Weaks::doForceShutDown();
            if (forceShutDownFlag && !forceShutDown)
            {
                forceShutDown = true;
                handleForceShutDown();
            }
            else if (!forceShutDownFlag && forceShutDown)
            {
                forceShutDown = false;
            }

            switch (g_state)
            {
            case AppState::OFF:
                handleOffState();
                break;
            case AppState::BOOTING:
                handleBootState();
                break;
            case AppState::CONNECTED:
                handleConnectedState();
                break;
            case AppState::SHUTTING_DOWN:
                handleShutDownState();
                break;
            case AppState::ERROR:
                handleErrorState();
                break;
            }

            afterStateHandle();

            logHighwater();

            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }

    void setAppState(AppState s)
    {
        g_state = s;
        switch (s)
        {
        case AppState::OFF:
            LOG_INFO("Moving into OFF state");

            Weaks::onOffStateSet();

            // Just in case that SBC manages
            // to write this flag again on shut down
            State::setShutDownFlag(false);
            State::setShutDownRequest(false);

            g_sbc.off();

            break;
        case AppState::BOOTING:
            LOG_INFO("Moving into BOOTING state");

            Weaks::onBootStateSet();

            g_bootAndShutDownStamp = millis();
            break;
        case AppState::CONNECTED:
            LOG_INFO("Moving into CONNECTED state");

            Weaks::onConnectedStateSet();

            g_ledCtrl.setEffect(LightEffectors::Effect::MANUAL);
            break;
        case AppState::SHUTTING_DOWN:
            LOG_INFO("Moving into SHUTTING_DOWN state");

            Weaks::onShutDownStateSet();

            State::setShutDownFlag(false);
            State::setShutDownRequest(true);
            g_bootAndShutDownStamp = millis();
            break;
        case AppState::ERROR:
            LOG_INFO("Moving into ERROR state");

            Weaks::onErrorStateSet();

            State::setShutDownFlag(false);
            State::setShutDownRequest(false);
            break;
        }
    }

    void handleForceShutDown()
    {
        Weaks::onForceShutDown();

        setAppState(AppState::OFF);

        LOG_INFO(F("Force shut down"));
    }

    void handleOffState()
    {
        Weaks::onOffState();

        if (Weaks::isOffError())
        {
            setAppState(AppState::ERROR);
            LOG_ERROR(F("isOffError error"));
        }

        if (Weaks::shouldBoot())
        {
            setAppState(AppState::BOOTING);

            LOG_INFO(F("External boot"));
        }
    }

    void handleBootState()
    {
        Weaks::onBootState();

        if (Weaks::isBootError())
        {
            setAppState(AppState::ERROR);
            LOG_ERROR(F("isBootError error"));
        }

        else if (millis() - g_bootAndShutDownStamp >= Config::AppTask::BOOT_TIMEOUT_DURATION)
        {
            setAppState(AppState::ERROR);
            LOG_ERROR(F("Boot timeout error"));
        }

        else if (Comm::connected())
        {
            setAppState(AppState::CONNECTED);

            LOG_INFO(F("Boot ok"));
        }
    }

    void handleConnectedState()
    {
        Weaks::onConnectedState();

        if (Weaks::isConnectedError())
        {
            setAppState(AppState::ERROR);

            LOG_ERROR(F("isConnectedError error"));
        }

        else if (Weaks::shouldShutDown())
        {
            setAppState(AppState::SHUTTING_DOWN);

            LOG_INFO(F("External shut down"));
        }

        else if (!Comm::connected())
        {
            setAppState(AppState::ERROR);

            LOG_ERROR(F("Disconnected error"));
        }

        else if (State::getShutDownFlag())
        {
            setAppState(AppState::SHUTTING_DOWN);

            LOG_INFO(F("SBC shut down"));
        }
    }

    void handleShutDownState()
    {
        Weaks::onShutDownState();

        if (Weaks::isShutDownError())
        {
            setAppState(AppState::ERROR);
            LOG_ERROR(F("isShutDownError err"));
        }
        
        else if (millis() - g_bootAndShutDownStamp >= Config::AppTask::SHUTDOWN_DURATION)
        {
            setAppState(AppState::OFF);
            LOG_INFO(F("Shut down ok"));
        }
    }

    void handleErrorState()
    {
        Weaks::onErrorState();

        if (Weaks::clearErrorState())
        {
            setAppState(AppState::OFF);

            LOG_INFO(F("External error state cleared"));
        }
    }

    void logHighwater()
    {
        static unsigned long stamp = millis();
        constexpr unsigned long period = 5000;

        if (millis() - stamp >= period)
        {
            uint16_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            // LOG_INFO("app: ", uxHighWaterMark);
            stamp = millis();
        }
    }
}
