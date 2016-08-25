#include <iostm8s003k3.h>

#include "hw.h"
#include "EletroValvula.h"
#include "tempo.h"
#include "analog.h"
#include "eeprom.h"
#include "motor.h"
#include "parametro.h"

extern Tempo t;

Timeout_t tAbrePorta;
Timeout_t tFechaPorta;

void EletV::Init(void)
{
    //Cfg PC1 pin to Output (POSITIVO_M_H) //Abrir Porta
    PC_ODR |= (1<<1);
    PC_DDR |= (1<<1);
    PC_CR1 |= (1<<1);
    PC_ODR &= ~(1<<1);
    
    //Cfg PC2 pin to Output (POSITIVO_M_AH) //Fechar Porta
    PC_ODR |= (1<<2);
    PC_DDR |= (1<<2);
    PC_CR1 |= (1<<2);
    PC_ODR &= ~(1<<2);
}

bool EletV::AbrePorta(void)
{
    static uint8_t fsmAbre;
    bool retval = 0;
    switch(fsmAbre){
    case 0:
        t.InitTimeout(&tAbrePorta, 1000);
        PC_ODR |= (1<<1);  //POSITIVO_M_H em alto
        PC_ODR &= ~(1<<2); //POSITIVO_M_AH em baixo
        fsmAbre = 1;
        retval = 0;
        break;
    case 1:
        if(t.CheckTimeout(&tAbrePorta))
        {
            PC_ODR &= ~(1<<1);  //POSITIVO_M_H em baixo
            fsmAbre = 0;
            retval = 1;
        }
        break;
    }
    return retval;
}

bool EletV::FechaPorta(void)
{
    static uint8_t fsmFecha;
    bool retval = 0;
    switch(fsmFecha){
    case 0:
        t.InitTimeout(&tFechaPorta, 1000);
        PC_ODR &= ~(1<<1);  //POSITIVO_M_H em baixo
        PC_ODR |=  (1<<2);  //POSITIVO_M_AH em alto
        fsmFecha = 1;
        retval = 0;
        break;
    case 1:
        if(t.CheckTimeout(&tFechaPorta))
        {
            PC_ODR &= ~(1<<2);  //POSITIVO_M_AH em baixo
            fsmFecha = 0;
            retval = 1;
        }
        break;
    }
    return retval;
}



