#include <iostm8s003k3.h>
#include <inttypes.h>

#include "tempo.h"
#include "hw.h"
#include "__RCSwitch.h"

uint32_t timer0;
uint8_t decimos;
uint8_t freql, freqh;
bool decFlag;
bool segFlag;
bool semPulsoFlag;  //se faz tempo q TACO esta parado, seta este bit
extern uint32_t usTickCnt;
extern uint32_t msTickCnt;

Hw h; //Generic hardware init

#pragma vector = TIM4_OVR_UIF_vector 
__interrupt void MyInterruptRoutine() 
{
    TIM4_SR   = 0; //clear interrupt flag(s)
    TIM4_CNTR = 0; //zera_contador
    usTickCnt++;
	timer0++;
    if(timer0 >= 10) //1ms???
    {
        msTickCnt++;
        timer0 = 0;
    }
} 

#pragma vector = TIM2_CAPCOM_CC3IF_vector
__interrupt void TacoInterrupt() 
{
    TIM2_IER = 0x00;
    TIM2_CNTRH = 0;
    TIM2_CNTRL = 0;    
    TIM2_SR1 = 0;
    TIM2_SR2 = 0;
    TIM2_IER = 0x08;    
    semPulsoFlag = false;   //Reseta o Sem pulso, pq agora tem
}

#pragma vector = EXTI3_vector
__interrupt void EXTI_PORTD_IRQHandler()
{
    if(RF_PIN)
    {
        h.EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY); 
        h.EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_FALL_ONLY);
        __handleInterrupt();
    }
    else if((RF_PIN)==0)
    {
        h.EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_ONLY);
        h.EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_RISE_ONLY);
        __handleInterrupt();
    }
}

Tempo::Tempo()
{
    /*
	TIM4_CR1  = 0x85;
	TIM4_IER  = 0x01;
    TIM4_CNTR = 0x88;
	TIM4_PSCR = 0x07;
	TIM4_ARR  = 0x20;
	TIM4_SR   = 0;	
    timer0 = TIMER;
    */
    
    TIM4_PSCR = (uint8_t)(TIM4_PRESCALER_64); 
	TIM4_ARR = (uint8_t)(25);
    TIM4_IER |= ((uint8_t)0x01);
	TIM4_CR1_CEN = 1;
	TIM4_CNTR = 0x00;
    TIM4_SR   = 0;
    timer0  = 0;
    decimos = 0;
    
    /*
    TIM2_CCMR3 = 0x41;
    TIM2_CCER2 = 0x01;
    TIM2_PSCR = 0x0B;
    TIM2_IER = 0x08;
    TIM2_CR1 = 0x01;
    */

}

uint32_t Tempo::micros(void){
    
    uint32_t temp = 0;
    temp = usTickCnt;
    temp = temp*100;
    return temp;
}

uint32_t Tempo::millis(void)
{
    return msTickCnt;
}

void Tempo::InitTimeout(Timeout_t *t, uint32_t timeout)
{
	t->start_time = millis();
	t->timeout = timeout;
}


uint32_t Tempo::CheckTimeout(Timeout_t *t)
{
    if((millis() - t->start_time) >= t->timeout)
    {
        t->start_time = millis();
        return 1;
    }
    else{
        return 0;
    }
}

bool Tempo::Decimo()
{
    if(decFlag)
    {
        decFlag = false;
        return true;
    }
    return false;
}

bool Tempo::Segundo()
{
    if(segFlag)
    {
        segFlag = false;
        return true;
    }
    return false;
}

void Tempo::Delay(unsigned int n)
{
    while (n-- > 0); 
}

bool Tempo::Get5Km()
{
    if(semPulsoFlag == true)
        return false;
    else if(TIM2_CNTRH > 1)      //meio q paro o veículo, 
    {
        semPulsoFlag = true;
        return false;
    }
    else if(TIM2_CCR3H ==0 && TIM2_CCR3L < 180)
        return true;   //Se periodo > 180 então ta a mais q 5km
    else
        return false;
}
