#ifndef INCLUDE_HW_H
#define INCLUDE_HW_H

#include <inttypes.h>

#define LAMP        PB_ODR_bit.ODR7
#define ACEL        PB_ODR_bit.ODR6
#define BUZZER      PB_ODR_bit.ODR5

#define BUZZER_F    PD_IDR_bit.IDR7 
#define ACEL_VCC_F  PD_IDR_bit.IDR2 
#define LAMP_F      PB_IDR_bit.IDR3 

#define DESLIGA     0
#define LIGA        1

/**
  * @brief  EXTI Sensitivity values for PORTA to PORTE
  */
typedef enum {
  EXTI_SENSITIVITY_FALL_LOW  = (uint8_t)0x00, /*!< Interrupt on Falling edge and Low level */
  EXTI_SENSITIVITY_RISE_ONLY = (uint8_t)0x01, /*!< Interrupt on Rising edge only */
  EXTI_SENSITIVITY_FALL_ONLY = (uint8_t)0x02, /*!< Interrupt on Falling edge only */
  EXTI_SENSITIVITY_RISE_FALL = (uint8_t)0x03  /*!< Interrupt on Rising and Falling edges */
} EXTI_Sensitivity_TypeDef;

/**
  * @brief  EXTI Sensitivity values for TLI
  */
typedef enum {
  EXTI_TLISENSITIVITY_FALL_ONLY = (uint8_t)0x00, /*!< Top Level Interrupt on Falling edge only */
  EXTI_TLISENSITIVITY_RISE_ONLY = (uint8_t)0x04  /*!< Top Level Interrupt on Rising edge only */
} EXTI_TLISensitivity_TypeDef;

/**
  * @brief  EXTI PortNum possible values
  */
typedef enum {
  EXTI_PORT_GPIOA = (uint8_t)0x00, /*!< GPIO Port A */
  EXTI_PORT_GPIOB = (uint8_t)0x01, /*!< GPIO Port B */
  EXTI_PORT_GPIOC = (uint8_t)0x02, /*!< GPIO Port C */
  EXTI_PORT_GPIOD = (uint8_t)0x03, /*!< GPIO Port D */
  EXTI_PORT_GPIOE = (uint8_t)0x04  /*!< GPIO Port E */
} EXTI_Port_TypeDef;

/**
  * @brief  GPIO modes
  *
  * Bits definitions:
  * - Bit 7: 0 = INPUT mode
  *          1 = OUTPUT mode
  *          1 = PULL-UP (input) or PUSH-PULL (output)
  * - Bit 5: 0 = No external interrupt (input) or No slope control (output)
  *          1 = External interrupt (input) or Slow control enabled (output)
  * - Bit 4: 0 = Low level (output)
  *          1 = High level (output push-pull) or HI-Z (output open-drain)
  */
typedef enum
{
  GPIO_MODE_IN_FL_NO_IT      = (uint8_t)0x00,  /*!< Input floating, no external interrupt */
  GPIO_MODE_IN_PU_NO_IT      = (uint8_t)0x40,  /*!< Input pull-up, no external interrupt */
  GPIO_MODE_IN_FL_IT         = (uint8_t)0x20,  /*!< Input floating, external interrupt */
  GPIO_MODE_IN_PU_IT         = (uint8_t)0x60,  /*!< Input pull-up, external interrupt */
  GPIO_MODE_OUT_OD_LOW_FAST  = (uint8_t)0xA0,  /*!< Output open-drain, low level, 10MHz */
  GPIO_MODE_OUT_PP_LOW_FAST  = (uint8_t)0xE0,  /*!< Output push-pull, low level, 10MHz */
  GPIO_MODE_OUT_OD_LOW_SLOW  = (uint8_t)0x80,  /*!< Output open-drain, low level, 2MHz */
  GPIO_MODE_OUT_PP_LOW_SLOW  = (uint8_t)0xC0,  /*!< Output push-pull, low level, 2MHz */
  GPIO_MODE_OUT_OD_HIZ_FAST  = (uint8_t)0xB0,  /*!< Output open-drain, high-impedance level,10MHz */
  GPIO_MODE_OUT_PP_HIGH_FAST = (uint8_t)0xF0,  /*!< Output push-pull, high level, 10MHz */
  GPIO_MODE_OUT_OD_HIZ_SLOW  = (uint8_t)0x90,  /*!< Output open-drain, high-impedance level, 2MHz */
  GPIO_MODE_OUT_PP_HIGH_SLOW = (uint8_t)0xD0   /*!< Output push-pull, high level, 2MHz */
}GPIO_Mode_TypeDef;

