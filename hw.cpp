#include <iostm8s003k3.h>
#include "hw.h"

unsigned char tBuzzer, tBeepOn, tBeepOff, tmpOn, tmpOff;
unsigned char ccBuzzer, ccLamp, ccAcel; //contadores de curto

Hw::Hw ()
{

    /* Port A Config *
    1: Key_SEL, Input
    2: Key_ENTER, Input
    3: Taco, Input
    */
    PA_DDR = 0x00; //00000000
    PA_CR1 = 0x06; //00000110
    PA_CR2 = 0x00; //00000000

    /* Port B Config *
    0: Supply Voltage, Analog
    1: Motor H Voltage, Analog
    2: Motor AH Voltage, Analog
    3: Lamp Voltage, Analog
    4: Desconectado, Input
    5: Buzzer, Output
    6: Positivo Acel, Output
    7: Lamp, Output
    */    
    PB_ODR = 0x20; //00100000
    PB_DDR = 0xE0; //11100000
    PB_CR1 = 0xE0; //11100000
    PB_CR2 = 0x00; //00000000

    /* Port C Config *
    0: ---
    1: Positivo Motor H, Output
    2: Positivo Motor AH, Output
    3: Negativo Motor H, Output
    4: Negativo Motor AH, Output
    5: OPEN CLOSE SWITCH, Input
    6: CLOSE SWITCH, Input
    7: OPEN SWITCH, Input
    */        
    
    PC_ODR = 0x18; //00011000 
    PC_DDR = 0x1E; //00011110
    PC_CR1 = 0x1E; //00011110
    PC_CR2 = 0x1E; //00011110

    /* Port D Config *
    0: Led Error, Output
    1: SWIM
    2: Acel VCC feedback, Input
    3: Enable, Input
    4: EMERGENCY, Input
    5: Debug TX
    6: Debug RX
    7: Buzzer feedback, Input
    */       
    
    PD_ODR = 0x21; //00100001
    PD_DDR = 0x21; //00100001
    PD_CR1 = 0x31; //00110001
    PD_CR2 = 0x20; //00100000
    
    /*PE5 - End Open feedback, Input floating
      Don't need change DDR, CR1 and CR2 */
    
    /*PF4 - END SWITCH, Input floating
      Don't need change DDR, CR1 and CR2 */
    
    asm("rim"); 
    
    tBuzzer = 0;
}

void Hw::Buzzer(bool estado)
{
    if(estado)
        BUZZER = 0;
    else
        BUZZER = 1;
}

void Hw::Beeps(unsigned char tON, unsigned char tOFF, unsigned char vezes)
{
    if(!tBuzzer)
    {
        tBuzzer = (tON * vezes) + (tOFF * vezes);
        tBeepOn = tON;
        tBeepOff = tOFF;
        tmpOn = tON;
        tmpOff = tOFF;    
        Buzzer(LIGA);
    }
}

void Hw::BeepRun()
{
    if(tBuzzer)
    {
     
        tBuzzer --;
        if(!tBuzzer)
        {
            Buzzer(DESLIGA);
            return;
        }
        
        if(!BUZZER) //if Buzzer Ligado
        {
            tBeepOn --;
            if(!tBeepOn)
            {
                tBeepOn = tmpOn;
                Buzzer(DESLIGA);
            }
        }else if(BUZZER) //if Buzzer Ligado
        {
            tBeepOff --;
            if(!tBeepOff)
            {
                tBeepOff = tmpOff;
                Buzzer(LIGA);
            }
        }
    }
}

void Hw::Acelerador(bool estado)
{
    if(estado)
        ACEL = 1;
    else
        ACEL = 0;
}

void Hw::Lampada(bool estado)
{
    if(estado)
        LAMP = 1;
    else
        LAMP = 0;    
}

bool Hw::CheckCurtoAcc()    
{
    bool patafi = false;
    if(!BUZZER && BUZZER_F)
        ccBuzzer ++;
    else
        ccBuzzer = 0;
    
    if(ccBuzzer > 3)
    {            
        BUZZER = 1;
        patafi = true;  //sim tem algum curto
    }
    
    if(ACEL && !ACEL_VCC_F)
        ccAcel ++;
    else
        ccAcel = 0;
    
    if(ccAcel > 3)
    {
        ACEL = 0;
        patafi = true;  //sim tem algum curto
    }
    
    if(LAMP && LAMP_F)
        ccLamp ++;
    else
        ccLamp = 0;
    
    if(ccLamp > 3)
    {
        LAMP = 0;
        patafi = true;  //sim tem algum curto
    }
    
    return patafi;
}

