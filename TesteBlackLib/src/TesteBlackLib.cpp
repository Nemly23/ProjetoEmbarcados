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
#include <math.h>
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

using namespace BlackLib;
using namespace std;

// Periodo do PWM (us).
#define PeriodTimeMicro 1000
#define max_speed 80
float constant_speed = 60;

// Motores direito e esquerdo.
BlackPWM motorR(P8_13);
BlackPWM motorL(P8_19);

// Saida para ponte H.
BlackGPIO IN1(GPIO_65, output, SecureMode); // Motor direito.
BlackGPIO IN2(GPIO_27, output, SecureMode);
BlackGPIO IN3(GPIO_47, output, SecureMode); // Motor esquerdo.
BlackGPIO IN4(GPIO_46, output, SecureMode);

// Pino para enable de todo o programa.
BlackGPIO button(GPIO_26, input, SecureMode);

int n = 0;
int n2 = 0;
int fd;
int ret = 1;
char ch;
int value = 0;
bool stop_flag=false;
bool turn_flag=false;

float angle_mag;

double dis;
double dis_right = 1;
double dis_left = 1;
double dis_front = 1;

double angle_base = 0;

chrono::duration<double> elapsed;
chrono::duration<double> elapsed2;
auto start_dis = chrono::high_resolution_clock::now();
auto stop_dis = chrono::high_resolution_clock::now();


// Inicializacao do PWM.
void setup_PWM()
{
    motorR.setDutyPercent(100.0);
    motorR.setPeriodTime(PeriodTimeMicro, microsecond);
    motorR.setPolarity(straight);
    motorL.setDutyPercent(100.0);
    motorL.setPeriodTime(PeriodTimeMicro, microsecond);
    motorL.setPolarity(straight);
}

// Inicializa todas as portas digitais em LOW.
void setup_GPIO()
{
    IN1.setValue(low);
    IN2.setValue(low);
    IN3.setValue(low);
    IN4.setValue(low);
}

// Motor direito para frente.
void go_straight_right(float speed)
{
	if (speed>max_speed){
		speed = max_speed;
	}
    motorR.setDutyPercent(100 - speed);
    IN1.setValue(high);
    IN2.setValue(low);
}

// Inversao da rotacao do motor direito.
void go_reverse_right(float speed)
{
	if (speed>max_speed){
		speed = max_speed;
	}
    motorR.setDutyPercent(100 - speed);
    IN1.setValue(low);
    IN2.setValue(high);
}

// Motor esquerdo para frente.
void go_straight_left(float speed)
{
	if (speed>max_speed){
		speed = max_speed;
	}
    motorL.setDutyPercent(100 - speed);
    IN3.setValue(low);
    IN4.setValue(high);
}

// Inversao da rotacao do motor esquerdo.
void go_reverse_left(float speed)
{
	if (speed>max_speed){
		speed = max_speed;
	}
    motorR.setDutyPercent(100 - speed);
    IN3.setValue(high);
    IN4.setValue(low);
}

void stop_motor(){
	motorR.setDutyPercent(100);
	motorR.setDutyPercent(100);
	IN1.setValue(low);
	IN2.setValue(low);
	IN3.setValue(low);
	IN4.setValue(low);
}

// Liga PWM.
void turn_on_pwm()
{
    motorR.setRunState(run);
    motorL.setRunState(run);
}

// Para o PWM.
void turn_off_pwm()
{
    motorR.setRunState(stop);
    motorL.setRunState(stop);
}

/*
void segue_10cm(){
	go_straight_left(50);
	go_straight_right(50);
	turn_on_pwm();
	calcula_distancia(10);
	turn_off_pwm();
}
*/

void verify_proximity(float speed){
	if (dis_front < 0.30){
		stop_flag = true;
		stop_motor();
	}
	else if(dis_front < 0.60){
		speed = speed*0.8;
		stop_flag = false;
	}
	else{
		stop_flag = false;
	}
}

