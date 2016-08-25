#include <iostm8s003k3.h>
#include "motor.h"

unsigned char status;
unsigned char tDoorOff = 0, tTestEnd = 0, tForcaFecha = 0;
unsigned char openTime, closeTime;
unsigned char contaCurto = 0, contaSuperCurto = 0;
unsigned char fatorPWM;
unsigned char contaCurtoReal;
bool andando;   //exclusivo para velocidade pulsada
bool parado;
bool testEnd;
bool fOpen, fClose, fOpenClose, fEmer, fEnd, fTaco;
bool desacelera;
bool endRetest; //um filtro rápido para a microchave, lida na interrupção

/* Variaveis de compensação */
unsigned char compLimAcc;   //Limite na Aceleração já compensado pela voltagem e configuração
unsigned char compLimDes;   //Limite na Desaceleração já compensado pela voltagem e configuração
unsigned char compMotorAP;   //Velociade inicial do força fecha já compensado
/**/

bool curto, esmag, erroEnd;

unsigned char *confFatorPWM = (unsigned char *) (0x4002);
unsigned char *conftDoorOff = (unsigned char *) (0x4003);
unsigned char *conftTestEnd = (unsigned char *) (0x4004);
unsigned char *conftForcaFecha = (unsigned char *) (0x4005);
unsigned char *confCorrCC = (unsigned char *) (0x4006);
unsigned char *confVel = (unsigned char *) (0x4007);
unsigned char *confContaCC = (unsigned char *) (0x4008);
unsigned char *confContaCCTeste = (unsigned char *) (0x4009);
unsigned char *confContaCCForca = (unsigned char *) (0x400A);
unsigned char *confOpenTime = (unsigned char *) (0x400B);
unsigned char *confCloseTime = (unsigned char *) (0x400C);
unsigned char *confTaco = (unsigned char *) (0x400D);

Motor::Motor()
{
//    status = INDEFINIDO;
//
//    TIM1_ARRH = 0x00;       //  Reload counter = 9600
//    TIM1_ARRL = 0xFE;
//    TIM1_PSCRH = 0x00;         //  Prescalar = 0 (i.e. 1)
//    TIM1_PSCRL = 0x34;
//    TIM1_CR1_DIR = 0x00;       //  Up counter.
//    TIM1_CR1_CMS = 0x00;       //  Edge aligned counter.
//    TIM1_RCR = 0x00;           //  No repetition.
//    TIM1_IER = 0x01;
//    
//    /* Configuração do 18 (PC1/TIM1_CH1) */
//    TIM1_CCMR1_OC1M = 7;    //  Set up to use PWM mode 2.
//    TIM1_CCER1_CC1E = 1;    //  Output is enabled.
//    TIM1_CCER1_CC1P = 0;    //  Active is defined as high.
//    TIM1_CCR1H = 0x00;      //  480 = 50% duty cycle (based on TIM1_ARR).
//    TIM1_CCR1L = 0xFF;      // Desliga pino PC1
//    
//    /* Configuração do 19 (PC2/TIM1_CH2) */
//    TIM1_CCMR2_OC2M = 7;    //  Set up to use PWM mode 2.
//    TIM1_CCER1_CC2E = 1;    //  Output is enabled.
//    TIM1_CCER1_CC2P = 0;    //  Active is defined as high.
//    TIM1_CCR2H = 0x00;      //  127 = 50% duty cycle (based on TIM1_ARR).
//    TIM1_CCR2L = 0xFF;      // Desliga pino PC2
//    
//    TIM1_BKR_MOE = 1;       //  Enable the main output.
//    TIM1_CR1_CEN = 1;
//    fatorPWM = *confFatorPWM;
//    contaCurtoReal = 0;
//    
//    curto = false;
//    esmag = false;
//    erroEnd = false;
}

#pragma vector = TIM1_OVR_UIF_vector 
__interrupt void ReadEndF() 
{
//    TIM1_IER = 0x00;
    if(status == FECHANDO && !tTestEnd && END_F)
    {   
        if(END_F && endRetest)
        { 
            MOTORHN = 0;
            MOTORAP = 0xFF;
            MOTORHP = 0xFF;
            MOTORAN = 1;            
        }
        endRetest = true;
    }else endRetest = false;
    
//    TIM1_IER = 0x01;
    TIM1_SR1 = 0;	// clear interrupt flag(s)
} 

