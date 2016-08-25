#ifndef INCLUDE_TERMINAL_H
#define INCLUDE_TERMINAL_H

#define TX_PIN PD_ODR_bit.ODR5 
#define RX_PIN PB_IDR_bit.IDR7 

class Terminal
{
public:
    
	Terminal();
    
    /* envia pela Serial um texto */
    void Print(char *text);
    
    /* Transforma um numero em texto e envia pela Serial */
    void PrintNumber(unsigned char number);
    
    /* Retorna true se algum comando foi digitado no terminal */
    bool IsCmd();
    
    /* Retorna o texto do comando   *
     * GetCmd(0) pega o comando,    *
     * GetCmd(1) pega argumento 1   *
     * e GetCmd(2) pega argumento 2 */
    char *GetCmd(unsigned char arg);
    
    /* Pega o argumento valor inteiro de um argumento */
    int GetIntArg(unsigned char arg);   

};
#endif