/**
  * @brief  Definition of the GPIO pins. Used by the @ref GPIO_Init function in
  * order to select the pins to be initialized.
  */

typedef enum
{
  GPIO_PIN_0    = ((uint8_t)0x01),  /*!< Pin 0 selected */
  GPIO_PIN_1    = ((uint8_t)0x02),  /*!< Pin 1 selected */
  GPIO_PIN_2    = ((uint8_t)0x04),  /*!< Pin 2 selected */
  GPIO_PIN_3    = ((uint8_t)0x08),   /*!< Pin 3 selected */
  GPIO_PIN_4    = ((uint8_t)0x10),  /*!< Pin 4 selected */
  GPIO_PIN_5    = ((uint8_t)0x20),  /*!< Pin 5 selected */
  GPIO_PIN_6    = ((uint8_t)0x40),  /*!< Pin 6 selected */
  GPIO_PIN_7    = ((uint8_t)0x80),  /*!< Pin 7 selected */
  GPIO_PIN_LNIB = ((uint8_t)0x0F),  /*!< Low nibble pins selected */
  GPIO_PIN_HNIB = ((uint8_t)0xF0),  /*!< High nibble pins selected */
  GPIO_PIN_ALL  = ((uint8_t)0xFF)   /*!< All pins selected */
}GPIO_Pin_TypeDef;

/** TIM4 Prescaler */
typedef enum
{
  TIM4_PRESCALER_1     = ((uint8_t)0x00),
  TIM4_PRESCALER_2     = ((uint8_t)0x01),
  TIM4_PRESCALER_4     = ((uint8_t)0x02),
  TIM4_PRESCALER_8     = ((uint8_t)0x03),
  TIM4_PRESCALER_16    = ((uint8_t)0x04),
  TIM4_PRESCALER_32    = ((uint8_t)0x05),
  TIM4_PRESCALER_64    = ((uint8_t)0x06),
  TIM4_PRESCALER_128   = ((uint8_t)0x07)
} TIM4_Prescaler_TypeDef;



class Hw
{
public:
	Hw();
    //void Motor(unsigned char dir);
    
    /* Liga e desliga o Buzzer */
    void Buzzer(bool estado);
    
    /* função Beeps, tempos em décimo de segundos */
    void Beeps(unsigned char tON, unsigned char tOFF, unsigned char vezes);
    
    /* Necessário executar no main uma vez a cada décimo de segundo */
    void BeepRun();
    
    /* Liga ou desliga o acelerador positivo e o negativo */
    void Acelerador(bool estado);
    
    /* Liga ou desliga a Lâmpada */
    void Lampada(bool estado);
    
    /* Verifica curto no Buzzer, Lampada e Acelerador */
    bool CheckCurtoAcc(); 
    
    void EXTI_SetExtIntSensitivity(uint8_t Port, EXTI_Sensitivity_TypeDef SensitivityValue);
    
    void EXTI_SetTLISensitivity(EXTI_TLISensitivity_TypeDef SensitivityValue);
    
    void InitialiseSystemClock(void);
    
    void Cfg_LED_Verde();
    
    void Cfg_PD6_ExternalInterrupt();
};
#endif