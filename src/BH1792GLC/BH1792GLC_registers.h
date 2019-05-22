/**
 *  @file       Bh1792glcReg.h
 *  @brief      BH1792GLC registers.
 *              Ported from Rohm Semiconductor Arduino library.
 *  @date       2018.02.03
 *  @version    1.0.0
 */
#ifndef BH1792GLC_registers
#define BH1792GLC_registers

/* registers */

// Error Codes
#define BH1792_SUCCESS                  (0)      // BH1792 operation is successful
#define BH1792_I2C_ERR                  (-1)     // I2C error with BH1792
#define BH1792_NOT_EXIST                (-2)     // BH1792 does not exist
#define BH1792_FIFO_FULL                (-3)     // BH1792 FIFO is full
#define BH1792_INVALID_MODE             (-4)

// Slave Address
#define BH1792_SLAVE_ADDR               (0x5BU)  // 7bit Slave Address

// Register Address
#define BH1792_ADDR_MANUFACTURERID      (0x0FU)  // R   : Manufacturer ID
#define BH1792_ADDR_PARTID              (0x10U)  // R   : PART ID
#define BH1792_ADDR_RESET               (0x40U)  // R/W : RESET
#define BH1792_ADDR_MEAS_CTRL1          (0x41U)  // R/W : Measurement Control1
#define BH1792_ADDR_MEAS_CTRL2          (0x42U)  // R/W : Measurement Control2
#define BH1792_ADDR_MEAS_CTRL3          (0x43U)  // R/W : Measurement Control3
#define BH1792_ADDR_MEAS_CTRL4_LSBS     (0x44U)  // R/W : Lower byte of Measurement Control4
#define BH1792_ADDR_MEAS_CTRL4_MSBS     (0x45U)  // R/W : Upper byte of Measurement Control4
#define BH1792_ADDR_MEAS_CTRL5          (0x46U)  // R/W : Measurement Control5
#define BH1792_ADDR_MEAS_START          (0x47U)  // R/W : Measurement Start
#define BH1792_ADDR_MEAS_SYNC           (0x48U)  // W   : Measurement Synchronization
#define BH1792_ADDR_FIFO_LEV            (0x4BU)  // R   : FIFO Level
#define BH1792_ADDR_FIFO_DATA0_LSBS     (0x4CU)  // R   : Lower byte of FIFO Data0
#define BH1792_ADDR_FIFO_DATA0_MSBS     (0x4DU)  // R   : Upper byte of FIFO Data0
#define BH1792_ADDR_FIFO_DATA1_LSBS     (0x4EU)  // R   : Lower byte of FIFO Data1
#define BH1792_ADDR_FIFO_DATA1_MSBS     (0x4FU)  // R   : Upper byte of FIFO Data1
#define BH1792_ADDR_IRDATA_LEDOFF_LSBS  (0x50U)  // R   : Lower byte of IRDATA LEDOFF
#define BH1792_ADDR_IRDATA_LEDOFF_MSBS  (0x51U)  // R   : Upper byte of IRDATA LEDOFF
#define BH1792_ADDR_IRDATA_LEDON_LSBS   (0x52U)  // R   : Lower byte of IRDATA LEDON
#define BH1792_ADDR_IRDATA_LEDON_MSBS   (0x53U)  // R   : Upper byte of IRDATA LEDON
#define BH1792_ADDR_GDATA_LEDOFF_LSBS   (0x54U)  // R   : Lower byte of GDATA LEDOFF
#define BH1792_ADDR_GDATA_LEDOFF_MSBS   (0x55U)  // R   : Upper byte of GDATA LEDOFF
#define BH1792_ADDR_GDATA_LEDON_LSBS    (0x56U)  // R   : Lower byte of GDATA LEDON
#define BH1792_ADDR_GDATA_LEDON_MSBS    (0x57U)  // R   : Upper byte of GDATA LEDON
#define BH1792_ADDR_INT_CLEAR           (0x58U)  // R   : Interrupt Clear

// Register Parameters
// MANUFACTURER ID (0Fh)
#define BH1792_PRM_MANUFACTURERID       (0xE0U)

// PART ID (10h)
#define BH1792_PRM_PARTID               (0x0EU)

// RESET (40h)
// 7bit:   SWRESET
#define BH1792_PRM_SWRESET              (0x01U)  // Software reset is performed

