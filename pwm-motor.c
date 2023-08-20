#include "pwm-motor.h"

void getDistance(void)
{

    /* pull trigger pin HIGH for 10 microsecond */

    /* pull trigger pin LOW*/

    // while (!ECHO_PIN_CHECK)
    //     ;    /* Waiting for Echo */
    // TR1 = 1; /* Timer Starts */
    // while (ECHO_PIN_CHECK && !TF1)
    //     ;
    // TR1 = 0; /* Stop the timer */
    // /* calculate distance using timer */
    // distance = ((TL1 | (TH1 << 8)) * Clock_period * sound_velocity) / 2;
} /* Waiting for Echo goes LOW */

/************************************************
 * Initiliaze the Board
 ************************************************/
void boardInit(void)
{
    TimerInit(); // Initialize timer to start generating interrupts

    P0 = 0xFF;
    P1 = 0xFF;
    P2 = 0xFF;
    P3 = 0xFF;

    autoflag = 0;
    sysTick = 0;
    ultrasonic_timer = 0;
    pwm_left = 0;
    pwm_right = 0;
    speedL = 0;
    speedR = 0;
}

/************************************************
 * Initiliaze the Timers
 ************************************************/
void TimerInit(void)
{
    // Set Timer 0 mode to 16-bit
    TMOD &= 0x00; // Clear lower 4 bits
    TMOD |= 0x11; // Set the first bit to configure Timer 0 as a 16-bit timer (Mode 1)

    // Set initial values for Timer 0
    TL0 = 0xFA; // Low byte initial value
    TH0 = 0xFF; // High byte initial value

    // Enable Timer 0 interrupt and global interrupt
    ET0 = 1; // Enable Timer 0 interrupt
    EA = 1;  // Enable global interrupts

    IT1 = 1; // Set INT1 to be edge-triggered (rising edge)
    EX1 = 1; // Enable External Interrupt 1

    // Start Timer 0
    TR0 = 1; // Start Timer 0 by setting its run control bit
}

/************************************************
 * Timer 0 Callback
 * Used to generate pwm signals
 ************************************************/
void InterruptTimer0() __interrupt(1)
{
    TR0 = 0; // Stop Timer 0

    TL0 = 0xFC; // Load low byte of timer value
    TH0 = 0xFF; // Load high byte of timer value

    pwm_left++;  // Increment pwm_left counter
    pwm_right++; // Increment pwm_right counter
    sysTick++;   // Increment sysTick counter

    if (pwm_left >= 100) // Check if pwm_left has reached 100
    {
        pwm_left = 0; // Reset pwm_left counter to 0
    }

    if (pwm_left <= speedL)
    {
        EN1_ON;
    }
    else
    {
        EN1_OFF;
    }

    if (pwm_right >= 100) // Check if pwm_right has reached 100
    {
        pwm_right = 0; // Reset pwm_right counter to 0
    }

    if (pwm_right <= speedR)
    {
        EN2_ON;
    }
    else
    {
        EN2_OFF;
    }
    if (ultrasonic_flag == 0)
    {
        TRIGGER_PIN_ON;
        ultrasonic_flag = 1;
    }

    if (sysTick - ultrasonic_timer >= 10 && ultrasonic_flag == 1) // 2HZ timer
    {
        ultrasonic_timer = sysTick;
        TRIGGER_PIN_OFF;
        ultrasonic_flag = 2;
    }

    if (ultrasonic_flag == 2 && ECHO_PIN_CHECK)
    {
        TR1 = 1;
        ultrasonic_flag == 3;
    }

    if (ultrasonic_flag == 3 && !ECHO_PIN_CHECK)
    {
        TR1 = 0;
        ultrasonic_flag == 0;
        distance = ((TL1 | (TH1 << 8)) * Clock_period * sound_velocity) / 2;
    }

    TR0 = 1; // Start Timer 0
}

/************************************************
 * External Interrupt 1 (INT1) handler
 * Used to handle ultrasonic callback
 ************************************************/
void External1_ISR() __interrupt(2)
{
}

/************************************************
 * Move the car forward
 * Minimum speed
 ************************************************/
void forward(void)
{
    speedL = 25;
    speedR = 25;
    MOTOR_L_GO;
    MOTOR_R_GO;
}

/************************************************
 * Stop the car
 ************************************************/
void stop(void)
{
    speedL = 0;
    speedR = 0;
    MOTOR_L_STOP;
    MOTOR_R_STOP;
}

/************************************************
 * SPEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEED
 ************************************************/
void nitro(void)
{
    speedL = 80;
    speedR = 80;
    MOTOR_L_GO;
    MOTOR_R_GO;
}

/************************************************
 * Turn the robot fast to the right or left
 ************************************************/
