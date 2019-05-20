//============================================================================
// Name        : Testev3.cpp
// Author      : Ricardo
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "BlackLib/BlackPWM/BlackPWM.h"
#include "BlackLib/BlackGPIO/BlackGPIO.h"
#include "BlackLib/BlackThread/BlackThread.h"
#include "BlackLib/BlackI2C/BlackI2C.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <syslog.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DELAY 5*1000*100

using namespace BlackLib;
using namespace std;

#define PeriodTimeMicro 1000
//#define addr 0x1E

double distan=0;

 class Task1 : public BlackThread
	{
		public:
			 void onStartHandler()
			 {
				 int i=0;
				 int n=0;
				 int n2=0;
				 int fd;
				 int value=0;
				 int ret=1;
				 char ch;
				 BlackGPIO  Trig(GPIO_45, output, FastMode);
				 BlackGPIO  Echo(GPIO_44, input, FastMode);
				 chrono::duration<double> elapsed;
				 while(i<100){
					n=0;
					n2=0;
					value=0;
					Trig.setValue(low);
					usleep(2);
					Trig.setValue(high);
					//usleep(10);
					Trig.setValue(low);
					while(value==0 && n2<100){
						if ((fd = open("/sys/class/gpio/gpio44/value", O_RDONLY | O_NONBLOCK)) < 0) {
							cout << "erro_open" << endl;
							n++;
							continue;
						}
						ret = read(fd, &ch, sizeof(ch));
						if (ret<0){
							n++;
							cout << "erro_read" << endl;
							continue;
						}
						if (ch != '0') {
							value = 1;
						} else {
							value = 0;
						}
						n2++;
						close(fd);
					}
					//cout << n2 << endl;
					auto start = chrono::high_resolution_clock::now();
					while(value==1 && n<100){
						if ((fd = open("/sys/class/gpio/gpio44/value", O_RDONLY | O_NONBLOCK)) < 0) {
							cout << "erro_open" << endl;
							n++;
							continue;
						}
						ret = read(fd, &ch, sizeof(ch));
						if (ret<0){
							n++;
							cout << "erro_read" << endl;
							continue;
						}
						if (ch != '0') {
							value = 1;
						} else {
							value = 0;
						}
						n++;
						close(fd);
					}
					auto finish = chrono::high_resolution_clock::now();
					elapsed = finish - start;
					//distan = elapsed.count()*346.3/2.0;
					//cout << elapsed.count() << "s" << endl;
					//cout << distan << "m" << endl;
					//cout << value << endl;
					usleep(100000);
					i++;
				}
			 }
			 void onStopHandler(){
			 }
	};



