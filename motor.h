#ifndef INCLUDE_MOTOR_H
#define INCLUDE_MOTOR_H

#define MOTORAN         PC_ODR_bit.ODR4     //Motor Anti hor�rio Negatico
#define MOTORHN         PC_ODR_bit.ODR3     //Motor hor�rio Negatico
#define FREIO           PC_ODR_bit.ODR1     //Motor hor�rio Positivo

/* O Duty est� invertido, ou seja, 0xFF desligado e 0x00 Ligado */
#define MOTORHP         TIM1_CCR1L          // Duty do pino PC1
#define MOTORAP         TIM1_CCR2L          // Duty do pino PC2

#define OPEN_F          PC_IDR_bit.IDR7 
#define CLOSE_F         PC_IDR_bit.IDR6
#define OPEN_CLOSE_F    PC_IDR_bit.IDR5 
#define EMERGENCY_F     PD_IDR_bit.IDR4
#define END_F           PF_IDR_bit.IDR4

#define TACO            PA_IDR_bit.IDR3

//Status do motor
#define INDEFINIDO  0
#define ABERTA      1
#define FECHADA     2
#define ABRINDO     3
#define FECHANDO    4
#define TEST_FECHA  5
#define TEST_ABRE   6
#define FORCA_FECHA 7
#define TEMPO_ABRE  8
#define TEMPO_FECHA 9

class Motor
{
public:
	Motor();
    
    /* Deve ser executado frequentemente, verifica curto e le teclas*/
    void Run(unsigned char corrente1, unsigned char corrente2);
    
    /* Deve ser executado a cada d�cimo de segundo */
    void BaseTempo();
    
    /* Desliga todos mosfets */
    void Desliga();
    
    /* Liga os mosfets de modo que a porta abra */
    void Abre();
    
    /* Liga os mosfets de modo que a porta feche */
    void Fecha();
    
    /* Fun��o auxiliar para verificar se a entrada est� positiva (filtrado) */
    bool GetHighSignal(unsigned char signal);
    
    /* Fun��o auxiliar para verificar se a entrada est� negativa (filtrado) */
    bool GetLowSignal(unsigned char signal);
    
    /* Fun��o edge especial da entrada da tecla OPEN CLOSE */
    bool EdgeOpenClose();
    
    /* Retorna o estado atual do motor */
    unsigned char getStatus();
    
    /* Seta para o motor que o ve�culo est� andando */
    void Set5Km(bool maisque5);
    
    /* Retorna se teve curto circuito */
    bool GetCurto();
    
    /* Retorna se teve anti esmagamento */
    bool GetEsmag();
    
    /* Retorna se existe defeito em algum fim de curso */
    bool GetErroEnd();
    
    /* retorna as entradas filtradas */
    void FiltraEntradas();
    
    /* Prepara as vari�veis de compensa��o, que mudam com voltagem, e configura��es */
    void Compensa(unsigned char voltz);
};
#endif