void Motor::Run(unsigned char corrente1, unsigned char corrente2)
{        
    FiltraEntradas();
    unsigned char corrente;
    corrente = corrente1 + corrente2;
    
    if(corrente > 150)
    {
        contaCurtoReal ++;
        if(contaCurtoReal > 20)
        {
            Desliga(); 
            curto = true;
            contaCurtoReal = 0;
        }
        contaCurto = contaCurto + 4; //Corrente muito alta da um up grande no Conta Curto
    }
    else if(corrente > 100)
    {
        contaCurto = contaCurto + 3; //Corrente muito alta da um up grande no Conta Curto
    }
    else if(corrente > *confCorrCC)
    {
        contaCurto ++; 
    }
    else if(corrente > 1 && corrente < 8)
    {
        if(contaCurto) contaCurto--;
        contaCurtoReal = 0;
    }
            
    if(!tDoorOff)
        Desliga();

    if(fEmer)
    {
        status = INDEFINIDO;
        Desliga(); 
    }
    
    if(fOpen && fClose)
    {
        status = INDEFINIDO;
        Desliga(); 
    }

    if(fEnd) testEnd = false;
        
    if(!tTestEnd)   //Já esta liberado para testar fim de curso
    {
        if(!fEnd)
        {
            if(status == ABRINDO) 
            {
                if(testEnd) erroEnd = true;
                Desliga();
                status = ABERTA;
            }
            if(status == FECHANDO)
            {
                //MOTORAP = 0xBF;     //Liga com duty menos da metade
                TIM1_PSCRL = 0x01;
                Abre();
                status = FORCA_FECHA;
                tDoorOff = *conftForcaFecha + 1;
                tForcaFecha = *conftForcaFecha;
                Fecha();
                MOTORAP = compMotorAP;
            }
        }
    }
    
    /* Verifica se esta parado */
    parado = true;
    
    if(*confTaco == 0)
    {
        if(andando) parado = false;
    }    
    else if(*confTaco == 1)
    {
        if(!fTaco) parado = false;
    }    
    else
    {
        if(fTaco) parado = false;
    }
        
    switch(status)
    {
        
    case ABRINDO:
        if(!desacelera)
        {
            if(MOTORHP > compLimAcc) // Está abrindo mas ainda nao esta com o duty 100%
            {
                fatorPWM --;
                if(!fatorPWM) //softstart configuravel com fatorPWM
                {
                    MOTORHP --;
                    fatorPWM = *confFatorPWM;
                }            
            }else desacelera = true;
        }
        else
        { 
            if(MOTORHP < compLimDes) // Está abrindo mas ainda nao esta com o duty 100%
            {
                fatorPWM --;
                if(!fatorPWM) //softstart configuravel com fatorPWM
                {
                    MOTORHP ++;
                    fatorPWM = *confFatorPWM;
                }            
            }
 
        }
        if(contaCurto > *confContaCC)
        {
            if(tDoorOff > *conftDoorOff - 10)   // Se cair aqui, provavelmente a porta esta perdida
            {
                contaCurto = 0;
                status = INDEFINIDO;
                Desliga();                  
            }
            else
            {
                contaCurto = 0;
                status = ABERTA;
                Desliga();                  
            }
        }
        if(fClose || EdgeOpenClose())
        {
/*            tTestEnd = 3;
            tDoorOff = *conftDoorOff;
            contaCurto = 0;
            status = FECHANDO;
            desacelera = false;
            Fecha();*/
            Desliga();
            status = TEMPO_FECHA;
            closeTime = 5;
        }
        break;       
    case ABERTA:
        if(fClose || EdgeOpenClose())
        {
            closeTime = *confCloseTime;
            openTime = 0;
            if(closeTime)
            {
                status = TEMPO_FECHA;
            }
            else
            {
                tTestEnd = *conftTestEnd;
                tDoorOff = *conftDoorOff;
                contaCurto = 0;
                status = FECHANDO;
                desacelera = false;
                Fecha();
            }
        }
        break;

    case TEMPO_FECHA:
        if(!closeTime)
        {
            tTestEnd = *conftTestEnd;
            tDoorOff = *conftDoorOff;
            contaCurto = 0;
            status = FECHANDO;
            desacelera = false;
            Fecha();            
        }
        if(fOpen || EdgeOpenClose())
        {
            tTestEnd = 0;
            tDoorOff = 0;
            contaCurto = 0;
            status = ABERTA;
            Desliga();              
        }
        
        break;
        
    case FECHANDO:
        if(!desacelera)
        {
            if(MOTORAP > compLimAcc) // limite da aceleração
            {
                fatorPWM --;
                if(!fatorPWM) //softstart configuravel com fatorPWM
                {
                    MOTORAP --;
                    fatorPWM = *confFatorPWM;
                }            
            }else desacelera = true;
        }
        else
        {
            if(MOTORAP < compLimDes) // limite da desaceleração
            {
                fatorPWM --;
                if(!fatorPWM) //softstart configuravel com fatorPWM
                {
                    MOTORAP ++;
                    fatorPWM = *confFatorPWM;
                }            
            }
            
        }
        if(contaCurto > *confContaCC)
        {
            if(tDoorOff > *conftDoorOff - 3)   // Se cair aqui, provavelmente a porta esta perdida
            {
                contaCurto = 0;
                status = INDEFINIDO;
                Desliga();                  
            }
            else
            {         
                esmag = true;
                tTestEnd = tDoorOff/2 + 1;
                tDoorOff = *conftDoorOff;
                contaCurto = 0;
                status = ABRINDO;
                desacelera = false;
                Abre();  
            }
        }
        if(fOpen || EdgeOpenClose())
        {

 /*           tTestEnd = 2;
            tDoorOff = *conftDoorOff;
            contaCurto = 0;
            status = ABRINDO;
            desacelera = false;
            Abre();*/
            Desliga();
            status = TEMPO_ABRE;
            openTime = 5;
        }
        break;        
        
    case FECHADA:
        if((fOpen || EdgeOpenClose()) && parado == true)
        {
            testEnd = true;     //habilita o teste de fim de curso
            openTime = *confOpenTime;
            closeTime = 0;
            if(openTime)
            {
                status = TEMPO_ABRE;
            }
            else
            {            
                tTestEnd = *conftTestEnd;
                tDoorOff = *conftDoorOff;
                contaCurto = 0;
                status = ABRINDO;
                desacelera = false;
                Abre();
            }
        }
        break;
        
    case TEMPO_ABRE:
        if(!openTime)
        {
            tTestEnd = *conftTestEnd;
            tDoorOff = *conftDoorOff;
            contaCurto = 0;
            status = ABRINDO;
            desacelera = false;
            Abre(); 
        }
        if(fClose || EdgeOpenClose())
        {
            tTestEnd = 0;
            tDoorOff = 0;
            contaCurto = 0;
            status = FECHADA;
            Desliga();              
        }
        
        break;        
        
    case INDEFINIDO:
        if(fOpen || EdgeOpenClose())
        {
            tDoorOff = *conftDoorOff;
            tTestEnd = 20;
            contaCurto = 0;
            Abre();            
            status = TEST_ABRE;   
            tTestEnd = 10;

        }
        else if(fClose)
        {
            tDoorOff = *conftDoorOff;
            tTestEnd = 20;
            contaCurto = 0;
            Fecha();            
            status = TEST_FECHA;
            tTestEnd = 10;
        }
        
        break;
        
    case TEST_ABRE:
        if(MOTORHP) // Está abrindo mas ainda nao esta com o duty 100%
        {
            fatorPWM --;
            if(!fatorPWM) //softstart configuravel com fatorPWM
            {
                MOTORHP --;
                fatorPWM = *confFatorPWM;
            }            
        }            
        if(contaCurto > *confContaCCTeste || (!fEnd && tTestEnd < 2) || !tDoorOff)
        {
            contaCurto = 0;
            status = ABERTA;
            Desliga();
        } 
        break;

    case TEST_FECHA:
        if(MOTORAP) // Está fechando mas ainda nao esta com o duty 100%
        {
            fatorPWM --;
            if(!fatorPWM) //softstart configuravel com fatorPWM
            {
                MOTORAP --;
                fatorPWM = *confFatorPWM;
            }            
        }
        if(contaCurto > *confContaCCTeste || (!fEnd && tTestEnd < 2) || !tDoorOff)
        {
            contaCurto = 0;
            status = FECHADA;
            Desliga();
        }        
        break;        

     case FORCA_FECHA:
        /*if(MOTORAP > 0xC0)
        {
            fatorPWM --;
            if(!fatorPWM) //softstart configuravel com fatorPWM
            {
                MOTORAP --;
                fatorPWM = *confFatorPWM;
            }
        }*/
        if(contaCurto > *confContaCCForca || !tForcaFecha)
        {
            contaCurto = 0;
            status = FECHADA;
            TIM1_PSCRL = 0x34;
            Desliga();  
        }
        if(fOpen || EdgeOpenClose())
        {
            tTestEnd = *conftTestEnd;
            tDoorOff = *conftDoorOff;
            contaCurto = 0;
            status = ABRINDO;
            TIM1_PSCRL = 0x34;
            desacelera = false;
            Abre();
        }
        break;   
        
    default:
        Desliga();
    }
}