void Hw::Cfg_PD6_ExternalInterrupt()
{
    //  Now configure the input pin for RF.
    PD_DDR_DDR6 = 0;                //  PD6 is input.
    PD_CR1_C16 = 0;                 //  PD6 is floating input.
    PD_CR2_C26 = 1;                 //  Pin can run up to 10 MHz.
    PD_CR2 |= GPIO_MODE_IN_PU_IT;   //  Interrupt (Input)
}

void Hw::Cfg_LED_Verde()
{
    //Cfg LED pin to Output
    PD_ODR |= (1<<0);
    PD_DDR |= (1<<0);
    PD_CR1 |= (1<<0);
    PD_ODR &= ~(1<<0);
}

/**
  * @brief  Set the external interrupt sensitivity of the selected port.
  * @warning
  * - The modification of external interrupt sensitivity is only possible when the interrupts are disabled.
  * - The normal behavior is to disable the interrupts before calling this function, and re-enable them after.
  * @param   Port The port number to access.
  * @param   SensitivityValue The external interrupt sensitivity value to set.
  * @retval None
  * @par Required preconditions:
  * Global interrupts must be disabled before calling this function.
  */
void Hw::EXTI_SetExtIntSensitivity(uint8_t Port, EXTI_Sensitivity_TypeDef SensitivityValue)
{
    /* Set external interrupt sensitivity */
    switch (Port){
    case EXTI_PORT_GPIOA:
        EXTI_CR1_bit.PAIS = 0;
        EXTI_CR1 |= (uint8_t)(SensitivityValue);
        break;
    case EXTI_PORT_GPIOB:
        EXTI_CR1_bit.PBIS = 0;
        EXTI_CR1 |= (uint8_t)((uint8_t)(SensitivityValue) << 2);
        break;
    case EXTI_PORT_GPIOC:
        EXTI_CR1_bit.PCIS = 0;
        EXTI_CR1 |= (uint8_t)((uint8_t)(SensitivityValue) << 4);
        break;
    case EXTI_PORT_GPIOD:
        EXTI_CR1_bit.PDIS = 0;
        EXTI_CR1 |= (uint8_t)((uint8_t)(SensitivityValue) << 6);
        break;
    case EXTI_PORT_GPIOE:
        EXTI_CR2_bit.PEIS = 0;
        EXTI_CR2 |= (uint8_t)(SensitivityValue);
        break;
    default:
        break;
    }
}


/**
  * @brief  Set the TLI interrupt sensitivity.
  * @param   SensitivityValue The TLI interrupt sensitivity value.
  * @retval None
  * @par Required preconditions:
  * Global interrupts must be disabled before calling this function.
  */
void Hw::EXTI_SetTLISensitivity(EXTI_TLISensitivity_TypeDef SensitivityValue)
{
    /* Set TLI interrupt sensitivity */
    EXTI_CR2_bit.TLIS = 0;
    EXTI_CR2 |= (uint8_t)(SensitivityValue);
}

//  Setup the system clock to run at 16MHz using the internal oscillator.
void Hw::InitialiseSystemClock(void){
    
    CLK_ICKR      = 0;                  //  Reset the Internal Clock Register.
    CLK_ECKR      = 0;                  //  Disable the external clock.
    CLK_CKDIVR    = 0;                  //  Ensure the clocks are running at full speed.
    CLK_PCKENR1   = 0xff;               //  Enable all peripheral clocks.
    CLK_PCKENR2   = 0xff;               //  Ditto.
    CLK_CCOR      = 0;                  //  Turn off CCO.
    CLK_HSITRIMR  = 0;                  //  Turn off any HSIU trimming.
    CLK_SWIMCCR   = 0;                  //  Set SWIM to run at clock / 2.
    CLK_SWR       = 0xe1;               //  Use HSI as the clock source.
    CLK_SWCR      = 0;                  //  Reset the clock switch control register.
    CLK_SWCR_SWEN = 1;                  //  Enable switching.
    while (CLK_SWCR_SWBSY != 0);        //  Pause while the clock switch isbusy.
}


