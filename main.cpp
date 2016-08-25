#include <iostm8s003k3.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intrinsics.h>
#include <inttypes.h>

#include "__RCSwitch.h"
#include "hw.h"
#include "EletroValvula.h"
#include "tempo.h"
#include "analog.h"
#include "eeprom.h"
#include "motor.h"
#include "parametro.h"

unsigned char *confOpenBeep = (unsigned char *) (0x400E);
unsigned char *confCloseBeep = (unsigned char *) (0x400F);

char *ver = "v1.0\n\r.>";
char *id = "ThDoor\n\r.>";
char *on = "On\n\r.>";
char *off = "Off\n\r.>";

#define RF_CMD_OPEN     1
#define RF_CMD_CLOSE    2

uint32_t usTickCnt;
uint32_t msTickCnt;
uint32_t RFCmd = 0;
uint32_t DelayReceiver = 0;
bool __EnableReceiver = 0;
uint8_t __RF_DoorCmd = 0;
Timeout_t AppTimer;

int main()
{
    Hw h;           //Generic hardware init
    Tempo t;        //Timers and Delay
    Eeprom e;       //eeprom handler
    Parametro p;
    Motor m;
    EletV EletroValvula;
    
    h.InitialiseSystemClock();
    h.Cfg_LED_Verde();
    h.Cfg_PD6_ExternalInterrupt();
    h.EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_ONLY);
    h.EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_RISE_ONLY);
    
    EletroValvula.Init();
    __RCSwitchInit();
    __enable_interrupt();
    
    t.InitTimeout(&AppTimer, 1000);
   
    while (1)
    {
        if(t.CheckTimeout(&AppTimer))
        {
            PD_ODR ^= (1<<0);
            //PC_ODR ^= (1<<1);
            //PC_ODR ^= (1<<2);
        }
        
        if(p.EdgeEnter())
        {
            __EnableReceiver = 1;
            __RF_DoorCmd = RF_CMD_OPEN;
        }
        
        
        if(!__EnableReceiver)
        {
            if (__IsCmd_RFavailable())
            {
                RFCmd = __getReceivedValue();
                __resetAvailable();
                
                if(RFCmd == 0x0A845875)
                {
                    RFCmd = 0;
                    __EnableReceiver = 1;
                    __RF_DoorCmd = RF_CMD_OPEN;
                }
                if(RFCmd == 0x0A845876)
                {
                    RFCmd = 0;
                    __EnableReceiver = 1;
                    __RF_DoorCmd = RF_CMD_CLOSE;
                }
            }
        }
        if(__EnableReceiver == 1) //trata o Cmd recebido por RF
        {
            switch(__RF_DoorCmd){
            case RF_CMD_OPEN:
                if(EletroValvula.AbrePorta())
                {
                    __EnableReceiver = 0;
                }
                break;
            case RF_CMD_CLOSE:
                if(EletroValvula.FechaPorta())
                {
                    __EnableReceiver = 0;
                }
                break;
            }
        }
    }
}




