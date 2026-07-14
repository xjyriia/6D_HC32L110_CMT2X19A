#include "user_low_power.h"
#include "user_lpt.h"
#include "user_motor.h"
#include "board.h"
#include "lpm.h"
#include "lpt.h"
#include "gpio.h"
#include "clk.h"
#include "pca.h"
#include "ddl.h"

/* Low power config */
#define LOW_POWER_TIMEOUT_10S   10000u
#define LPTIMER_CLOCK_HZ        32768u

/* State */
static uint8_t  u8LowPowerEnabled = 0;
static uint32_t u32TimeoutMs = 0;
static uint32_t u32CurrentCountMs = 0;
static func_ptr_t pfnTimeoutCallback = NULL;

/* ===== LPTimer IRQ callback ===== */
static void Lptimer_IrqCallback(void)
{
    if (Lpt_GetIntFlag()) {
        Lpt_ClearIntFlag();
        u32CurrentCountMs += 100;
        if (u32CurrentCountMs >= u32TimeoutMs) {
            if (pfnTimeoutCallback != NULL) {
                pfnTimeoutCallback();
            }
            Lpt_Stop();
            u8LowPowerEnabled = 0;
        }
    }
}

/* ===== Init LPTimer for wake-up ===== */
static void LowPower_Timer_Init(uint32_t timeout_ms)
{
    stc_lpt_config_t stcLptConfig;
    uint16_t u16ReloadValue;

    DDL_ZERO_STRUCT(stcLptConfig);
    Clk_SetPeripheralGate(ClkPeripheralLpTim, TRUE);

    stcLptConfig.enCT     = LptTimer;
    stcLptConfig.enMD     = LptMode2;
    stcLptConfig.enTckSel = LptXIL32K;
    stcLptConfig.enGate   = LptGateDisable;
    stcLptConfig.enGateP  = LptOpposite;
    stcLptConfig.enTog    = LptTogDisable;
    stcLptConfig.pfnLpTimCb = Lptimer_IrqCallback;

    Lpt_Init(&stcLptConfig);

    u16ReloadValue = LPTIMER_CLOCK_HZ / 10;   /* ~100ms per tick */
    Lpt_ARRSet(u16ReloadValue);

    Lpt_EnableIrq();
    EnableNvic(LPTIM_IRQn, DDL_IRQ_LEVEL_DEFAULT, TRUE);

    u32TimeoutMs = timeout_ms;
    u32CurrentCountMs = 0;
}

/* ===== Deep-sleep GPIO: ALL pins to analog except wake-up =====
 *
 * CRITICAL: Gpio_SetAnalog() only sets ADS=1 (disables digital input buffer).
 * It does NOT disable pull-up/pull-down resistors!  If i2c_init() or any other
 * init enabled internal pull-ups (~47KΩ), they still draw 3.3V/47K ≈ 70µA each
 * even in analog mode.  We MUST explicitly clear PU/PD for pins going to analog.
 */
