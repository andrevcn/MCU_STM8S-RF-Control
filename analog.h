
#ifndef INCLUDE_ANALOG_H
#define INCLUDE_ANALOG_H

class Analog
{
    
protected:
    unsigned char valor[4];
public:
	Analog();
    
    /* Deve ser executado frequentemente, preenche o vetor valor com as convers�es */
	void Run(/*unsigned char estado*/);
    
    /* Le um valor j� lido do AD */
    unsigned char GetValue(unsigned char index);
};
#endif