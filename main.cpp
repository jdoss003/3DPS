/*
 * FinalProjct.c
 *
 * Created: 5/10/2018 7:01:04 PM
 * Author : Justin
 */

#include "defs.h"

#include "TEST_CUBE_GCODE"
char *TEST_GCODE = TEST_CUBE_GCODE;
char *error_msg = "";

enum system_states { SYS_START, SYS_WAITING, SYS_RUNNING, SYS_FALURE } system_state;

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

void keepAlive()
{
	Extruder::checkTemp();
}

unsigned char getSysState()
{
	return system_state;
}

void LCDTick(_task *task)
{
    float curTemp = Extruder::getTemp();
    char string[] = "Temp:        ";
    dtostrf(curTemp, 6, 2, (&string[0] + 6));
	string[12] = 'C';

    //LCD_ClearScreen();
    LCD_DisplayString(1, (unsigned char *) &string[0]);

    char *status;

    switch (system_state)
    {
        case SYS_START:
            status = "Initializing...";
            break;
        case SYS_WAITING:
            status = "Ready";
            break;
        case SYS_RUNNING:
            status = "Printing...";
            break;
        case SYS_FALURE:
            status = error_msg;
            break;
    }

	if (system_state == SYS_FALURE)
		LCD_DisplayString(17, (const unsigned char*)status);
}

void systemFailure(char *msg)
{
	Extruder::setTemp(0.0);
	SETPIN(EXTRUDER_PIN, LOW);
	system_state = SYS_FALURE;
	MovController::stopAllMoves();
	error_msg = msg;
	LCDTick(0);

	while (1) { keepAlive(); } // stay here until reset
}

void mainLoop()
{
    SETPIN(READY_LIGHT, HIGH);
    system_state = SYS_WAITING;

    while (!GETPIN(START_BTN, 1)) { keepAlive(); }
    SETPIN(READY_LIGHT, LOW);

    system_state = SYS_RUNNING;
    GCode::executeFString((char*)TEST_GCODE);
}

int main()
{
    system_state = SYS_START;

	INITPIN(START_BTN, OUTPUT, HIGH);
    INITPIN(PB_0, OUTPUT, LOW);
    INITPIN(PB_1, OUTPUT, LOW);

    INITPIN(PD_0, OUTPUT, LOW);
    INITPIN(PD_1, OUTPUT, LOW);
    INITPIN(PD_2, OUTPUT, LOW);
    INITPIN(PD_3, OUTPUT, LOW);
    INITPIN(PD_4, OUTPUT, LOW);
    INITPIN(PD_5, OUTPUT, LOW);
    INITPIN(PD_6, OUTPUT, LOW);
    INITPIN(PD_7, OUTPUT, LOW);

     _task LCD_task;
     LCD_init();
     LCD_task.state = -1;
     LCD_task.period = 1000 / TICK_PERIOD; // 1 second
     LCD_task.elapsedTime = LCD_task.period;
     LCD_task.TickFct = &LCDTick;

     addTask(&LCD_task);

     initMovScheduler();

     unsigned char i;
     for (i = 0; i < 4; ++i)
     {
         MovController::getMovController((_axis) i)->init((_axis) i);
     }

     Extruder::init();

     TimerSet(TICK_PERIOD);
     TimerOn();

    while (1) { mainLoop(); }
}