static void LowPower_GpioConfig(void)
{
    /*
     * HC32L110 TSSOP20 pins:
     *   P00(SWCLK) P01(SWDIO) P02 P03
     *   P14 P15  P23 P24 P25 P26 P27
     *   P31 P32 P33 P34 P35 P36
     */
    Gpio_SetAnalog(0u, 2u, TRUE);   /* P02 */
    M0P_GPIO->P0PU_f.P02 = 0u; M0P_GPIO->P0PD_f.P02 = 0u;
#if defined(BOARD_TYPE_TX)
    /* P03: wake-up for TX, handled in caller */
#else
    Gpio_SetAnalog(0u, 3u, TRUE);   /* P03 (unused on 6D/RX) */
    M0P_GPIO->P0PU_f.P03 = 0u; M0P_GPIO->P0PD_f.P03 = 0u;
#endif

    Gpio_SetAnalog(1u, 4u, TRUE);   /* P14 (TWI_SDA) */
    M0P_GPIO->P1PU_f.P14 = 0u; M0P_GPIO->P1PD_f.P14 = 0u;
    Gpio_SetAnalog(1u, 5u, TRUE);   /* P15 (TWI_SCK) */
    M0P_GPIO->P1PU_f.P15 = 0u; M0P_GPIO->P1PD_f.P15 = 0u;

    Gpio_SetAnalog(2u, 3u, TRUE);   /* P23 (vibrator/RX) */
    M0P_GPIO->P2PU_f.P23 = 0u; M0P_GPIO->P2PD_f.P23 = 0u;
    Gpio_SetAnalog(2u, 4u, TRUE);   /* P24 (MOTOR_A/6D) */
    M0P_GPIO->P2PU_f.P24 = 0u; M0P_GPIO->P2PD_f.P24 = 0u;
    Gpio_SetAnalog(2u, 5u, TRUE);   /* P25 (MOTOR_B/6D) */
    M0P_GPIO->P2PU_f.P25 = 0u; M0P_GPIO->P2PD_f.P25 = 0u;
    Gpio_SetAnalog(2u, 6u, TRUE);   /* P26 */
    M0P_GPIO->P2PU_f.P26 = 0u; M0P_GPIO->P2PD_f.P26 = 0u;
    Gpio_SetAnalog(2u, 7u, TRUE);   /* P27 */
    M0P_GPIO->P2PU_f.P27 = 0u; M0P_GPIO->P2PD_f.P27 = 0u;

    Gpio_SetAnalog(3u, 1u, TRUE);   /* P31 */
    M0P_GPIO->P3PU_f.P31 = 0u; M0P_GPIO->P3PD_f.P31 = 0u;
#if defined(BOARD_TYPE_RX)
    /* P32: CMT_GPO1 wake-up for RX, handled in caller */
#else
    Gpio_SetAnalog(3u, 2u, TRUE);   /* P32 (CMT_GPO1, unused on 6D/TX) */
    M0P_GPIO->P3PU_f.P32 = 0u; M0P_GPIO->P3PD_f.P32 = 0u;
#endif
    /* P33 = I2C_SDA — i2c_init() enables pull-up! Must clear or wastes ~70µA */
    Gpio_SetAnalog(3u, 3u, TRUE);
    M0P_GPIO->P3PU_f.P33 = 0u; M0P_GPIO->P3PD_f.P33 = 0u;
    /* P34 = I2C_SCL — i2c_init() enables pull-up! Must clear or wastes ~70µA */
    Gpio_SetAnalog(3u, 4u, TRUE);
    M0P_GPIO->P3PU_f.P34 = 0u; M0P_GPIO->P3PD_f.P34 = 0u;
#if !defined(BOARD_TYPE_6D)
    Gpio_SetAnalog(3u, 5u, TRUE);   /* P35 (SPI_SDIO) */
    M0P_GPIO->P3PU_f.P35 = 0u; M0P_GPIO->P3PD_f.P35 = 0u;
#endif
    Gpio_SetAnalog(3u, 6u, TRUE);   /* P36 (SPI_CSB) */
    M0P_GPIO->P3PU_f.P36 = 0u; M0P_GPIO->P3PD_f.P36 = 0u;
}

/* ===== Public API ===== */

void user_low_power_init(void)
{
    u8LowPowerEnabled = 0;
    u32TimeoutMs = 0;
    u32CurrentCountMs = 0;
    pfnTimeoutCallback = NULL;
}

void user_low_power_start_timer(uint32_t timeout_ms, func_ptr_t callback)
{
    pfnTimeoutCallback = callback;
    u32TimeoutMs = timeout_ms;
    u32CurrentCountMs = 0;
    u8LowPowerEnabled = 1;
    LowPower_Timer_Init(timeout_ms);
    Lpt_Run();
}

void user_low_power_stop_timer(void)
{
    Lpt_Stop();
    Lpt_DisableIrq();
    u8LowPowerEnabled = 0;
}

/*
 * Sleep mode (SLEEPDEEP=0): CPU stops, clocks keep running.
 * Used by RX board between radio polls.
 */
void user_enter_sleep_mode(void)
{
    stc_lpm_config_t stcLpmConfig;

#if defined(BOARD_TYPE_RX)
    {
        static uint8_t s_u8LptInit = 0;
        if (!s_u8LptInit) {
            LowPower_Timer_Init(80000);
            s_u8LptInit = 1;
        }
        Lpt_Run();
    }
#endif

    stcLpmConfig.enSEVONPEND  = FALSE;
    stcLpmConfig.enSLEEPDEEP  = FALSE;
    stcLpmConfig.enSLEEPONEXIT = FALSE;
    Lpm_Config(&stcLpmConfig);
    Lpm_GotoLpmMode();
}

/*
 * Deep sleep (SLEEPDEEP=1): CPU + all clocks stop. Target: 0.5uA.
 *
 * Key optimizations:
 *   1. Gpio_SetAnalog() on ALL unused pins - disables digital input buffer
 *   2. SysTick->CTRL = 0 - Cortex-M0+ SysTick leaks in deep sleep
 *   3. M0P_CLOCK->PERI_CLKEN = 0 - one-shot disable all peripherals
 *   4. Only wake-up pin kept as digital input with interrupt
 */