void Motor::BaseTempo()
{
    if(closeTime)
    {
        closeTime --;
    }
    else if(openTime)
    {
        openTime--;
    }
    else
    {
        if(tDoorOff)
        {
            tDoorOff--;
            
            if(tTestEnd && fEnd)
                tTestEnd --;
        }
        
        if(tForcaFecha)
            tForcaFecha --;
    }
}

void Motor::Desliga()
{
    MOTORAN = 0;
    MOTORHN = 0;
    MOTORAP = 0xFF;
    MOTORHP = 0xFF;
}

void Motor::Abre()
{
    Desliga();
    
    unsigned int n = 200;
    while (n-- > 0); 
    
    MOTORAN = 1;
    MOTORHP = ~0x0F;
}

void Motor::Fecha()
{
    Desliga();

    unsigned int n = 200;
    while (n-- > 0); 
    
    MOTORHN = 1;
    MOTORAP = ~0x0F;
}

bool Motor::EdgeOpenClose()
{
    static bool aux;
    
    if(!fOpenClose && aux)
    {
        aux = false;    //resta o edge, e só libera quando tiver negativo no pino
        return true;
    }
    if(fOpenClose)
        aux = true;     //recebe true o permite usar o edge.
    
    return false;
}

unsigned char Motor::getStatus()
{
    return status;
}

