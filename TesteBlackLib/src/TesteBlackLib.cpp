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

// Percentual maximo para velocidade.
#define max_speed 100
float constant_speed = 80; // Velocidade base.

enum functionNumber{
    parede = 0,
    giraLeste = 1,
    stepLeste = 2,
    giraSul = 3,
    segueSul = 4,
    giraOeste = 5,
    stepOeste = 6,
    giraNorte = 7,
    segueNorte = 8,
    fimSul = 9,
	fimNorte = 10
};
enum mode{
    leste = 0,
	oeste = 1,
    sul = 2,
    norte = 3
};

enum ladoParede{
    esquerda = false,
	direita = true,
};

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
bool stop_flag = false;
bool turn_flag = false;
bool fim_flag = false;
bool fim_oeste_flag = false;
bool fim_leste_flag = false;
bool read_angle = false;
bool fim = false;

float angle_mag;

double dis;
double dis_right = 1;
double dis_left = 1;
double dis_front = 1;
double dis_right_ant = 0;
double dis_left_ant = 0;

float rate_gyr_x;
float rate_gyr_y;
float rate_gyr_z;

float gyr_x=0;
float gyr_y=0;
float gyr_z=0;

float offset_z=0;

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
    if (speed > max_speed)
    {
        speed = max_speed;
    }
    motorR.setDutyPercent(100 - speed);
    IN1.setValue(high);
    IN2.setValue(low);
}

// Inversao da rotacao do motor direito.
void go_reverse_right(float speed)
{
    if (speed > max_speed)
    {
        speed = max_speed;
    }
    motorR.setDutyPercent(100 - speed);
    IN1.setValue(low);
    IN2.setValue(high);
}

// Motor esquerdo para frente.
void go_straight_left(float speed)
{
    if (speed > max_speed)
    {
        speed = max_speed;
    }
    motorL.setDutyPercent(100 - speed);
    IN3.setValue(low);
    IN4.setValue(high);
}

// Inversao da rotacao do motor esquerdo.
void go_reverse_left(float speed)
{
    if (speed > max_speed)
    {
        speed = max_speed;
    }
    motorL.setDutyPercent(100 - speed);
    IN3.setValue(high);
    IN4.setValue(low);
}

