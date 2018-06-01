# Standby / wake from RTC on STM32 - library for Mbed OS 5

For low-power applications you often want to bring your microcontroller into the deepest sleep mode possible, and then wake it up from an interrupt or from the RTC. For some reason I couldn't find any working code samples to do an RTC wakeup in Mbed OS 5. This library fixes this. Originally written by [Jammu Kekkonen](https://github.com/JammuKekkonen).

Note that going into standby mode on STM32 will clear all state. Your application will start from the beginning after waking up. If you don't want this, use sleep mode. This is supported natively by Mbed OS 5 - see the [Sleep Manager API](https://os.mbed.com/docs/v5.8/reference/sleep-manager.html).

## Usage

1. Add this library to your Mbed OS 5 project:

    ```
    $ mbed add https://github.com/janjongboom/stm32-standby-rtc-wakeup
    ```

1. Use this library:

    ```cpp
    #include "mbed.h"
    #include "stm32_standby.h"

    int main() {
        printf("Hello world, going to sleep for 5 seconds\n");

        standby(5);

        // after 5 seconds the application will run from the beginning again
    }
    ```
