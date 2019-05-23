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

float x_mag = 0;
float z_mag = 0;
float y_mag = 0;
double dis;

chrono::duration<double> elapsed;

// Inicializacao do PWM.
void setup_PWM()
{
    motorR.setDutyPercent(100.0);
    motorR.setPeriodTime(PeriodTimeMicro, microsecond);
    motorR.setDutyPercent(0);
    motorR.setPolarity(straight);
    motorL.setDutyPercent(100.0);
    motorL.setPeriodTime(PeriodTimeMicro, microsecond);
    motorL.setDutyPercent(0);
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
    motorR.setDutyPercent(100 - speed);
    IN1.setValue(high);
    IN2.setValue(low);
}

// Inversao da rotacao do motor direito.
void go_reverse_right(float speed)
{
    motorR.setDutyPercent(100 - speed);
    IN1.setValue(low);
    IN2.setValue(high);
}

// Motor esquerdo para frente.
void go_straight_left(float speed)
{
    motorL.setDutyPercent(100 - speed);
    IN3.setValue(low);
    IN4.setValue(high);
}

// Inversao da rotacao do motor esquerdo.
void go_reverse_left(float speed)
{
    motorR.setDutyPercent(100 - speed);
    IN3.setValue(high);
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

// Recebe sinal ECHO do sensor US e faz processamento; calcula a distancia.
void receive_pulse_ultrasound(string pin_path)
{
    // Pulso ECHO em LOW.
    while (value == 0 && n2 < 100)
    {
        // Erro na abertura do diretorio com valor do pulso.
        if ((fd = open(pin_path.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
        {
            cout << "erro_open" << endl;
            n++;
            continue;
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
            continue;
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
    }

    // Distancia ate obstaculo em metros.
    dis = elapsed.count() * 346.3 / 2.0;
    //cout << elapsed.count() << " s" << endl;
}

// Thread com sensores.
class Task1 : public BlackThread
{
public:
    void onStartHandler()
    {
        int i = 0;

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

            usleep(100000);
            i++;
        }
    }

    void onStopHandler()
    {
    }
};

int main()
{
    int i = 0;
    float speed = 0;

    Task1 *t1 = new Task1();

    setup_PWM();
    setup_GPIO();

    t1->run();

    while (!button.isHigh())
    {
        cout << button.isHigh() << endl;
        go_straight_right(30);
        cout << "Right_30" << endl;

        turn_on_pwm();
        sleep(1);
        turn_off_pwm();
        sleep(1);

        go_straight_right(60);
        cout << "Right_60" << endl;

        turn_on_pwm();
        sleep(1);
        turn_off_pwm();
        sleep(1);

        go_straight_right(100);
        cout << "Right_100" << endl;
        cout << button.isHigh() << endl;

        turn_on_pwm();
        sleep(1);
        go_straight_right(0);
        turn_off_pwm();
        sleep(1);

        go_straight_left(30);
        cout << "Left_30" << endl;

        turn_on_pwm();
        sleep(1);
        turn_off_pwm();
        sleep(1);

        go_straight_left(60);
        cout << "Left_60" << endl;

        turn_on_pwm();
        sleep(1);
        turn_off_pwm();
        sleep(1);

        go_straight_left(100);
        cout << "Left_100" << endl;

        turn_on_pwm();
        sleep(1);

        go_straight_left(0);

        turn_off_pwm();
        sleep(1);

        i++;
    }

    turn_off_pwm();
    t1->waitUntilFinish();

    //if (!motorE.isRunning())
    cout << "programa_finalizado";
    //cout << "Pwm period time: " << motorE.getPeriodValue() << " nanoseconds \n";
    return 0;
}