void stop_motor()
{
    motorR.setDutyPercent(0);
    motorR.setDutyPercent(0);
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

void verify_proximity(float speed)
{
    if (dis_front < 0.30)
    {
        stop_flag = true;
        stop_motor();
    }
    else if (dis_front < 0.60)
    {
        speed = speed * 0.8;
        stop_flag = false;
    }
    else
    {
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
    else
    {
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
        //int erro = 0;

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
        uint8_t read_buffer[6] = {0};
        //uint8_t register_value = 0x03; // Primeiro registrador.
        uint8_t readBlockSize;

        //float x_mag = 0;
        //float z_mag = 0;
       // float y_mag = 0;

		float tempo;
        
        BlackI2C  gyro(I2C_2, 0x6B);
		gyro.open( ReadWrite);
		bool resultOfWrite          = gyro.writeByte(0x20, 0x6F);
		std::cout << "new value is wrote?: " << std::boolalpha << resultOfWrite << std::dec << std::endl;
		resultOfWrite          = gyro.writeByte(0x23, 0x40);
		std::cout << "new value is wrote?: " << std::boolalpha << resultOfWrite << std::dec << std::endl;
		/*
        // Inicia comunicacao I2C com magnetometro (endereco 0x1E).
        BlackI2C mag(I2C_2, 0x1E);

        mag.open(ReadWrite);

        // Configuracao sensor.
        resultOfWrite = mag.writeByte(0x00, 0x0C);
        cout << "new value is wrote?: " << boolalpha << resultOfWrite << dec << endl;

        // Configuracao sensor.
        resultOfWrite = mag.writeByte(0x01, 0x20);
        cout << "new value is wrote?: " << boolalpha << resultOfWrite << dec << endl;

        // Configuracao modo (leitura continua).
        resultOfWrite = mag.writeByte(0x02, 0x00);
        cout << "new value is wrote?: " << boolalpha << resultOfWrite << dec << endl;
		*/
        int count = 0; // Counter for alternating shifts.
        short value2 = 0;
        chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
        chrono::high_resolution_clock::time_point t0 = chrono::high_resolution_clock::now();

        while (!button.isHigh() && fim == false)
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
            dis_front = dis * 100;

            //cout << dis*100 << " cm (1)" << endl;
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
            dis_right = dis * 100;
            //cout << dis*100 << " cm (2)" << endl;
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
            dis_left = dis * 100;
            //cout << dis*100 << " cm (3)" << endl;
            //cout << value << endl;
            /*
            // Leitura do magnetometro.
            readBlockSize = mag.readBlock(register_value, read_buffer, sizeof(read_buffer));
            //cout << "Total read block size: " << (int)readBlockSize << endl;

            // Checa se todos os bytes foram lidos.
            if (readBlockSize != 6)
            {
                cout << "erro_read_I2C" << endl;
            }
            else
            {
            	//printf("Looks like the I2C bus is operational! \n");
				for (int j=0; j<6; j++) {
					value2 = value2 | read_buffer[j];	//OR read_buffer into lower byte
					if (count%2 == 0)	//Shift 8 bits every other loop
						value2 = value2 << 8;
					else {
						if (j == 1)
							x_mag = (float)value2/1100 - 0.09;
						if (j == 3)
							z_mag = (float)value2/980 ;
						if (j == 5)
							y_mag =  (float)value2/1100 + 0.04;
						value2 = 0;
					}//end if
					count++;
				}// end for

				count = 0;
            	//x_mag = (float)(read_buffer[0] << 8 | read_buffer[1]);
				//y_mag = (float)(read_buffer[4] << 8 | read_buffer[5]);
				//z_mag = (float)(read_buffer[2] << 8 | read_buffer[3]);
				//x_mag = x_mag/1100 - 0.09;
				//y_mag = y_mag/1100 + 0.04;
            }
            //cout << endl;
            //usleep(DELAY); //500 ms
            angle_mag = atan2(y_mag, x_mag) / M_PI * 180 - angle_base;
            // Transformacao para intervalo -180 180
            if (angle_mag > 180){
            	angle_mag -= 360;
            }
            if (angle_mag < -180)
            {
            	angle_mag += 360;
            }
            //cout << "x_mag " << x_mag << "degres" << endl;
            //cout << "y_mag " << y_mag << "degres" << endl;
            //cout << "angle " << angle_mag << "degres" << endl;
			*/
            readBlockSize       = gyro.readBlock(0x28 | 0x80, read_buffer, sizeof(read_buffer) );
			//std::cout << "Total read block size: " << (int)readBlockSize << std::endl;
			if (readBlockSize!=6){
				cout << "erro" << endl;
			}
			else {
				//printf("Looks like the I2C bus is operational! \n");
				for (int j=0; j<6; j++) {
					value2 = value2 | read_buffer[j];	//OR read_buffer into lower byte
					if (count%2 == 0)	//Shift 8 bits every other loop
						value2 = value2 << 8;
					else {
						//if (j == 1)
							//rate_gyr_x = value2 * 0.00875;
						//if (j == 3)
							//rate_gyr_y = value2* 0.00875;
						if (j == 5)
							rate_gyr_z =  value2 * 0.00875 - offset_z;
						value2 = 0;
					}//end if
					count++;
				}// end for

				count = 0;
			}//end else
			t1 = chrono::high_resolution_clock::now();
			tempo = chrono::duration_cast<chrono::duration<double>>(t1 - t0).count();
			t0 = t1;
			//cout << "rate_z " << rate_gyr_z << endl;
			//cout << "tempo " << tempo << endl;
			if (read_angle == true){
				gyr_z += rate_gyr_z * tempo;
				 if (gyr_z> 180)
				{
					 gyr_z -= 360;
				}

				if (gyr_z < -180)
				{
					gyr_z += 360;
				}
				//cout << "gyr_z = " << gyr_z << endl;
			}

            usleep(10*1000);
            i++;
        }
    }

    void onStopHandler()
    {
    }
};

// Faz switch das funcoes na ordem da ocorrencia dos eventos
int change_function(int function)
{
    if (function == segueNorte)
    {
        return giraLeste;
    }
    if (function == stepLeste && fim_oeste_flag == true){
    	fim_oeste_flag = false;
    }
    if (function == stepOeste && fim_leste_flag == true){
		fim_leste_flag = false;
	}
    /*
    if (function == giraSul && fim_flag)
    {
    	return fimSul;
    }
    if (function == giraNorte && fim_flag)
    {
    	return fimNorte;
    }
    */
    return (function + 1);
}

// Rotacao de 90 graus.
float turn_90(mode m, int *count)
{
    float adjust = 0.8* max_speed;
    float angle_ref;
    float Kp = 2; //2
    stop_flag = true;

    switch (m)
    {
    case leste:
        angle_ref = 90 - gyr_z;
        break;
    case oeste:
        angle_ref = -90 - gyr_z;
        break;
    case sul:
        angle_ref = 180 - gyr_z;
        break;
    case norte:
        angle_ref = 0 - gyr_z;
        break;
    }

    if (angle_ref > 180)
    {
        angle_ref -= 360;
    }

    if (angle_ref < -180)
    {
        angle_ref += 360;
    }

    adjust = Kp * angle_ref;

    if (adjust > 0 && adjust < 0.7* max_speed){
    	adjust = 0.7* max_speed;
    }
    if (adjust < 0 && adjust > -0.7*max_speed){
       	adjust = -0.7* max_speed;
    }

    /*
    if (angle_ref < 15 && angle_ref > -15)
    {
        adjust *= 0.9;
    }
	*/
    if (angle_ref < 3.0 && angle_ref > -3.0)
    {
    	*count += 1;
    	cout << "count = " << *count << endl;
    	if (*count==5){
    		adjust = 0;
    		turn_flag = false;
    	}
    }
    if (angle_ref > 3.0 || angle_ref < -3.0)
	{
    	cout << "angle_ref " << angle_ref << endl;
    	*count -= 1;
    	if (*count<0){
    		*count = 0;
    	}
	}
    return -adjust;
    /*
    if (angle_ref > 0)
    {
        return -adjust;
    }
    else
    {
        return adjust;
    }
    */
}

float follow_wall(float *error_i, ladoParede lado){
	float adjust = 0;
	float dis_ref=10;
	float Kp = 3;
	float Ki = 0;
	if (lado == esquerda){
		if (dis_left > 20){
			dis_left = dis_left_ant;
		}
		else{
			dis_left_ant = dis_left;
		}
		dis_ref = - dis_ref + dis_left_ant;
	}
	else{
		if (dis_right >  20){
			dis_right = dis_right_ant;
		}
		else{
			dis_right_ant = dis_right;
		}
		dis_ref = dis_ref - dis_right_ant;
	}
	*error_i += dis_ref;
	adjust = Kp * dis_ref + Ki * (*error_i);
	return adjust;
}

// Segue em caminho reto.
float follow_direction(mode dir, float *error_i)
{
    float adjust = 0;
    float angle_ref;
    float Kp = 2; //2
    float Ki = 0.01; //0.01
    float dis_time = 1.5;
    //float Kd = 0;
    turn_flag = false;
    // Angulo inicial do movimento tomado como referencia. Setpoint do controle e o angulo zero, entao angle_ref e o proprio erro nas iteracoes seguintes.
    if (dis_left < 10 || dis_right < 10){
		if (dis_left < 10){
			*error_i = 0;
			adjust = follow_wall(error_i,esquerda);
		}
		if (dis_right < 10){
			*error_i = 0;
			adjust = follow_wall(error_i,direita);
		}
	}
    else{
		switch (dir)
		{
		case leste:
			angle_ref = gyr_z - 90;
			break;
		case oeste:
			angle_ref = gyr_z + 90;
			break;
		case sul:
			angle_ref = gyr_z - 180;
			break;
		case norte:
			angle_ref = gyr_z - 0;
			break;
		}

		if (angle_ref > 180)
		{
			angle_ref -= 360;
		}

		if (angle_ref < -180)
		{
			angle_ref += 360;
		}

		*error_i += angle_ref;
		adjust = Kp * angle_ref + Ki * (*error_i);
    }

    stop_dis = chrono::high_resolution_clock::now();
    elapsed2 = stop_dis - start_dis;

    if (elapsed2.count() > dis_time && dir < 2)
    {
    	cout << "distancia chegou "  << endl;
        stop_flag = true;
        return 0;
    }


    return adjust;
}

void calcule_offset(){
	int i=0;
	float soma_z=0;
	while(i<500){
		soma_z += rate_gyr_z;
		usleep(1*1000);
		i++;
	}
	offset_z += soma_z/i;
}

int main()
{
	ofstream myfile;
    int i = 0;
    float speed_right = 0;
    float speed_left = 0;
    float adjustment = 0;
    int function = parede;
    float dis_stop = 10;
    float dis_slow = 15;
    float error_i = 0;
    float angle_soma=0;
    int count=0;
    int count_left=0;
    int count_right=0;
    int false_stop = 0;
    ladoParede lado = esquerda;
    sleep(2);
    Task1 *t1 = new Task1();
    cout << "Waiting..." << endl;
    while (button.isHigh()){}
    cout << "Start" << endl;


    setup_PWM();
    setup_GPIO();
    t1->run();

    sleep(2);
    calcule_offset();
    calcule_offset();
    calcule_offset();

    while (count<10){
    	if (dis_left < 15){
    		count_left++;
    	}
    	if (dis_right < 15){
			count_right++;
		}
    	count++;
    	usleep(30 * 1000);
    }
    count=0;

    if (count_left > 5){
    	lado = esquerda;
    	function = segueNorte;
    }
    else{
		function = segueNorte;
	}
    if (count_right > 5){
    	lado = direita;
    	gyr_z -=180;
    	function = segueSul;
    }
    else{
    	function = segueNorte;
    }
    //angle_base = angle_mag;
    read_angle = true;

    turn_on_pwm();
    myfile.open ("Dados.txt");
    myfile << "tempo" << "\t" << "dis_front" << "\t" << "angle_mag" << "\t"  << "gyr_z" << "\t" << "dis_left" << "\t" << "dis_right" << "\t" << "funcao" << endl;
    chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
    chrono::high_resolution_clock::time_point t3 = chrono::high_resolution_clock::now();
    while (!button.isHigh())
    {
    	cout << dis_front << " cm (front)" << endl;
    	cout << dis_right << " cm (right)" << endl;
    	cout << dis_left << " cm (left)" << endl;
    	cout << "angle_ref = " << angle_base << "degres" << endl;
    	cout << "angle_mag = " << angle_mag << "degres" << endl;
    	cout << "gyr_z = " << gyr_z << endl;
    	t3 = chrono::high_resolution_clock::now();
    	myfile << chrono::duration_cast<chrono::duration<double>>(t3 - t2).count() << "\t" << dis_front << "\t" << angle_mag << "\t" << gyr_z << "\t" << dis_left << "\t" << dis_right << "\t" << function << endl;
    	//function = segueNorte;
        if (function == parede)
        {
        	cout << "parede" << endl;
            adjustment = follow_wall(&error_i, lado);
            angle_soma += angle_mag;
            count += 1;
            speed_left = constant_speed - adjustment;
            speed_right = constant_speed + adjustment;
        }

        if (function == giraLeste)
        {
        	cout << "giraL" << endl;
            adjustment = turn_90(leste, &i);
            // Giro em torno do centro.
            // Alinhamento com giraleste.
            speed_left = -adjustment;
            speed_right = +adjustment;
        }

        if (function == stepLeste)
        {
        	cout << "stepL" << endl;
            adjustment = follow_direction(leste, &error_i);
            // Passo ate proxima linha de limpeza.
            speed_left = constant_speed - adjustment;
            speed_right = constant_speed + adjustment;
        }

        if (function == giraSul)
        {
        	cout << "giraS" << endl;
            adjustment = turn_90(sul, &i);
            // Alinhamento com sul.
            speed_left = -adjustment;
            speed_right = +adjustment;
        }

        if (function == segueSul)
        {
        	cout << "segueSul" << endl;
            adjustment = follow_direction(sul, &error_i);
            // Segue sul ate encontrar parede.
            speed_left = constant_speed - adjustment;
            speed_right = constant_speed + adjustment;
        }

        if (function == giraOeste)
        {
        	cout << "giraO" << endl;
            adjustment = turn_90(leste, &i);
            // Alinhamento com oeste.
            speed_left = -adjustment;
            speed_right = +adjustment;
        }

        if (function == stepOeste)
        {
        	cout << "step0" << endl;
            adjustment = follow_direction(leste, &error_i);
            // Passo ate proxima linha de limpeza.
            speed_left = constant_speed - adjustment;
            speed_right = constant_speed + adjustment;
        }

        if (function == giraNorte)
        {
        	cout << "giraN" << endl;
            adjustment = turn_90(norte, &i);
            // Alinhamento com norte (volta para estado inicial).
            speed_left = -adjustment;
            speed_right = adjustment;
        }

        if (function == segueNorte)
        {
        	cout << "segueN" << endl;
            adjustment = follow_direction(norte, &error_i);
            // Segue norte ate encontrar parede.
            speed_left = constant_speed - adjustment;
            speed_right = constant_speed + adjustment;
        }
        /*
        if (function == fimSul)
        {
        	cout << "fimS" << endl;
            // Funcao final.
        	adjustment = follow_wall(&error_i, &angle_soma, &count, lado);
			speed_left = constant_speed - adjustment;
			speed_right = constant_speed + adjustment;
        }

        if (function == fimNorte)
		{
        	cout << "fimN" << endl;
			// Funcao final.
        	if (lado == esquerda)
        		adjustment = follow_wall(&error_i, &angle_soma, &count, direita);
        	else
        		adjustment = follow_wall(&error_i, &angle_soma, &count, esquerda);
			speed_left = constant_speed - adjustment;
			speed_right = constant_speed + adjustment;

		}
		*/
        cout <<  "sTOP_FLAG = " << stop_flag  << endl;
        cout <<  "tURN_FLAG = " << turn_flag  << endl;
        //stop_flag = false;
        if (dis_front < dis_stop && stop_flag == false)
        {
        	false_stop++;
        	stop_motor();
        	cout << "stop" << endl;
        	if (false_stop==5){
        		 stop_flag = true;
        		 if (function == parede){
					gyr_z -= angle_soma/count;
					if (lado == direita){
						gyr_z -=180;
					}
				 }
				 if (function == stepLeste){
					 if (fim_oeste_flag == true){
						 gyr_z = gyr_z - 90;
						 fim_oeste_flag = false;
						 if (fim_flag==true){
							 break;
						 }
						 fim_flag = true;
					 }
					 else{
						 fim_leste_flag = true;
					 }
				 }
				 if (function == stepOeste){
					 if (fim_leste_flag == true){
						 gyr_z = gyr_z + 90;
						 fim_leste_flag = false;
						 if (fim_flag==true){
							 break;
						 }
						 fim_flag = true;
					 }
					 else{
						 fim_oeste_flag = true;
					 }
				 }
				 /*
				 if (function == fimSul || function == fimNorte){
					break;
				 }
				 */
        	}
            //stop_flag = true;
        }
        else
        {
        	cout << "velocidade esquerdo = " << speed_left << endl;
        	cout << "velocidade direito = " << speed_right << endl;
        	false_stop = 0;
            if (dis_front < dis_slow)
            {
                speed_right = speed_right * 0.8;
                speed_left = speed_left * 0.8;
            }
            if (speed_right > 0)
            {
                go_straight_right(speed_right);
            }
            else
            {
                go_reverse_right(-speed_right);
            }
            if (speed_left > 0)
            {
                go_straight_left(speed_left);
            }
            else
            {
                go_reverse_left(-speed_left);
            }
        }

        if (stop_flag == true && turn_flag == false)
        {
        	cout << "muda de funcao" << endl;
            stop_motor();
            usleep(200 * 1000);
            calcule_offset();
            if (lado == direita && function == parede){
            	function = segueSul;
            }
            function = change_function(function);
            error_i = 0;
            start_dis = chrono::high_resolution_clock::now();
            turn_flag = true;
            stop_flag = false;
            i = 0;
        }
        //stop_flag = false;
        usleep(50 * 1000);
    }
    turn_off_pwm();
    fim = true;
    cout << "programa_finalizado";
    t1->stop();

    //if (!motorE.isRunning())
    cout << "programa_finalizado";
    //cout << "Pwm period time: " << motorE.getPeriodValue() << " nanoseconds \n";
    return 0;

   	 myfile.close();
}
