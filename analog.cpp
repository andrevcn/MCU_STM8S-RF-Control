#include <iostm8s003k3.h>
#include "analog.h"

unsigned char canal;
Analog::Analog()
{
    ADC_CR1 = 0x00;
    ADC_CR1_ADON = 1;

    canal = 9;
    unsigned int n = 10;
    while (n-- > 0);
    
    /* Start Conversion */
    ADC_CR1_ADON = 1;
}

void Analog::Run()
{
    if(ADC_CSR_EOC)
    {
        //ADC_CR1_ADON = 1;
        //ADC_CSR_EOC = 0;
        valor[canal] = ADC_DRH;
        ADC_CR1_ADON = 1;
        canal ++;
        if(canal > 2)
            canal = 0;
        ADC_CSR = canal;
        ADC_CR1_ADON = 1;
    }
}

unsigned char Analog::GetValue(unsigned char index)
{
    return valor[index];
}

