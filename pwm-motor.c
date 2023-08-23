#include "pwm-motor.h"

void sonic(void)
{
    if (scount >= 750)
    {
        P1_4 ^= 1;
        if (ultrasonic_flag == 0)
        {
            ultrasonic_flag = 1;
            P2_6 = 1; // 800 ms starts module once
            time = (TH0 << 8) + TL0;
        }
        else if (ultrasonic_flag == 1)
        {
            if (((TH0 << 8) + TL0) - time > 10)
            {
                ultrasonic_flag = 2;
                P2_6 = 0;
            }
        }
        else if (ultrasonic_flag == 2 && !ECHO_PIN_CHECK)
        {
            ultrasonic_flag = 3;
            // P1_2 = 0;
        }
        else if (ultrasonic_flag == 3 && ECHO_PIN_CHECK)
        {
            ultrasonic_flag = 4;
            lcount = scount;
        }
        else if (ultrasonic_flag == 4 && !ECHO_PIN_CHECK)
        {
            ultrasonic_flag = 0;
            scount = 0;
            P1 = 0x00;
            ultrasonic_enable_flag = 0;
            TR0 = 0;
            // autoflag = 0;
            // stop();
            evade_flag = 1;
        }
        else if (ultrasonic_flag == 4 && scount - lcount > 2 && ECHO_PIN_CHECK)
        {
            ultrasonic_flag = 0;
            scount = 0;
            P1_5 = 1;
        }
    }
}

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
    ultrasonic_flag = 0;
    ultrasonic_enable_flag = 0;
    boost_enable_flag = 1;
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

    TMOD = 0x11;
    TH0 = 0xFE;
    TL0 = 0xD3;

    TH1 = 0xFF;
    TL1 = 0xF0;

    // Enable Timer 0 interrupt and global interrupt
    EA = 1;  // Enable global interrupts
    ET0 = 1; // Enable Timer 0 interrupt
    ET1 = 1;

    // Start Timer 0
    TR0 = 1; // Start Timer 0 by setting its run control bit
    TR1 = 1; // Start Timer 0 by setting its run control bit
}

/************************************************
 * Timer 0 Callback
 * Used to generate pwm signals
 ************************************************/
void InterruptTimer0() __interrupt(1)
{
    // TR0 = 0; // Stop Timer 0
    TH0 = 0xFE;
    TL0 = 0xD3;

    // pwm_left++;  // Increment pwm_left counter
    // pwm_right++; // Increment pwm_right counter
    // sysTick++;   // Increment sysTick counter
    scount++;

    // TR0 = 1; // Resume Timer 0
}

/************************************************
 * Timer 1 Callback
 * Used to generate pwm signals
 ************************************************/
void InterruptTimer1() __interrupt(3)
{
    TR1 = 0; // Stop Timer 0
    TH1 = 0xFF;
    TL1 = 0xE0;

    pwm_left++;  // Increment pwm_left counter
    pwm_right++; // Increment pwm_right counter
    // sysTick++;   // Increment sysTick counter
    // scount++;

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
    TR1 = 1; // Resume Timer 0
}

/************************************************
 * Move the car forward
 * Minimum speed
 ************************************************/
void forward(void)
{
    if (boost_flag)
    {
        speedL = 45;
        speedR = 50;
    }
    else
    {
        speedL = 23;
        speedR = 23;
    }

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
        speedR = 80;
        MOTOR_L_BACK;
        MOTOR_R_GO;
    }
    else if (d == LEFT)
    {
        speedL = 80;
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
        delayMs(50);

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

    if (!IR1 && IR2 && !IR3 && !IR4) // 0100
    {
        turnSlow(LEFT);
    }

    if (IR1 && !IR2 && !IR3 && !IR4) // 1000
    {
        delayMs(50);

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
        autoflag = 1;

        while (!K1)
            ;
        nitro();
        delayMs(200);
        ultrasonic_enable_flag = 1;
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

/************************************************
 * Push Buttons controll
 * Must be called in the main while loop
 ************************************************/
void evade(void)
{
    P1 = 0x00;
    autoflag = 0;
    // stop();
    // delayMs(2);
    //
    speedR = 100;
    speedL = 20;

    MOTOR_R_BACK;
    MOTOR_L_GO;
    delayMs(295);

    speedR = 100;
    speedL = 100;

    MOTOR_L_BACK;
    MOTOR_R_BACK;
    delayMs(100);

    speedR = 100;
    speedL = 20;

    while (!IR3 && !IR4)
    {
        MOTOR_R_BACK;
        MOTOR_L_GO;
    }

    autoflag = 1;
    P1 = 0xFF;
}

int main(void)
{
    boardInit();
    while (1)
    {
        if (autoflag)
            lineFollow();

        if (ultrasonic_enable_flag)
            sonic();
        if (evade_flag)
        {
            evade();
            evade_flag = 0;
        }

        checkSensor();
        pushButton();
    }
}