void user_enter_deep_sleep_mode(void)
{
    stc_lpm_config_t stcLpmConfig;

    /* --- Board-specific wake-up source --- */
#if defined(BOARD_TYPE_RX)
    {
        static uint8_t s_u8LptInit = 0;
        if (!s_u8LptInit) {
            LowPower_Timer_Init(50000);
            s_u8LptInit = 1;
        }
        Lpt_Run();
    }
    /* CMT_GPO1 (P3.2): digital input + pull-up + falling edge */
    Gpio_InitIOExt(3u, 2u, GpioDirIn, TRUE, FALSE, FALSE, FALSE);
    Gpio_ClearIrq(3u, 2u);
    Gpio_EnableIrq(3u, 2u, GpioIrqFalling);
    EnableNvic(PORT3_IRQn, DDL_IRQ_LEVEL_DEFAULT, TRUE);

#elif defined(BOARD_TYPE_6D)
    Motor_Stop();
    Motor_Sleep();
    /* LIS2DH INT (P3.5): digital input, NO pull-up (LIS2DH push-pull output, pull-up wastes ~80µA) */
    Gpio_InitIOExt(3u, 5u, GpioDirIn, FALSE, FALSE, FALSE, FALSE);
    Gpio_ClearIrq(3u, 5u);
    Gpio_EnableIrq(3u, 5u, GpioIrqRising);
    EnableNvic(PORT3_IRQn, DDL_IRQ_LEVEL_DEFAULT, TRUE);

#else
    /* TX: Key (P0.3): digital input + pull-up + falling edge */
    Gpio_InitIOExt(0u, 3u, GpioDirIn, TRUE, FALSE, FALSE, FALSE);
    Gpio_ClearIrq(0u, 3u);
    Gpio_EnableIrq(0u, 3u, GpioIrqFalling);
    EnableNvic(PORT0_IRQn, DDL_IRQ_LEVEL_DEFAULT, TRUE);
#endif

    /* --- 1. Stop SysTick --- */
    SysTick->CTRL = 0u;

    /* --- 2. Stop PCA watchdog --- */
    Pca_Stop();

    /* --- 3. Disable BGR and LVD (save ~15-30µA) --- */
    M0P_BGR->CR_f.BGR_EN = 0u;
    M0P_LVD->CR_f.LVDEN = 0u;

    /* --- 4. Disable RCL for 6D/TX (wake by GPIO, not LPT) --- */
#if !defined(BOARD_TYPE_RX)
    Clk_Enable(ClkRCL, FALSE);
#endif

    /* --- 5. Disable ALL peripheral clocks, keep FLASH + GPIO + wakeup source --- */
    M0P_CLOCK->PERI_CLKEN = 0u;
    Clk_SetPeripheralGate(ClkPeripheralFlash, TRUE);  /* MUST keep: CPU fetches code from flash after wake-up */
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
#if defined(BOARD_TYPE_RX)
    Clk_SetPeripheralGate(ClkPeripheralLpTim, TRUE);
#endif

    /* --- 6. All GPIOs to analog (except wake-up pin configured above) --- */
    LowPower_GpioConfig();

    /* --- 7. Configure deep sleep --- */
    DDL_ZERO_STRUCT(stcLpmConfig);
    stcLpmConfig.enSEVONPEND   = SevPndDisable;
    stcLpmConfig.enSLEEPDEEP   = SlpDpEnable;
    stcLpmConfig.enSLEEPONEXIT = SlpExtDisable;
    Lpm_Config(&stcLpmConfig);

    Clk_SetFunc(ClkFuncWkupRCH, TRUE);

    __NOP(); __NOP(); __NOP(); __NOP(); __NOP();

    /* --- 8. Enter deep sleep --- */
    Lpm_GotoLpmMode();

    /* --- 9. Wake-up recovery: re-enable BGR and RCL --- */
    M0P_BGR->CR_f.BGR_EN = 1u;
#if !defined(BOARD_TYPE_RX)
    Clk_Enable(ClkRCL, TRUE);
#endif
}

/*
 * Legacy: full system low power entry
 */
void user_system_enter_low_power_mode(void)
{
    M0P_CLOCK->PERI_CLKEN = 0u;
    Clk_SetPeripheralGate(ClkPeripheralFlash, TRUE);
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
    LowPower_GpioConfig();
    user_enter_deep_sleep_mode();
}

uint8_t user_low_power_is_timer_running(void)
{
    return u8LowPowerEnabled;
}
