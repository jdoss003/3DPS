/*
 * FinalProjct.c
 *
 * Created: 5/10/2018 7:01:04 PM
 * Author : Justin
 */

#include <stdlib.h>
#include "io.c"
#include "defs.h"

_task LCD_task, wait_task;
unsigned char waitTemp = 0;

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1;         // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0;   // Current internal count of 1ms ticks

void TimerOn()
{
    // AVR timer/counter controller register TCCR1
    TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
    // bit2bit1bit0=011: pre-scaler /64
    // 00001011: 0x0B
    // SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
    // Thus, TCNT1 register will count at 125,000 ticks/s
    // AVR output compare register OCR1A.
    OCR1A = 125;    // Timer interrupt will be generated when TCNT1==OCR1A
    // We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
    // So when TCNT1 register equals 125,
    // 1 ms has passed. Thus, we compare to 125.
    // AVR timer interrupt mask register
    TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

    //Initialize avr counter
    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;
    // TimerISR will be called every _avr_timer_cntcurr milliseconds

    //Enable global interrupts
    sei(); // 0x80: 1000000
}

void TimerOff()
{
    TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR()
{
    tickTasks();
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect)
{
    // CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
    _avr_timer_cntcurr--;               // Count down to 0 rather than up to TOP
    if (_avr_timer_cntcurr == 0)
    {                                   // results in a more efficient compare
        TimerISR();                     // Call the ISR that the user uses
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M)
{
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

void WaitTick(_task *task)
{
    if (!waitTemp && ((Extruder::getTemp() - 150 < 2) || (150 - Extruder::getTemp() < 2)))
    {
        waitTemp = 1;
    }
    else if ((Extruder::getTemp() - 150 < 2) || (150 - Extruder::getTemp() < 2))
    {
        task->state = 1;
    }
}

void LCDTick(_task *task)
{
    char string[] = "Temp:       ";
    dtostrf(Extruder::getTemp(), 6, 2, (&string[0] + 6));

    LCD_ClearScreen();
    LCD_DisplayString(1, (unsigned char *) &string[0]);
}

void TempTest()
{
    LCD_init();
    LCD_task.state = -1;
    LCD_task.period = 1000 / TICK_PERIOD; // 1 second
    LCD_task.elapsedTime = 0;
    LCD_task.TickFct = &LCDTick;

    addTask(&LCD_task);

    wait_task.state = 0;
    wait_task.period = 1000 / TICK_PERIOD * 60 * 3; // 1 sec * 60 * 3 = 3 min
    wait_task.elapsedTime = 0;
    wait_task.TickFct = &WaitTick;

    addTask(&wait_task);

    Extruder::setTemp(150);

    while (!wait_task.state) { keepAlive(); }

    Extruder::setTemp(0);
}

int main(void)
{
    DDRB = 0x03;
    DDRD = 0xFF;
    unsigned char i;
    for (i = 0; i < 4; ++i)
    {
        //getMovController((_axis) i)->init((_axis) i);
    }

    Extruder::init();

    INITPIN(PB_0, OUTPUT, LOW); // TODO remove

    TimerSet(TICK_PERIOD);
    TimerOn();

    TempTest();
    while (1) {}
}
