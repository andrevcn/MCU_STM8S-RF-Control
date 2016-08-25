
#ifndef INCLUDE_PARAMETRO_H
#define INCLUDE_PARAMETRO_H

#define KEY_SEL     PA_IDR_bit.IDR1
#define KEY_ENTER   PA_IDR_bit.IDR2
#define LED         PD_ODR_bit.ODR0

#define T_ENTRACONF 40     //Tempo em décimos para entrar na configuração
#define T_SAICONF   100     //Tempo em Décimos para Sair da Configuração   

#define SEMERRO     0       // Não tem erro
#define ERROEND     1       // Erro no fim de curso
#define ERROESMAG   2       // Ocorreu um anti esmagamento ou sobre corrente
#define ERROCURTO   3       // Ocorreu curto circuito em algum acessório

/* Modifica Parametros pelas teclas */
class Parametro
{
public:
    
	Parametro();
    
    /* retorna true se esta no modo configuração */
    bool onConf();
	
    /* Executar no loop do modo Conf */
    void Run();
    
    /* Executar loop do modo normal, na parte dos décimos de segundo, 
     * serve para ler as teclas e colocar no modo Conf */
    void Test();
    
    /* retorna true se a tecla Sel foi pressionada */
    bool EdgeSel();
    
    /* Retorna true se a tecla Enter Foi pressionada */
    bool EdgeEnter();
        
    /* Função auxiliar para verificar se a entrada está negativa (filtrado) */
    bool GetSignal(unsigned char signal); 
    
    void SetError(unsigned char erro);
};
#endif