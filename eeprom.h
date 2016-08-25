#ifndef INCLUDE_EEPROM_H
#define INCLUDE_EEPROM_H

#include <inttypes.h>

#define FATORPWM 12     //quanto maior maior a rampa softstart
#define TDOOROFF 40     //Tempo máximo de abertura ou fechamento
#define TTESTEND 3      //tempo para liberar o teste do fim de curso
#define TFORCAFECHA 6   //tempo máximo para forçar o fechamento
#define CORRCC 30       //corrente que considera alta na porta
#define VEL 50          //Velocidade (quanto maior mais leto)
#define CONTACC 40      //Quantas vezer q le corrente alta para a porta
#define CONTACCTESTE 80 //Quantas vezer q le corrente alta para a porta no estado TEST
#define CONTACCFORCA 20 //Quantas vezes q tem q ler corrente alta no estado FORCAFECHA
#define OPENTIME 0      //Tempo para abertura da porta é décimos (0 = sem tempo)
#define CLOSETIME 0     //Tempo para fechamento da porta é décimos (0 = sem tempo)
#define TACOCONF 0      //Config do Taco, 0 = pulsado, 1 = positivo, 2 = Negativo
#define OPENBEEP 1      //Se beepa na abertura
#define CLOSEBEEP 1     //Se beepa no fechamento

class Eeprom
{
    
public:
	Eeprom();
    
    /* Escreve na memória e2prom */
    void SetValue(unsigned char valor, char pos);
    
    /* Le da memória e2prom */
    unsigned char GetValue(char pos);
    
    void Writelong(uint8_t address, uint32_t value);
    
    uint32_t Readlong(uint8_t address);
    
    /* Inicializa eeprom se eh a primeira vez que o ThDoor é ligado */
    void InitEeprom();
};
#endif