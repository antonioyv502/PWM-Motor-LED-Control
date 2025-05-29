# Controlling a DC Motor and LED Using PWM
In this lab we learned how to use a PWM signal to change the brightness of an LED(on board) and the speed of a DC motor. We also learned how to calculate the LOAD and CMP values for the PWM. We also used the switches on the board to increase the speed of the motor and to change the direction of the motor. 


## Pin Configuration Table

| Device     | Port.Pin     | Signal Type | PCTL | Direction | Drive Mode |
|------------|--------------|--------------|------|-----------|-------------|
| PWM_MOTOR  | PORTB.PIN5   | Digital      | GPIO | OUT/POS   |             |
| LED        | PORTF.PIN3   | Digital      | GPIO | OUT/POS   |             |
| SW1        | PORTF.PIN4   | Digital      | GPIO | IN/NEG    | UP          |
| SW2        | PORTF.PIN0   | Digital      | GPIO | IN/NEG    | UP          |
| DIRA       | PORTA.PIN2   | Digital      | GPIO | OUT       |             |
| DIRB       | PORTA.PIN3   | Digital      | GPIO | OUT       |             |