int main(){
		//char c;
		int i=0;
		BlackPWM 	motorL(P8_13);
		BlackPWM 	motorR(P8_19);
		BlackGPIO  N1(GPIO_65, output);
		BlackGPIO  N2(GPIO_27, output);
		BlackGPIO  N3(GPIO_47, output);
		BlackGPIO  N4(GPIO_46, output);
		//BlackPWM trig(P9_14);
		cout << "teste" << endl;
		//trig.setRunState(stop);
		//trig.setDutyPercent(100.0);
		//trig.setPeriodTime(100, milisecond);
		//trig.setPolarity(straight);

		Task1 *t1 = new Task1();

	//Opening Bus
	int file;
	char *filename = "/dev/i2c-2";
	if ((file = open(filename, O_RDWR)) < 0) {
		//opening i2c port of BeagleBone Black
		perror("Failed to open the i2c bus");
		exit(1);
	}//end if

	//Initiating Comms
	int addr = 0x1E; //I2C address of HMC5883L
	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		//Identifying HMC5883L device on i2c line
		printf("Failed to acquire buss access and/or talk to slave. \n");
		exit(1);
	}//end if


	/*Writing to HMC5883L*/
	//Device register to access
	char read_buffer[13] = {0};	//Stores vales read from registers (13 registers)
	char rd_wr_buffer[2] = {0};	//Data to be written to device. 2 bytes (port, value)
	char reg_buffer[1] = {0};	//Value of first register
	unsigned char register_value = 0x00; //Value of first register

	/*Writing address to read from*/
	//Configuration Register A
	rd_wr_buffer[0] = 0x00;	//Register 0x00
	rd_wr_buffer[1] = 0x70;	//Configuration value (read DataSheet)

	puts("Setting Registers A, B, Mode");
	if (write(file, rd_wr_buffer, 2) != 2) {
		//Write 1 byte from reg_buffer to HMC5883L and check it was written
		printf("Failed to write to I2C bus.\n\n");
	}//end if
	else
		printf("0x%d to 0x%d\n", rd_wr_buffer[0], rd_wr_buffer[1]);

	//Configuration Register B
	rd_wr_buffer[0] = 0x01;	//Register 0x01
	rd_wr_buffer[1] = 0xA0;	//Configuration value (read DataSheet)
	if (write(file, rd_wr_buffer, 2) != 2) {
		//Write 1 byte from reg_buffer to HMC5883L and check it was written
		printf("Failed to write to I2C bus.\n\n");
	}//end if
	else
		printf("0x%d to 0x%d\n", rd_wr_buffer[0], rd_wr_buffer[1]);

	//Mode Register
	rd_wr_buffer[0] = 0x02;	//Register 0x02
	rd_wr_buffer[1] = 0x00;	//Configuration value (read DataSheet)
	if (write(file, rd_wr_buffer, 2) != 2) {
		//Write 1 byte from reg_buffer to HMC5883L and check it was written
		printf("Failed to write to I2C bus.\n\n");
	}//end if
	else
		printf("0x%d to 0x%d\n", rd_wr_buffer[0], rd_wr_buffer[1]);

	usleep(6*1000); //sleep 6 ms


	/*Reading from HMC5883L*/
	printf("\n500 ms Delay in Readings.\n");
	puts("-------------------------");
	printf(" X  | Z |  Y \n");
	//Create short (2 bytes) for concatenating MSB and LSB
	short value = 0;	//concatenated calue
	int count = 0;		//Counter for alternating shifts


	//***** Main Loop *****//
	while (1) {

		//Reset To register 0x03
		reg_buffer[0] = register_value;	//Reset Register Ptr to first register
		if (write(file, reg_buffer, 1) != 1) {
			//Write 1 byte from reg_buffer to HMC5883L and check it was written
			printf("Failed to write to I2C bus.\n\n");
		}//end if

		//Using I2C read
		if (read(file, read_buffer, 13) != 13) {
			//Read 13 bytes into "read_buffer" then check 13 bytes were read
			//Internal HMC5883L Addr Ptr automatically incremented after every read
			printf("Failed to read from the I2C bus: %s.\n", strerror(errno));
			printf("1. Check if wired to correct pins\n");
			printf("2. Check if the pins are set properly\n\n");
		}//end if
		else {
			//printf("Looks like the I2C bus is operational! \n");
			for (int j=3; j<9; j++) {
				value = value | read_buffer[j];	//OR read_buffer into lower byte
				if (count%2 == 0)	//Shift 8 bits every other loop
					value = value << 8;
				else {
					printf("%d ",value);
					value = 0;
				}//end if
				count++;
			}// end for

			count = 0;
		}//end else

		printf("\n");
		usleep(DELAY); //500 ms
	}//end while()


		//t1->run();
		/*
		BlackI2C  myI2c(I2C_1, 0x1E);
		myI2c.open( ReadWrite|NonBlock );
		uint8_t who_am_i = myI2c.readByte(0x00);
		std::cout << "Mag: " << std::hex << (int)who_am_i << std::endl;
		bool resultOfWrite          = myI2c.writeByte(0x02, 0x00);
		std::cout << "Power Ctrl's new value is wrote?: " << std::boolalpha << resultOfWrite << std::dec << std::endl;
		uint16_t out_y = myI2c.readWord(0x07);
		std::cout << "Mag: " << std::hex << (int)who_am_i << std::endl;
		std::cout << "out_y: " << out_y << std::endl;
		*/
		/*
		//int i=0;
		 int n=0;
		 int fd;
		 int value;
		 int ret=1;
		 char ch;
		 BlackGPIO  Trig(GPIO_45, output, SecureMode);
		 BlackGPIO  Echo(GPIO_44, input, SecureMode);
		 chrono::duration<double> elapsed;
		 while(i<50){
			n=0;
			value=1;
			Trig.setValue(low);
			usleep(2);
			Trig.setValue(high);
			usleep(10);
			Trig.setValue(low);
			auto start = chrono::high_resolution_clock::now();
			while(value==1 && n<100){
				if ((fd = open("/sys/class/gpio/gpio44/value", O_RDONLY | O_NONBLOCK)) < 0) {
					return 0;
				}
				ret = read(fd, &ch, sizeof(ch));
				if (ret<0){
					n++;
					cout << "erro_read" << endl;
					continue;
				}
				if (ch != '0') {
					value = 1;
				} else {
					value = 0;
				}
				n++;
				close(fd);
			}
			auto finish = chrono::high_resolution_clock::now();
			elapsed = finish - start;
			dis = elapsed.count()*346.3/2.0;
			cout << dis << "m" << endl;
			cout << value << endl;
			usleep(100000);
			i++;
		}
		 */
		/*
		motorL.setRunState(stop);
		motorL.setDutyPercent(100.0);
		motorL.setPeriodTime(PeriodTimeMicro, microsecond);
		motorL.setPolarity(straight);
		motorR.setRunState(stop);
		motorR.setDutyPercent(100.0);
		motorR.setPeriodTime(PeriodTimeMicro, microsecond);
		motorR.setPolarity(straight);

		std::cout << motorL.getPeriodValue() << "nanosecond" << endl;
		std::cout << motorL.getDutyValue() << " duty" << endl;
		std::cout << motorL.getValue() << "Teste" << endl;
		std::cout << motorR.getPeriodValue() << "nanosecond" << endl;
		std::cout << motorR.getDutyValue() << " duty" << endl;

		motorR.setRunState(run);
		while(i<100){
			if (dis<0.05){
				motorR.setDutyPercent(0.0);
				std::cout << "Devagar" << endl;
			}
			else if (dis<0.10){
				motorR.setDutyPercent(30.0);
				std::cout << "Menos Devagar" << endl;
			}
			else if (dis<0.30){
				motorR.setDutyPercent(60.0);
				std::cout << "Mais rapido" << endl;
			}
			else{
				motorR.setDutyPercent(100.0);
				std::cout << "Rapido" << endl;
			}
			usleep(100000);
			i++;
		}

		motorR.setRunState(stop);

		/*
		while(1){
			cout  << Echo.getValue() << " high" << endl;
			Trig.setValue(high);
			sleep(3);
			cout  << Echo.getValue() << " low" << endl;
			Trig.setValue(low);
			sleep(3);
		}
		*/
		/*
		Trig.setValue(high);
		cout << Echo.getValue() << endl;
		sleep(2);
		Trig.setValue(low);
		cout << Echo.getValue() << endl;
		sleep(2);
		Trig.setValue(high);
		cout << Echo.getValue() << endl;
		sleep(2);
		Trig.setValue(low);
		cout << Echo.getValue() << endl;
		sleep(2);
		Trig.setValue(high);
		cout << Echo.getValue() << endl;
		sleep(2);
		Trig.setValue(low);
		cout << Echo.getValue() << endl;
		sleep(2);
		*/
		/*
			chrono::duration<double> elapsed;
			sleep(1);
		while(i<100){
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
			cout << dis << "m" << endl;
			usleep(100000);
			i++;
		}
		*/



		//t1->waitUntilFinish();
		//if (!motorE.isRunning())

		//std::cout << "Pwm period time: " << motorE.getPeriodValue() << " nanoseconds \n";
		return 0;}





