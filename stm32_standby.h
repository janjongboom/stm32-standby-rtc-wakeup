#ifndef _STM32_STANDBY_RTC_WAKEUP_H_
#define _STM32_STANDBY_RTC_WAKEUP_H_

#include "mbed.h"
#include "mbed_debug.h"     // Use debug() instead of printf() so we don't include UART drivers in release builds
#include "rtc_api_hal.h"

enum WakeupType {
    WAKEUP_RESET,
    WAKEUP_TIMER,
    WAKEUP_PIN
};

static RTC_HandleTypeDef RtcHandle;

static void rtc_set_wake_up_timer_s(uint32_t delta)
{
    uint32_t clock = RTC_WAKEUPCLOCK_CK_SPRE_16BITS;

    // HAL_RTCEx_SetWakeUpTimer_IT will assert that delta is 0xFFFF at max
    if (delta > 0xFFFF) {
        delta -= 0x10000;
        clock = RTC_WAKEUPCLOCK_CK_SPRE_17BITS;
    }

    RtcHandle.Instance = RTC;

    HAL_StatusTypeDef status = HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, delta, clock);

    if (status != HAL_OK) {
        debug("Set wake up timer failed: %d\n", status);
        NVIC_SystemReset();
     }
}

static WakeupType get_wakeup_type() {
    if(READ_BIT(RTC->ISR, RTC_ISR_WUTF))
        return WAKEUP_TIMER;

    // this is set by timer too, but that's checked already
    // above.
    if(READ_BIT(PWR->CSR, PWR_CSR_WUF))
        return WAKEUP_PIN;

    return WAKEUP_RESET;
}

void standby(int seconds) {
    core_util_critical_section_enter();

    // Clear wakeup flag, just in case.
    SET_BIT(PWR->CR, PWR_CR_CWUF);

    // Enable wakeup timer.
    rtc_set_wake_up_timer_s(seconds);

    // Enable debug interface working in standby. Causes power consumption to increase drastically while in standby.
    //HAL_DBGMCU_EnableDBGStandbyMode();

    HAL_PWR_EnterSTANDBYMode();

    // this should not happen...
    rtc_deactivate_wake_up_timer();
    core_util_critical_section_exit();

    debug("Continued after getting out of STANDBY mode - this should not happen\n");

    // something went wrong, let's reset
    NVIC_SystemReset();
}

#endif // _STM32_STANDBY_RTC_WAKEUP_H_
