# Arane Core 

Arane Core is a simple SBC power management software designed for MCUs supporting Arduino framework.

Code of Arane Core was written with customization in mind. While in it's purest form it allows for simple power management (on, off and reset via buttons and a status LED), it can be extended with FreeRTOS' tasks and by customizing multiple weak linked handlers embedded into Arane's main task to alter it's internal state and create user defined control methods. For example user can attach a WiFi module and override shouldBoot() function to power SBC via WiFi, or override onConnectedStateSet() function to power on USB switch.

Additionaly, one can attach custom sensors and actuators to the MCU and read / control them from SBC.

For getting started see [TODO](TODO).
