#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "TM4C123GH6PM.h"
#include "MyDefines.h"  // Your Definitions Header File
#include "ez123G.h"
void Setup_PWM(void);
void Setup_GPIO(void);




int main()
{
    
    static bool preSw1 = true;
    static bool preSw2 = true;
    bool curSw1;
    bool curSw2;
    int cmpDuty[] = {0, 20000 , 35000 , 49999};  //0%, 20%, 40%, 70%, 100%
    int i = 0;    
    
    Setup_123G_80MHz();
    Setup_PWM();
    Setup_GPIO();
    
    GPIOA->DATA |= _PIN2;      //turn on PA2
    GPIOA->DATA &= ~_PIN3;  //turn off PA3
    
    while(1){
        curSw1 = GPIOF->DATA & _PIN4; //left button
        curSw2 = GPIOF->DATA & _PIN0; //right button
        
        if(preSw1 == true && curSw1 == false){  //detecting falling edge
            i++; //increase motor speed
            
          if(i > 3){ //checking if i is greater than 3
               i = 0; //go back to zero
            }
        }
        preSw1 = curSw1;  //updating previous state to current state

        if(preSw2 == true && curSw2 == false){  //detecting falling edge
        
            if(i != 0){
                PWM0->_1_CMPB = cmpDuty[0]; //set PWM0->_1_CMPB to 0
                ezDelay(2000);  // wait 2 seconds 
                
                GPIOA->DATA ^= _PIN2; //toggle to change direction 
                GPIOA->DATA ^= _PIN3; //toggle to change direction
                PWM0->_1_CMPB = cmpDuty[i]; 
            } 
        }
               
        preSw2 = curSw2;  //updating previous state to current state
        
        PWM1->_3_CMPB = cmpDuty[i];  //restore value(LED) 
        PWM0->_1_CMPB = cmpDuty[i];  //restore value(motor)
        ezDelay(20);  //wait 20ms
    }
}

//------------------------------------------------------------------------------
// GPIO for PWM: PB5(Motor)
    // M0PWM3 - PWM Module 0, generator 1, GENB, PWM3, PCTL = 0x4
    // SysClk = 80 MHz, PWMDIV = /4, Freq for PWMTimer = 80MHz /4 = 20MHz
    // PWM frequency = 400Hz 
    // LOAD = 50000
    // CMP = 0


// GPIO for PWM: PF3(LED)
    // M1PWM7 - PWM Module 1, generator 3, GENB, PWM7, PCTL = 0x5
    // SysClk = 80 MHz, PWMDIV = /4, Freq for PWMTimer = 80MHz /4 = 20MHz
    // PWM frequency = 400Hz
    // LOAD = 50000
    // CMP = 0
void Setup_PWM(void)
{
    // 1. Enable Clock for PWM Modules
    SYSCTL->RCGCPWM |= (_PWM_MODULE0 | _PWM_MODULE1);
    while((SYSCTL->PRPWM & (_PWM_MODULE0 | _PWM_MODULE1)) != (_PWM_MODULE0 | _PWM_MODULE1)){};
    // 2. Enable and Setup Clock Divider for PWM Timer
    SYSCTL->RCC |= (_PWM_123G_USEPWMDIV);                      
    SYSCTL->RCC &= ~(_PWM_123G_PWMDIV_MASK );                     
    SYSCTL->RCC |= (_PWMDIV_4 << _PWM_123G_PWMDIV_BIT);      //divisor 4
    // 3. Disable PWM Blocks and Setup the Timer Counting Mode
    PWM0->_1_CTL = 0x00;                            // Disable PWM Generator, and set to count-down mode
    PWM1->_3_CTL = 0x00;                            // Disable PWM Generator, and set to count-down mode
    // 4. Configure LOAD, CMP, GEN values
    PWM0->_1_LOAD = 50000;
    PWM0->_1_CMPB = 0;
    PWM0->_1_GENB = (_PWM_GEN_ACT_PWM_HIGH << _PWM_GEN_ACTCMPBD_BIT) |
                    (_PWM_GEN_ACT_PWM_LOW  << _PWM_GEN_ACTLOAD_BIT);    // Right-Aligned PWM

    PWM1->_3_LOAD = 50000 ;
    PWM1->_3_CMPB = 0;
    PWM1->_3_GENB = (_PWM_GEN_ACT_PWM_HIGH << _PWM_GEN_ACTCMPBD_BIT) |
                    (_PWM_GEN_ACT_PWM_LOW  << _PWM_GEN_ACTLOAD_BIT);    // Right-Aligned PWM

    // 5. Enable PWM Blocks
    PWM0->_1_CTL |= _PWM_BLOCK_ENABLE |  _PWM_COUNT_DOWN_MODE;     
    PWM1->_3_CTL |= _PWM_BLOCK_ENABLE |  _PWM_COUNT_DOWN_MODE;
    // 6. Enable PWM outputs on each PWM module
    PWM0->ENABLE  = _PWM3;
    PWM1->ENABLE  = _PWM7;
}
//------------------------------------------------------------------------------
void Setup_GPIO(void)
{
    // GPIO Initialization and Configuration
    // 1. Enable Clock to the GPIO Modules (SYSCTL->RCGCGPIO |= (_PORTs);)
    SYSCTL->RCGCGPIO |= (_PORTB | _PORTF | _PORTA);
    // allow time for clock to stabilize (SYSCTL->PRGPIO & (_PORTs))
    while((SYSCTL->PRGPIO & (_PORTB | _PORTF | _PORTA)) != (_PORTB | _PORTF | _PORTA)){};
    // 2. Unlock GPIO only PD7, PF0 on TM4C123G; PD7 on TM4C1294 (GPIOx->LOCK = 0x4C4F434B; and GPIOx->CR = _PINs;)
     GPIOF->LOCK = 0x4C4F434B;   // Unlock for GPIOF
     GPIOF->CR |= _PIN0;         // Commit for PIN0
     GPIOF->LOCK = 0;
    // 3. Set Analog Mode Select bits for each Port (GPIOx->AMSEL = _PINs;  0=digital, 1=analog)
    // 4. Set Port Control Register for each Port (GPIOx->PCTL = PMCn << _PTCL_PINn, check the PCTL table)
        GPIOB->PCTL = 0x4 << _PCTL_PIN5;
        GPIOF->PCTL = 0x5 << _PCTL_PIN3;

    // 5. Set Alternate Function Select bits for each Port (GPIOx->AFSEL = _PINs;  0=regular I/O, 1=PCTL peripheral)
        GPIOB->AFSEL = _PIN5;
        GPIOF->AFSEL = _PIN3;

    // 6. Set Output pins for each Port (Direction of the Pins: GPIOx->DIR = _PINs;  0=input, 1=output)
        GPIOA->DIR = _PIN2 | _PIN3;

    // 7. Set PUR bits (internal Pull-Up Resistor), PDR (Pull-Down Resistor), ODR (Open Drain) for each Port (0: disable, 1=enable)
        GPIOF->PUR = _PIN4 | _PIN0;

    // 8. Set Digital ENable register on all port.pins (GPIOx->DEN = _PINs; 0=disable, 1=enable)
        GPIOB->DEN = _PIN5;
        GPIOF->DEN = _PIN3 | _PIN4 | _PIN0 ; 
        GPIOA->DEN = _PIN2 | _PIN3;  
}
