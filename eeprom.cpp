#include <iostm8s003k3.h>
#include <inttypes.h>
#include "eeprom.h"

Eeprom::Eeprom()
{    
    FLASH_IAPSR_DUL = 0;
}

void Eeprom::SetValue(unsigned char valor, char pos)
{
    if (FLASH_IAPSR_DUL == 0)
    {
        FLASH_DUKR = 0xae;
        FLASH_DUKR = 0x56;
    }
    char *address = (char *) (0x4000 | pos);
    *address = valor;
    
    FLASH_IAPSR_DUL = 0; 
}

//This function will write a 4 byte (32bit) long to the eeprom at the specified address to address + 3.
void Eeprom::Writelong(uint8_t address, uint32_t value)
{
    //Decomposition from a long to 4 bytes by using bitshift.
    //One = Most significant -> Four = Least significant byte
    uint8_t four  = (value & 0xFF);
    uint8_t three = ((value >> 8)  & 0xFF);
    uint8_t two   = ((value >> 16) & 0xFF);
    uint8_t one   = ((value >> 24) & 0xFF);

    //Write the 4 bytes into the eeprom memory.
    SetValue(address, four);
    SetValue(address + 1, three);
    SetValue(address + 2, two);
    SetValue(address + 3, one);
}

uint32_t Eeprom::Readlong(uint8_t address)
{
    //Read the 4 bytes from the eeprom memory.
    long four  = GetValue(address);
    long three = GetValue(address + 1);
    long two   = GetValue(address + 2);
    long one   = GetValue(address + 3);

    //Return the recomposed long by using bitshift.
    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

unsigned char Eeprom::GetValue(char pos)
{
    char *address = (char *) (0x4000 | pos);
    return *address;
}

void Eeprom::InitEeprom()
{
    SetValue('T',0);
    SetValue('H',1);
    SetValue(FATORPWM,2);
    SetValue(TDOOROFF,3);
    SetValue(TTESTEND,4);
    SetValue(TFORCAFECHA,5);
    SetValue(CORRCC,6);
    SetValue(VEL,7);
    SetValue(CONTACC,8);
    SetValue(CONTACCTESTE,9);
    SetValue(CONTACCFORCA,10);
    SetValue(OPENTIME,11);
    SetValue(CLOSETIME,12);
    SetValue(TACOCONF,13);   
    SetValue(OPENBEEP,14);
    SetValue(CLOSEBEEP,15);   
    
    /*vetor dos parametros */
    SetValue(0,16);
    SetValue(0,17);   
    SetValue(0,18);
    //SetValue(0,19);     
}