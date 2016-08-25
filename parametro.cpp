#include <iostm8s003k3.h>
#include "parametro.h"
#include "eeprom.h"

bool emConf;
unsigned char tOut;         //tempo para sair da configuração
unsigned char valores[3];   //5 configurações
unsigned char iVal;
unsigned char tTeste;
unsigned char contaPisca;
unsigned char piscaLed;
unsigned char tLedOn, tLedOff;
unsigned char erroId;       // = 0 sem erros, 1,2,3 etc ver no .h
unsigned char estado;

Eeprom ee;

unsigned char *param1 = (unsigned char *) (0x4010);
unsigned char *param2 = (unsigned char *) (0x4011);
unsigned char *param3 = (unsigned char *) (0x4012);


Parametro::Parametro()
{
    emConf = false;
    erroId = 0;
}

bool Parametro::onConf()
{
    return emConf;
}

void SetPisca(unsigned char piscadas)
{
    contaPisca = 35;
    piscaLed = piscadas;
    tLedOn = 2;
    LED = 1;
}

void SetPiscaErro()
{
    contaPisca = 30;
    piscaLed = erroId;
    tLedOn = 2;
    LED = 1;
}

void Parametro::Run()
{
    tOut --;
    if(!tOut)
    {
        emConf = false;
        estado = 1;     //Acabou de sair da configuração
        SetPisca(6);       
    }
    else
    {
        if(EdgeEnter())
        {
            tOut = T_SAICONF;
            iVal++;
            if(iVal > 2) iVal = 0;
            SetPisca(iVal + 1);
            
        }
        else if(EdgeSel())
        {
            tOut = T_SAICONF;
            valores[iVal] ++;
            if(valores[iVal] > 2) valores[iVal] = 0;
            SetPisca(valores[iVal] + 1);
        }
        
        /* Lógica de piscar os leds */

        if(piscaLed)
        {            
            if(LED)
            {
                tLedOn--;
                if(!tLedOn)
                {
                    tLedOff = 5;
                    LED = 0;
                    piscaLed--;
                }
            }
            else
            {
                tLedOff--;
                if(!tLedOff)
                {
                    tLedOn = 2;
                    LED = 1;                    
                }
            }   
        }
    }
}

void Parametro::Test()
{
    if(estado)
    {
        if(piscaLed)
        {            
            if(LED)
            {
                tLedOn--;
                if(!tLedOn)
                {
                    tLedOff = 2;
                    LED = 0;
                    piscaLed--;
                }
            }
            else
            {
                tLedOff--;
                if(!tLedOff)
                {
                    tLedOn = 2;
                    LED = 1;                    
                }
            }   
        }else estado = 0;        
        
    }
    else if(!erroId)
    {
        LED = 0;
    }
    else
    {
        contaPisca --;
        if(!contaPisca)
        {
            SetPiscaErro();
        }
        if(piscaLed)
        {            
            if(LED)
            {
                tLedOn--;
                if(!tLedOn)
                {
                    tLedOff = 4;
                    LED = 0;
                    piscaLed--;
                }
            }
            else
            {
                tLedOff--;
                if(!tLedOff)
                {
                    tLedOn = 2;
                    LED = 1;                    
                }
            }   
        }        
    }
    
    if(GetSignal(!KEY_ENTER) && GetSignal(!KEY_SEL))
    {
        erroId = 0;
        tTeste --;
        if(!tTeste)
        {
            emConf = true;
            tOut = T_SAICONF;
            iVal = 4;
            valores[0] = ee.GetValue(16);
            valores[1] = ee.GetValue(17);
            valores[2] = ee.GetValue(18);
            SetPisca(3);
            //valores[3] = ee.GetValue(19);
        }
    }
    else tTeste = T_ENTRACONF;
    
    if(GetSignal(!KEY_SEL))
        erroId = 0;

}

bool Parametro::EdgeSel()
{
    static bool aux;
    
    if(GetSignal(!KEY_SEL) && aux)
    {
        aux = false;    //resta o edge, e só libera quando tiver negativo no pino
        return true;
    }
    if(GetSignal(KEY_SEL))
        aux = true;     //recebe true o permite usar o edge.
    
    return false;
}

bool Parametro::EdgeEnter()
{
    static bool aux;
    
    if(GetSignal(!KEY_ENTER) && aux)
    {
        aux = false;    //resta o edge, e só libera quando tiver negativo no pino
        return true;
    }
    if(GetSignal(KEY_ENTER))
        aux = true;     //recebe true o permite usar o edge.
    
    return false;
}

bool Parametro::GetSignal(unsigned char signal)
{
    unsigned char count = 0;
    for(int i = 0; i < 5; i++)
    {
        if(signal)
            count ++;
    }
    if(count > 3)
        return true;
    else 
        return false;           
}

void Parametro::SetError(unsigned char erro)
{
    if(erro > erroId)
    {
        erroId = erro;
        SetPiscaErro();
    }
}

