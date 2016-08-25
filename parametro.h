
#ifndef INCLUDE_PARAMETRO_H
#define INCLUDE_PARAMETRO_H

#define KEY_SEL     PA_IDR_bit.IDR1
#define KEY_ENTER   PA_IDR_bit.IDR2
#define LED         PD_ODR_bit.ODR0

#define T_ENTRACONF 40     //Tempo em d�cimos para entrar na configura��o
#define T_SAICONF   100     //Tempo em D�cimos para Sair da Configura��o   

#define SEMERRO     0       // N�o tem erro
#define ERROEND     1       // Erro no fim de curso
#define ERROESMAG   2       // Ocorreu um anti esmagamento ou sobre corrente
#define ERROCURTO   3       // Ocorreu curto circuito em algum acess�rio

/* Modifica Parametros pelas teclas */
class Parametro
{
public:
    
	Parametro();
    
    /* retorna true se esta no modo configura��o */
    bool onConf();
	
    /* Executar no loop do modo Conf */
    void Run();
    
    /* Executar loop do modo normal, na parte dos d�cimos de segundo, 
     * serve para ler as teclas e colocar no modo Conf */
    void Test();
    
    /* retorna true se a tecla Sel foi pressionada */
    bool EdgeSel();
    
    /* Retorna true se a tecla Enter Foi pressionada */
    bool EdgeEnter();
        
    /* Fun��o auxiliar para verificar se a entrada est� negativa (filtrado) */
    bool GetSignal(unsigned char signal); 
    
    void SetError(unsigned char erro);
};
#endif