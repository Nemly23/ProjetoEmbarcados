//============================================================================
// Name        : TesteBlackLib.cpp
// Author      : Ricardo
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h>
#include <chrono>
#include "BlackLib/BlackPWM/BlackPWM.h"
#include "BlackLib/BlackGPIO/BlackGPIO.h"
using namespace BlackLib;
using namespace std;

/* periodo pwm (us) */
#define PeriodTimeMicro 1000

BlackPWM 	motorR(P8_13);
BlackPWM 	motorL(P8_19);

/* sensor US */
BlackGPIO  	Echo(GPIO_38, input, SecureMode);
BlackGPIO  	Trig(GPIO_34, output, SecureMode);

/* saida para ponte h */
BlackGPIO  	IN1(GPIO_36, output, SecureMode); // dir
BlackGPIO  	IN2(GPIO_47, output, SecureMode);
BlackGPIO  	IN3(GPIO_27, output, SecureMode); // esq
BlackGPIO  	IN4(GPIO_62, output, SecureMode);

double dis;

/* inicializacao pwm e portas */
void setup_PWM(){
	motorR.setDutyPercent(100.0);
	motorR.setPeriodTime(PeriodTimeMicro, microsecond);
	motorR.setDutyPercent(0);
	motorR.setPolarity(straight);
	motorL.setDutyPercent(100.0);
	motorL.setPeriodTime(PeriodTimeMicro, microsecond);
	motorL.setDutyPercent(0);
	motorL.setPolarity(straight);
}

void setup_GPIO(){
	IN1.setValue(low);
	IN2.setValue(low);
	IN3.setValue(low);
	IN4.setValue(low);
	Trig.setValue(low);
	Echo.setValue(low);
}


void go_straight_right(float speed){
	motorR.setDutyPercent(speed);
	IN1.setValue(high);
	IN2.setValue(low);
}

void go_reverce_right(float speed){
	motorR.setDutyPercent(speed);
	IN1.setValue(low);
	IN2.setValue(high);
}

void go_straight_left(float speed){
	motorL.setDutyPercent(speed);
	IN3.setValue(high);
	IN4.setValue(low);
}

void go_reverce_left(float speed){
	motorR.setDutyPercent(speed);
	IN3.setValue(low);
	IN4.setValue(high);
}

void turn_on_pwm(){
	motorR.setRunState(run);
	motorL.setRunState(run);
}

void turn_off_pwm(){
	motorR.setRunState(stop);
	motorL.setRunState(stop);
}

void send_pulse_ultrasound(){
	chrono::duration<double> elapsed;
	Trig.setValue(low);
	usleep(2);
	Trig.setValue(high);
	usleep(10);
	Trig.setValue(low);
	while(!Echo.isHigh()){}
	auto start = chrono::high_resolution_clock::now();
	while(Echo.isHigh()){}
	auto finish = chrono::high_resolution_clock::now();
	elapsed = finish - start;
	dis = elapsed.count()*346.3/2.0;
}

int main()
{
	setup_PWM();
	float speed=0;

	go_straight_right(50);
	turn_on_pwm();
	sleep(1);
	turn_off_pwm();
	sleep(1);
	go_straight_left(50);
	turn_on_pwm();
	sleep(1);
	turn_off_pwm();
	sleep(1);
	go_straight_left(20);
	turn_on_pwm();
	sleep(1);
	turn_off_pwm();
	sleep(1);
	cout << "oi testando";

	//if (!motorE.isRunning())

  	//std::cout << "Pwm period time: " << motorE.getPeriodValue() << " nanoseconds \n";
    return 0;
}
