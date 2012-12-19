/* Low-voltage detect functions.
 * The PIC 18F8520 has a low-voltage detect function on its VDD pin,
 * but it's rather coarse.  There is a LM2931 voltage regulator
 * supplying 5V to the PIC, and it has a nominal dropout voltage of
 * between 15mV @ 10mA and 160mV @ 100mA.
 *
 * The battery has a nominal voltage of 7.2V.
 *
 * We can infer the battery voltage by using this dropout voltage and
 * the low-voltage detect functionality.
 * Because the PIC operates at 5V, we can only use the two highest
 * settings of the low-voltage detect block:
 * 4.64V
 * 4.33V
 * which would correspond to battery voltages at low/high currents of:
 * 4.66V/4.80V
 * 4.35V/4.49V
 *
 * To use this low-voltage detector, We set the threshold and clear
 * the LVDIF flag.  When the voltage goes below threshold, LVDIF will
 * be set and stay set until cleared.
 */

#include "platform.h"
#include "lvd.h"

/**
 * \defgroup lvd Low Voltage Detect
 *  @{
 *
 *  These functions have not proven useful on the PIC-based Vex controller,
 *  since the power threshold detected is below the level required for
 *  typical robot operation.
 */

/**
 *  Set the LVD module for the given low voltage threshold.
 *  
 *  Use lvd_set() in conjunction with lvd_triggered() for minimal CPU
 *  usage and optimal respone time.
 *
 *  After calling lvd_set(), the LVD block will continuously consume
 *  up to 65uA.  If you want to minimize power consumption, Use
 *  lvd_check() instead of lvd_set() / lvd_triggered().
 *  
 *  \param  threshold One of the manifest constants LVD_433 (4.33V) or
 *  LVD_464 (4.64V).
 */

/****************************************************************************
 * History: 
 *  May 2009    Ag Primatic
 ****************************************************************************/

void    lvd_set(unsigned char threshold)
{
    /* Ensure that LVD interrupts are disabled */
    PIE2bits.LVDIE = 0;

    /* Disable the LVD module */
    LVDCONbits.LVDEN = 0;

    /* Write the value to LVDL[3:0] which selects the desired trip
     * point.
     */
    LVDCON &= 0xf0;
    LVDCON |= threshold;

    /* Enable the LVD module */
    LVDCONbits.LVDEN = 1;

    /* Wait for the internal voltage reference to stabilize.  This
     * takes 20-50 us.
     */
    while (LVDCONbits.BGST == 0)
	;

    /* Clear the LVD interrupt flag, which may have falsely become
     * set because of the register modification.
     */
    PIR2bits.LVDIF = 0;

    /* We could enable the LVD interrupt here */
}

/**
 *  Returns true if the low LVD block has detected a low voltage condition.
 *
 *  The threshold must first be set with lvd_set().
 */

/***************************************************************************
 * History: 
 *  May 2009    Ag Primatic
 ***************************************************************************/
 
unsigned char   lvd_triggered(void)
{
    return PIR2bits.LVDIF;
}

/**
 *  Returns true if the LVD block has detected a low voltage condition.
 *
 *  This is a standalone function that configures the LVD block with
 *  the given threshold, immediately tests the voltage, and disables
 *  the LVD block.
 *
 *  This entails a 20-50 us delay for each call, but reduces power
 *  consumption, since the LVD block continuously consumes up to 65uA
 *  when configured.
 *
 *  \param  threshold One of the manifest constants LVD_433 (4.33V) or
 *  LVD_464 (4.64V).
 */

/*******************************************************************************************
 * History: 
 *  May 2009    Ag Primatic
 *******************************************************************************************/

unsigned char   lvd_check(unsigned char threshold)
{
    unsigned char voltage_low;

    /* Ensure that LVD interrupts are disabled */
    PIE2bits.LVDIE = 0;

    /* Disable the LVD module */
    LVDCONbits.LVDEN = 0;

    /* Write the value to LVDL[3:0] which selects the desired trip
     * point.
     */
    LVDCON &= 0xf0;
    LVDCON |= threshold;

    /* Enable the LVD module */
    LVDCONbits.LVDEN = 1;

    /* Wait for the internal voltage reference to stabilize.  This
     * takes 20-50 us.
     */
    while (LVDCONbits.BGST == 0)
	;

    /* Clear the LVD interrupt flag, which may have falsely become
     * set because of the register modification.
     */
    PIR2bits.LVDIF = 0;

    /* Check the flag */
    voltage_low = PIR2bits.LVDIF;

    /* Disable the LVD module */
    LVDCONbits.LVDEN = 0;

    return voltage_low;
}

/** @} */

