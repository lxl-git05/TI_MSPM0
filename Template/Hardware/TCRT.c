#include "TCRT.h"

bool isLoad(void)
{
    return !DL_GPIO_readPins(GPIO_TCRT_PORT , GPIO_TCRT_TCRT_0_PIN);
}