// MEAS_CTRL1 (41h)
// 7bit:   RDY
#define BH1792_PRM_RDY                  (0x01U)  // OSC block is active
// 6bit:   SEL_LED
#define BH1792_PRM_SEL_LED              (0x00U)  // LED1=Green, LED2=Green, LED3=IR
// 4bit:   SEL_ADC
#define BH1792_PRM_SEL_ADC_GREEN        (0x00U)  // ADC1=Green
#define BH1792_PRM_SEL_ADC_IR           (0x01U)  // ADC2=IR
// 2-0bit: MSR
#define BH1792_PRM_MSR_32HZ             (0x00U)  // LED_Freq= 128Hz, OUTPUT_Freq=  32Hz, FIFO=Stored
#define BH1792_PRM_MSR_128HZ            (0x01U)  // LED_Freq= 128Hz, OUTPUT_Freq= 128Hz, FIFO=Stored
#define BH1792_PRM_MSR_64HZ             (0x02U)  // LED_Freq= 256Hz, OUTPUT_Freq=  64Hz, FIFO=Stored
#define BH1792_PRM_MSR_256HZ            (0x03U)  // LED_Freq= 256Hz, OUTPUT_Freq= 256Hz, FIFO=Stored
#define BH1792_PRM_MSR_1024HZ           (0x05U)  // LED_Freq=1024Hz, OUTPUT_Freq=1024Hz, FIFO=Stored
#define BH1792_PRM_MSR_NON_SYNC         (0x06U)  // LED_Freq=   4Hz, OUTPUT_Freq=   4Hz, FIFO=Not Stored
#define BH1792_PRM_MSR_SINGLE           (0x07U)  // LED_Freq=   -Hz, OUTPUT_Freq=   -Hz, FIFO=Not Stored

// MEAS_CTRL2 (42h)
// 7-6bit: LED_EN1
#define BH1792_PRM_LED_EN1_0            (0x00U)  // Selection of LED Driver Mode
#define BH1792_PRM_LED_EN1_1            (0x01U)
#define BH1792_PRM_LED_EN1_2            (0x02U)
#define BH1792_PRM_LED_EN1_3            (0x03U)
// 5-0bit: LED_CURRENT1
#define BH1792_PRM_LED_CUR1_MIN         (0x00U)  // LED1 MIN Current  :  0mA
#define BH1792_PRM_LED_CUR1_MAX         (0x3FU)  // LED1 MAX Current  : 63mA
#define BH1792_PRM_LED_CUR1_MA(x)       (x)      // LED1 Current Range: 0-63mA

// MEAS_CTRL3 (43h)
// 7bit:   LED_EN2
#define BH1792_PRM_LED_EN2_0            (0x00U)  // Selection of LED Driver Mode
#define BH1792_PRM_LED_EN2_1            (0x01U)
// 5-0bit: LED_CURRENT2
#define BH1792_PRM_LED_CUR2_MIN         (0x00U)  // LED2 MIN Current  :  0mA
#define BH1792_PRM_LED_CUR2_MAX         (0x3FU)  // LED2 MAX Current  : 63mA
#define BH1792_PRM_LED_CUR2_MA(x)       (x)      // LED2 Current Range: 0-63mA

// MEAS_CTRL5 (46h)
// 1-0bit: INT_SEL
#define BH1792_PRM_INT_SEL_DISABLE      (0x00U)  // No interrupt
#define BH1792_PRM_INT_SEL_WTM          (0x01U)  // FIFO watermark interrupt
#define BH1792_PRM_INT_SEL_IR           (0x02U)  // IR Threshold interrupt of Non-Sync Measurement Mode
#define BH1792_PRM_INT_SEL_SGL          (0x03U)  // Completion interrupt of Single Measurement Mode

// MEAS_START (47h)
// 0bit: MEAS_ST
#define BH1792_PRM_MEAS_ST              (0x01U)  // Measurement start

// MEAS_SYNC (48h)
// 0bit: MEAS_SYNC
#define BH1792_PRM_MEAS_SYNC            (0x01U)  // Measurement synchronization

// FIFO_LEVEL (4Bh)
// 5-0bit: FIFO_LEV
#define BH1792_PRM_FIFO_LEV_EMPTY       (0x00U)  // FIFO Length = 0  (Empty)
#define BH1792_PRM_FIFO_LEV_WTM         (0x20U)  // FIFO Length = 32 (Watermark)
#define BH1792_PRM_FIFO_LEV_FULL        (0x23U)  // FIFO Length = 35 (Full)


//etc
#define BH1792_LED_MODE_GREEN           (0x00U)
#define BH1792_LED_MODE_IR              (0x01U)


#define INT_CHECK_INTERVAL_US   (10)        /* Int terminal check interval [us] */
#define INT_CHECK_COUNT_LIMIT   (100)       /* Int terminal check count limit */

#define DEFAULT_MEAS_CTRL4_LSB  (0xFC)      /* Threshold IR interrupt (LSB) */
#define DEFAULT_MEAS_CTRL4_MSB  (0xFF)      /* Threshold IR interrupt (MSB) */

#endif    /* INC_Bh1792glcReg */