void Motor::Set5Km(bool maisque5)
{
    andando = maisque5;
}

bool Motor::GetCurto()
{
    if(curto)
    {
        curto = false;
        return true;
    }else return false;
}

bool Motor::GetEsmag()
{
    if(esmag)
    {
        esmag = false;
        return true;
    }else return false;
}

bool Motor::GetErroEnd()
{
    if(erroEnd)
    {
        erroEnd = false;
        return true;
    }else return false;
}

void Motor::FiltraEntradas()
{
    unsigned char count[6] = {0,0,0,0,0,0};
    for(int i = 0; i < 6; i++)
    {
        /** Filtro Tecla abertura de Porta */
        if(!fOpen)
        {
            if(!OPEN_F) count[0] ++;
        }
        else
        {
            if(OPEN_F) count[0] ++;
        }
        
        /** Filtro Tecla fechamento de Porta */
        if(!fClose)
        {
            if(!CLOSE_F) count[1] ++;
        }
        else
        {
            if(CLOSE_F) count[1] ++;
        }
        
        /** Filtro Tecla Abre e Fecha a POrta */
        if(!fOpenClose)
        {
            if(!OPEN_CLOSE_F) count[2] ++;
        }
        else
        {
            if(OPEN_CLOSE_F) count[2] ++;
        }
        
        /** Filtro Tecla Emergencia */
        if(!fEmer)
        {
            if(!EMERGENCY_F) count[3] ++;
        }
        else
        {
            if(EMERGENCY_F) count[3] ++;
        }
        
        /** Filtro Microchave de fim de curso */
        if(!fEnd)
        {
            if(!END_F) count[4] ++;
        }
        else
        {
            if(END_F) count[4] ++;
        }
        
        /** Filtro Tacografo */
        if(!fTaco)
        {
            if(!TACO) count[5] ++;
        }
        else
        {
            if(TACO) count[5] ++;
        }
        
    }
    if(count[0] > 5) fOpen = !fOpen;
    if(count[1] > 5) fClose = !fClose;
    if(count[2] > 5) fOpenClose = !fOpenClose;
    if(count[3] > 5) fEmer = !fEmer;
    if(count[4] > 5) fEnd = !fEnd;
    if(count[5] > 5) fTaco = !fTaco;
}

void Motor::Compensa(unsigned char voltz)
{
    if(voltz < 150) //12V
    {
        
        compLimAcc = (voltz / 2) + *confVel;
        compLimDes = voltz + *confVel;
        if(voltz > 112) compMotorAP = 0xD0;
        else compMotorAP = voltz * 2 - 10;
    }
    else
    {
        compLimAcc = (voltz / 4)  + *confVel;
        compLimDes = (voltz / 2) + *confVel;
        if(voltz > 224) compMotorAP = 0xD0;
        else compMotorAP = voltz - 10;
    }
}