// Recebe sinal ECHO do sensor US e faz processamento; calcula a distancia.
int receive_pulse_ultrasound(string pin_path)
{
    // Pulso ECHO em LOW.
    while (value == 0 && n2 < 100)
    {
        // Erro na abertura do diretorio com valor do pulso.
        if ((fd = open(pin_path.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
        {
            cout << "erro_open" << endl;
            n++;
            return -1;
        }

        // Erro na leitura do valor.
        ret = read(fd, &ch, sizeof(ch));
        if (ret < 0)
        {
            n++;
            cout << "erro_read" << endl;
            continue;
        }

        // Se valor do pulso muda, sai do loop.
        if (ch != '0')
        {
            value = 1;
        }
        else
        {
            value = 0;
        }

        n2++;
        close(fd);
    }
    //cout << n2 << endl;

    // Inicia contador de tempo para ECHO em HIGH.
    auto start = chrono::high_resolution_clock::now();

    // ECHO em HIGH; continua no loop enquanto sinal nao e recebido de volta.
    while (value == 1 && n < 100)
    {
        // Erro na abertura do diretorio com valor do pulso.
        if ((fd = open(pin_path.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
        {
            cout << "erro_open" << endl;
            n++;
            return -1;
        }

        // Erro na leitura do valor.
        ret = read(fd, &ch, sizeof(ch));
        if (ret < 0)
        {
            n++;
            cout << "erro_read" << endl;
            continue;
        }

        // Se valor do pulso muda, sai do loop.
        if (ch != '0')
        {
            value = 1;
        }
        else
        {
            value = 0;
        }

        n++;
        close(fd);
    }

    auto finish = chrono::high_resolution_clock::now();
    elapsed = finish - start;

    // Erro se nao entra no segundo loop e da timeout no primeiro.
    if (n2 == 100 && n == 0)
    {
        cout << "erro_receive" << endl;
        return -1;
    }
    else {
    	// Distancia ate obstaculo em metros.
    	dis = elapsed.count() * 346.3 / 2.0;
    }
    return 0;
    //cout << elapsed.count() << " s" << endl;
}

// Thread com sensores.
class Task1 : public BlackThread
{
public:
    void onStartHandler()
    {
        int i = 0;
        int erro = 0;

        // Diretorio de cada US.
        string pin1_path = "/sys/class/gpio/gpio44/value";
        string pin2_path = "/sys/class/gpio/gpio68/value";
        string pin3_path = "/sys/class/gpio/gpio67/value";

        // US 1.
        BlackGPIO trig1(GPIO_45, output, SecureMode);
        BlackGPIO echo1(GPIO_44, input, SecureMode);

        // US 2.
        BlackGPIO trig2(GPIO_69, output, SecureMode);
        BlackGPIO echo2(GPIO_68, input, SecureMode);

        // US 3.
        BlackGPIO trig3(GPIO_66, output, SecureMode);
        BlackGPIO echo3(GPIO_67, input, SecureMode);

        // Magnetometro.
        uint8_t read_buffer[13] = {0};
        uint8_t register_value = 0x00; // Primeiro registrador.
        uint8_t readBlockSize;

        float x_mag = 0;
        float z_mag = 0;
        float y_mag = 0;

        // Inicia comunicacao I2C com magnetometro (endereco 0x1E).
        BlackI2C myI2c(I2C_2, 0x1E);

        myI2c.open(ReadWrite);

        // Configuracao sensor.
        bool resultOfWrite = myI2c.writeByte(0x00, 0x70);
        cout << "new value is wrote?: " << boolalpha << resultOfWrite << dec << endl;

        // Configuracao sensor.
        resultOfWrite = myI2c.writeByte(0x01, 0xA0);
        cout << "new value is wrote?: " << boolalpha << resultOfWrite << dec << endl;

        // Configuracao modo (leitura continua).
        resultOfWrite = myI2c.writeByte(0x02, 0x00);
        cout << "new value is wrote?: " << boolalpha << resultOfWrite << dec << endl;

        int count = 0; // Counter for alternating shifts.
        short value2 = 0;

        while (!button.isHigh())
        {

            // US 1.
            n = 0;
            n2 = 0;
            value = 0;

            // Seta o TRIGGER.
            trig1.setValue(low);
            usleep(2);
            trig1.setValue(high);
            //usleep(10);
            trig1.setValue(low);

            // Espera ECHO.
            receive_pulse_ultrasound(pin1_path);
            dis_front = dis*100;

            cout << dis << " m (1)" << endl;
            //cout << value << endl;

            // US 2.
            n = 0;
            n2 = 0;
            value = 0;

            // Seta o TRIGGER.
            trig2.setValue(low);
            usleep(2);
            trig2.setValue(high);
            //usleep(10);
            trig2.setValue(low);

            // Espera ECHO.
            receive_pulse_ultrasound(pin2_path);
            dis_right = dis*100;
            cout << dis << " m (2)" << endl;
            //cout << value << endl;

            // US 3.
            n = 0;
            n2 = 0;
            value = 0;

            // Seta o TRIGGER.
            trig3.setValue(low);
            usleep(2);
            trig3.setValue(high);
            //usleep(10);
            trig3.setValue(low);

            // Espera ECHO.
            receive_pulse_ultrasound(pin3_path);
            dis_left = dis*100;
            cout << dis << " m (3)" << endl;
            //cout << value << endl;

            // Leitura do magnetometro.
            readBlockSize = myI2c.readBlock(register_value, read_buffer, sizeof(read_buffer));
            //cout << "Total read block size: " << (int)readBlockSize << endl;

            // Checa se todos os bytes foram lidos.
            if (readBlockSize != 13)
            {
                cout << "erro_read_I2C" << endl;
            }
            else
            {
                // Registradores 0x03 a 0x09.
                for (int j = 3; j < 9; j++)
                {
                    value2 = value2 | read_buffer[j]; // OR read_buffer into lower byte.

                    // LSB nos registradores pares.
                    if (count % 2 == 0)
                    {
                        value2 = value2 << 8; // Shift 8 bits every other loop.
                    }
                    else
                    {
                        cout << value2 << "	";
                        // Le 3 direcoes em sequencia.
                        if (count == 1)
                        {
                            x_mag = value2;
                        }
                        else if (count == 3)
                        {
                            z_mag = value2;
                        }
                        else
                        {
                            y_mag = value2;
                        }
                        value2 = 0;
                    }
                    count++;
                }
                count = 0;
            }
            cout << endl;
            //usleep(DELAY); //500 ms
            angle_mag = atan2(x_mag, y_mag)/M_PI*180 - angle_base;
            cout << "angle" << angle_mag << "degres" << endl;

            usleep(100000);
            i++;
        }
    }

    void onStopHandler()
    {
    }
};

int change_function(int function){
	if (function==7){
		return 1;
	}
	return (function+1);
}

float turn_90(int modo){
	float adjust=0.5*max_speed;
	float angle_ref;
	stop_flag = true;
	switch(modo){
		case 0:
			angle_ref = -90 - angle_mag;
			break;
		case 1:
			angle_ref = 90 - angle_mag;
			break;
		case 2:
			angle_ref = 180 - angle_mag;
			break;
		case 3:
			angle_ref = 0 - angle_mag;
			break;
	}
	if (angle_ref>180){
		angle_ref -= 360;
	}
	if (angle_ref<-180){
		angle_ref += 360;
	}
	if (angle_ref<30 && angle_ref>-30){
		adjust *= 0.8;
	}
	if (angle_ref<2 && angle_ref>-2){
		adjust = 0;
		turn_flag = false;
	}
	if (angle_ref>0){
		return adjust;
	}else{
		return -adjust;
	}

}

float follow_direction(int dir, float *error_i){
	float adjust=0;
	float angle_ref;
	float Kp = 3;
	float Ki = 0.1;
	float dis_time = 2;
	//float Kd = 0;

	switch(dir){
		case 0:
			angle_ref = -90 - angle_mag;
			break;
		case 1:
			angle_ref = 90 - angle_mag;
			break;
		case 2:
			angle_ref = 180 - angle_mag;
			break;
		case 3:
			angle_ref = 0 - angle_mag;
			break;
	}
	*error_i += angle_ref;
	stop_dis = chrono::high_resolution_clock::now();
	elapsed2 = stop_dis - start_dis;
	if (elapsed2.count()> dis_time && dir%2==0){
		turn_flag = false;
		stop_flag = true;
		return 0;
	}
	adjust = Kp*angle_ref + Ki*(*error_i);
	return adjust;
}

int main()
{
    int i = 0;
    float speed_right = 0;
    float speed_left = 0;
    float adjustment = 0;
    int function = 0;
    float dis_stop = 0.2;
    float dis_red = 0.6;
    float error_i = 0;


    Task1 *t1 = new Task1();

    setup_PWM();
    setup_GPIO();
    t1->run();

    turn_on_pwm();
    while (!button.isHigh())
    {
    	if (function==0){
    		//adjustment = follow_wall();
    		speed_left = constant_speed - adjustment;
    		speed_right = constant_speed + adjustment;
    	}

    	if (function==1){
			adjustment = turn_90(0);
    		// Giro em torno do centro.
    		// Alinhamento com leste.
			speed_left = -adjustment;
			speed_right = adjustment;
		}

    	if (function==2){
    		adjustment = follow_direction(0, &error_i);
			// Passo ate proxima linha de limpeza.
    		stop_flag = true;
			speed_left = constant_speed - adjustment;
			speed_right = constant_speed + adjustment;
		}

    	if (function==3){
			adjustment = turn_90(2);
    		// Alinhamento com sul.
			speed_left = -adjustment;
			speed_right = adjustment;
		}

    	if (function==4){
    		adjustment = follow_direction(2, &error_i);
			// Segue sul ate encontrar parede.
    		turn_flag = false;
    		speed_left = constant_speed - adjustment;
    		speed_right = constant_speed + adjustment;
		}

    	if (function==5){
			adjustment = turn_90(1);
    		// Alinhamento com oeste.
			speed_left = -adjustment;
			speed_right = adjustment;
		}

    	if (function==6){
    		adjustment = follow_direction(1, &error_i);
    		// Passo ate proxima linha de limpeza.
    		stop_flag = true;
    		speed_left = constant_speed - adjustment;
			speed_right = constant_speed + adjustment;
		}

    	if (function==7){
			adjustment = turn_90(3);
    		// Alinhamento com norte (volta para estado inicial).
			speed_left = -adjustment;
			speed_right = adjustment;
		}

    	if (function==8){
    		adjustment = follow_direction(3, &error_i);
			// Segue norte ate encontrar parede.
    		turn_flag = false;
    		speed_left = constant_speed - adjustment;
			speed_right = constant_speed + adjustment;
		}

    	if (function==9){
			// Funcao final.
		}

    	if (dis_front < dis_stop && stop_flag==false){
    		stop_flag = true;
    		stop_motor();
    	}
    	else {
    		if(dis_front < dis_red){
    			speed_right = speed_right*0.8;
    			speed_left = speed_left*0.8;
    		}
			if (speed_right>0){
				go_straight_right(speed_right);
			}
			else{
				go_reverse_right(-speed_right);
			}
			if (speed_left>0){
				go_straight_left(speed_right);
			}
			else{
				go_reverse_left(-speed_right);
			}
    	}
    	if (stop_flag == true && turn_flag == false){
    		stop_motor();
    		function = change_function(function);
    		error_i = 0;
    		start_dis = chrono::high_resolution_clock::now();
    		turn_flag = true;
    		stop_flag = false;
    	}
    	usleep(50*1000);
    }
    turn_off_pwm();

    t1->waitUntilFinish();

    //if (!motorE.isRunning())
    cout << "programa_finalizado";
    //cout << "Pwm period time: " << motorE.getPeriodValue() << " nanoseconds \n";
    return 0;
}
