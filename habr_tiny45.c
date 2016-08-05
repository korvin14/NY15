#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define r_pwm(pwm) OCR0A = pwm 
#define b_pwm(pwm) OCR0B = pwm
#define g_pwm(pwm) OCR1A = pwm

#define NUM_MODES 16

int main(void)
{
    /* Init LED GPIO pins */
    DDRB = 7; /* pins 0, 1, 2 */

    /* Init PWM timers */
    /* Timer0 is for R and G channels */
    TCCR0A = (1<<COM0A1)|(1<<COM0B1)|(WGM01)|(WGM00);
    TCCR0B = (1<<CS00); /* divide by 1 */

    /* Timer 1 is for B channel; enable interrupts */
    TIMSK = (1<<OCIE1A)|(1<<TOIE1);
    TCCR1 = (1<<CS10); /* divide by 1 */

    sei();

    /* Get mode value from the EEPROM and update it */
    uint8_t mode = eeprom_read_byte((uint8_t *) 1);
    mode++;
    if (mode >= NUM_MODES)
        mode = 0;
    eeprom_write_byte((uint8_t *) 1, mode);

    uint8_t i;
    while(1) {
        switch(mode >> 1) {
            case 0:
                r_pwm(255);
                break;
            case 1:
                g_pwm(255);
                break;
            case 2:
                b_pwm(255);
                break;
            case 3:
                r_pwm(255);
                g_pwm(255);
                break;
            case 4:
                g_pwm(255);
                b_pwm(255);
                break;
            case 5:
                r_pwm(255);
                b_pwm(255);
                break;
            case 6:
                r_pwm(255);
                g_pwm(255);
                b_pwm(255);
                break;
            case 7:
                for (i=0; i < 255; i++) {
                    r_pwm(255-i); g_pwm(i);
                    _delay_ms(20);
                }
                for (i=0; i < 255; i++) {
                    g_pwm(255-i); b_pwm(i);
                    _delay_ms(20);
                }
                for (i=0; i < 255; i++) {
                    b_pwm(255-i); r_pwm(i);
                    _delay_ms(20);
                }
        }
    }

    return 0;
}

ISR(TIM1_OVF_vect)
{
    if (OCR1A != 0)
        PORTB |= (1<<2);
}

ISR(TIM1_COMPA_vect)
{
    PORTB &= ~(1<<2);
}