#ifndef PWM_MOTOR_H
#define PWM_MOTOR_H

/************************************************
 * includes
 ************************************************/
#include <8051.h>

/************************************************
 * useful macros
 ************************************************/
#define bitset(byte, nbit) ((byte) |= (1 << (nbit)))
#define bitclear(byte, nbit) ((byte) &= ~(1 << (nbit)))
#define bitcheck(byte, nbit) ((byte) & (1 << (nbit)))
#define bitflip(byte, nbit) ((byte) ^= (1 << (nbit)))

/************************************************
 * GPIO PINS
 ************************************************/
// #define K4 P3_2   // button 1
// #define K3 P3_3   // button 2
#define K2 P3_4   // button 3
#define K1 P3_5   // button 4
#define BEEP P3_6 // buzzer

#define IN1 7
#define IN2 2
#define IN3 1
#define IN4 0
#define EN1 4
#define EN2 5

/************************************************
 * motor enable pins control
 ************************************************/
#define EN1_Check bitcheck(P2, EN1)
#define EN1_ON bitset(P2, EN1)
#define EN1_OFF bitclear(P2, EN1)

#define EN2_Check bitcheck(P2, EN2)
#define EN2_ON bitset(P2, EN2)
#define EN2_OFF bitclear(P2, EN2)

/************************************************
 * motor direction pins control
 ************************************************/
#define MOTOR_L_GO                          \
    {                                       \
        bitclear(P3, IN1), bitset(P3, IN2); \
    }

#define MOTOR_L_BACK                        \
    {                                       \
        bitset(P3, IN1), bitclear(P3, IN2); \
    }

#define MOTOR_R_GO                          \
    {                                       \
        bitclear(P3, IN3), bitset(P3, IN4); \
    }

#define MOTOR_R_BACK                        \
    {                                       \
        bitset(P3, IN3), bitclear(P3, IN4); \
    }

#define MOTOR_L_STOP                          \
    {                                         \
        bitclear(P3, IN1), bitclear(P3, IN2); \
    }

#define MOTOR_R_STOP                          \
    {                                         \
        bitclear(P3, IN3), bitclear(P3, IN4); \
    }

/************************************************
 * sensor pins
 ************************************************/
#define IR1 bitcheck(P0, 0) // ir sensor 1 is connected to pin P00
#define IR2 bitcheck(P0, 1) // ir sensor 2 is connected to pin P01
#define IR3 bitcheck(P0, 2) // ir sensor 3 is connected to pin P02
#define IR4 bitcheck(P0, 3) // ir sensor 4 is connected to pin P03

/************************************************
 * ultrasonic pins control
 *
 ************************************************/
#define TRIGGER_PIN_ON bitset(P2, 6)
#define TRIGGER_PIN_OFF bitclear(P2, 6)
#define ECHO_PIN_CHECK bitcheck(P3, 3)

/************************************************
 * Global variables
 ************************************************/
#define sound_velocity 34300 /* sound velocity in cm per second */
#define period_in_us 0.000001
#define Clock_period 1.085 * period_in_us /* period for clock cycle of 8051*/

unsigned char speedR; // It can have a value from 0 (0% duty cycle) to 100 (100% duty cycle)
unsigned char speedL; // It can have a value from 0 (0% duty cycle) to 100 (100% duty cycle)
float distance;

unsigned char pwm_left;
unsigned char pwm_right;
unsigned int sysTick;
unsigned int ultrasonic_timer;

unsigned int boost_timer;


unsigned short ultrasonic_flag;
unsigned short boost_flag;
unsigned short autoflag;

typedef enum
{
    LEFT,
    RIGHT
} CAR_DIRECTION_t;

typedef union
{
    unsigned char flags;
    struct
    {
        unsigned char sent_trigger : 1;
        unsigned char started_timer : 1;
        unsigned char finished_timer : 1;
        unsigned char bit4 : 1;
        unsigned char bit5 : 1;
        unsigned char bit6 : 1;
        unsigned char bit7 : 1;
        unsigned char bit8 : 1;
    } flag;
} sys_t;

sys_t sys;

unsigned char num[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82,
                       0xf8, 0x80, 0x90};

/************************************************
 * Function Prototypes
 ************************************************/
void getDistance(void);
void boardInit(void);
void TimerInit(void);
void forward(void);
void stop(void);
void nitro(void);
void turnFast(CAR_DIRECTION_t d);
void turnSlow(CAR_DIRECTION_t d);
void delayMs(unsigned int i);
void checkSensor(void);
void lineFollow(void);

#endif // PWM_MOTOR_H