/* Timer_GetTime_Elapsed @ 0x8004B41C — ms since oldVal; writes current total to *retVal (if non-NULL);
 * wraps by adding 0xC7E18 when the new time is less than oldVal. */
extern int func_8004B3A4(void);   /* Timer_GetTime_Total */

int Timer_GetTime_Elapsed(int oldVal, int *retVal)
{
    int newVal = func_8004B3A4();
    if (retVal != 0) {
        *retVal = newVal;
    }
    if (newVal < oldVal) {
        newVal = newVal + 0xc7e18;
    }
    return newVal - oldVal;
}
