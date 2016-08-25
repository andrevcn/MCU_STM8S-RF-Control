
#ifndef INCLUDE_TEMPO_H
#define INCLUDE_TEMPO_H

#define TIMER 48;
#define RF_PIN PD_IDR_bit.IDR6

typedef struct{
    uint32_t start_time;
    uint32_t timeout;
}Timeout_t;

class Tempo
{
public:
    
	Tempo();
    
    /* entra em if(Decimo()) a cada décimo de segundo */
    bool Decimo();
    
    /* entra em if(Segundo()) a cada segundo */
    bool Segundo();
    
    /* Função delay simples */
    void Delay(unsigned int n);
    
    /* Detecta se o Veículo está a mais de 5km quando for vel pulsado */
    bool Get5Km();
    
    uint32_t micros(void);
    
    uint32_t millis(void);
    
    void InitTimeout(Timeout_t *t, uint32_t timeout);
    
    uint32_t CheckTimeout(Timeout_t *t);
	
};
#endif