void turnFast(CAR_DIRECTION_t d)
{
    speedL = 60;
    speedR = 60;
    if (d == RIGHT)
    {
        MOTOR_L_BACK;
        MOTOR_R_GO;
    }
    else if (d == LEFT)
    {
        MOTOR_L_GO;
        MOTOR_R_BACK;
    }
}

/************************************************
 * Turn the robot slowly to the right or left
 ************************************************/
void turnSlow(CAR_DIRECTION_t d)
{

    if (d == RIGHT)
    {
        speedL = 30;
        speedR = 90;
        MOTOR_L_BACK;
        MOTOR_R_GO;
    }
    else if (d == LEFT)
    {
        speedL = 90;
        speedR = 30;
        MOTOR_L_GO;
        MOTOR_R_BACK;
    }
}

/************************************************
 * Delay for some milliseconds
 ************************************************/
void delayMs(unsigned int i)
{
    unsigned int j;
    while (i--)
    {
        for (j = 0; j < 125; j++)
            ;
    }
}

/************************************************
 * Check IR Sensors and toggle leds
 * Must be called in the main while loop
 ************************************************/
void checkSensor(void)
{
    if (IR1)
    {
        bitclear(P1, 0);
    }
    else
    {
        bitset(P1, 0);
    }
    if (IR2)
    {
        bitclear(P1, 1);
    }
    else
    {
        bitset(P1, 1);
    }
    if (IR3)
    {
        bitclear(P1, 2);
    }
    else
    {
        bitset(P1, 2);
    }
    if (IR4)
    {
        bitclear(P1, 3);
    }
    else
    {
        bitset(P1, 3);
    }
}

/************************************************
 * Main line following algorithem
 * Must be called in the main while loop
 ************************************************/
void lineFollow(void)
{
    if (!IR1 && !IR2 && !IR3 && !IR4) // 0000
    {
        forward();
    }

    if (!IR1 && !IR2 && IR3 && !IR4) // 0010
    {
        turnSlow(RIGHT);
    }

    if (!IR1 && !IR2 && !IR3 && IR4) // 0001
    {
        delayMs(6);

        do
        {
            turnFast(RIGHT);
        } while (!IR2);

        do
        {
            turnSlow(LEFT);
        } while (!IR3);
    }

    if (!IR1 && !IR2 && IR3 && IR4) // 0011
    {
        do
        {
            turnSlow(RIGHT);
        } while (!IR2);
    }

    // if (!IR1 && IR2 && !IR3 && IR4) // 0101
    // {
    //     delayMs(610);

    //     do
    //     {
    //         turnSlow(RIGHT);
    //     } while (!IR1);
    // }

    if (!IR1 && IR2 && !IR3 && !IR4) // 0100
    {
        turnSlow(LEFT);
    }

    if (IR1 && !IR2 && !IR3 && !IR4) // 1000
    {
        delayMs(6);

        do
        {
            turnFast(LEFT);
        } while (!IR3);

        do
        {
            turnSlow(RIGHT);
        } while (!IR2);
    }
    if (IR1 && IR2 && !IR3 && !IR4) // 1100
    {
        do
        {
            turnSlow(LEFT);
        } while (!IR3);
    }
    // if (IR1 && !IR2 && IR3 && !IR4) // 1010
    // {
    //     delayMs(100);

    //     do
    //     {
    //         turnSlow(LEFT);
    //     } while (!IR4);
    // }
    if (IR1 && IR2 && IR3 && IR4) // 1111
    {
        stop();
    }
}

/************************************************
 * Push Buttons controll
 * Must be called in the main while loop
 ************************************************/
void pushButton(void)
{
    if (!K1)
    {
        while (!K1)
            ;
        autoflag = 1;
    }
    if (!K2)
    {
        while (!K2)
            ;
        autoflag = 0;
    }
    // if (!K3)
    // {
    //     while (!K3)
    //         ;
    //     stop();
    //     autoflag = 0;
    // }
}

// void DisplayNumber()
// {
//     // char buf[4];
//     float n = 153.4;
//     P2_3 = 0;
//     P0 = num[1];
//     P2_3 = 1;
//     P2_0 = 0;
//     P0 = num[2];
//     P2_0 = 1;
//     P2_1 = 0;
//     P0 = num[3];
//     P2_1 = 1;
//     P2_2 = 0;
//     P0 = num[4];
//     P2_2 = 1;
// }

int main(void)
{
    boardInit();

    while (1)
    {
        if (autoflag)
            lineFollow();
        else
            stop();

        // if (!ultrasonic_flag)
        // {
        //     getDistance();
        // }

        if (distance <= 10)
        {
            // stop();
            bitclear(P1, 6);
        }
        else
        {
            bitset(P1, 6);
        }
        checkSensor();
        pushButton();
    }
}