#include <iostm8s003k3.h>
#include "terminal.h"

bool cmdFlag;
unsigned char index;
char serial_in[32];
char valor[] = "256\n\r";
char cmd[3][9];
char *serial_out;
char *init_mess = "\n\n\r(-)\n\r | lulios 0.05 #\n\r -\n\r.>";
char *cmd_err = "\n\rcomando desconecido\n\r.>";
char *prompt = ".>";

#pragma vector = UART1_T_TXE_vector
__interrupt void TxInterrupt() 
{
	UART1_SR  = 0;
	
	if(serial_out[index] == '\0' || index >= 32)
	{
		index = 0;
        UART1_CR2 = 0x24;
	}
	else
	{
		UART1_DR = serial_out[index];
		index ++;
	}
}
    
#pragma vector = UART1_R_RXNE_vector
__interrupt void RxInterrupt() 
{
	if(UART1_DR == '\r')
	{
		serial_in[index] = '\0';
		//serial_out = prompt;
        //FormatCmd();
        cmdFlag = true;
		index = 0;
        //UART1_CR2 = 0x48;
	}
	else
	{
		serial_in[index] = UART1_DR;
		index++;
        if(index > 32)
            index = 0;
	}
}

Terminal::Terminal ()
{	
    TX_PIN = 1;

	UART1_BRR2 = 0x07;
	UART1_BRR1 = 0x06;			//Set the baudrate to 19200 bps
	
    UART1_CR2 = 0x48;
	
	index = 0;
	
	serial_out = init_mess;
    cmdFlag = false;
		
}

void Terminal::Print(char *text)
{
	serial_out = text;
	index = 0;
    UART1_CR2 = 0x48;
}

void Terminal::PrintNumber(unsigned char number)
{
    unsigned char centena = 48, dezena = 48;
    
    while(number > 99)
    {
        centena++;
        number = number - 100;
    }
    
    while(number > 9)
    {
        dezena++;
        number = number - 10;
    }
    valor[0] = centena;
    valor[1] = dezena;
    valor[2] = number + 48;
    
 	serial_out = valor;
	index = 0;
    UART1_CR2 = 0x48;
}

bool Terminal::IsCmd()
{
    unsigned char serialPos;    //posição do serial_in
    unsigned char argPos;       //posição da string dentro do arg
    unsigned char argCount;     //0 = comando, 1 = arg1 e 2 = arg2
    if(cmdFlag)
    {
        cmdFlag = false;
        
        serialPos = 0;
        argPos = 0;
        argCount = 0;
        
        while(serial_in[serialPos] != '\0')
        {
            if(serial_in[serialPos] == ' ')
            {
                //argPos++;
                cmd[argCount][argPos] = '\0';
                argCount ++;
                if(argCount > 2) return false;
                argPos = 0;
                serialPos ++;
            }
            cmd[argCount][argPos] = serial_in[serialPos];
            serialPos ++;
            argPos++;
        }
        cmd[argCount][argPos] = '\0';
        return true;
    }
    else return false;
}

char *Terminal::GetCmd(unsigned char arg)
{
    return cmd[arg];
}

int Terminal::GetIntArg(unsigned char arg)
{
    unsigned char pos = 0;
    while(cmd[arg][pos] != '\0' && pos < 4)
    {
        if(cmd[arg][pos] < 48 || cmd[arg][pos] > 57) return -1;
        pos ++;
    }
    
    if(pos == 3)
        return (cmd[arg][2] - 48) + (cmd[arg][1] - 48) * 10 + (cmd[arg][0] - 48) * 100;
    
    else if(pos == 2)
        return (cmd[arg][1] - 48) + (cmd[arg][0] - 48) * 10;
    
    else if(pos == 1)
        return (cmd[arg][0] - 48);
    else
        return -1;
}


