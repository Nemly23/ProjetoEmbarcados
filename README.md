# ROBÔ ASPIRADOR DE PÓ...

Projeto desenvolvido para disciplina Sistemas Embarcados.

Vídeo de apresentação e funcionamento: [🎬🎥🚗](https://youtu.be/IGNw8D5H5aE)

<p align="center">
  <img width="640" height="360" src=/imagens/robo.jpeg>
</p>

## ... que não aspira pó de verdade.
Queremos simular o funcionamento de um aspirador de pó autônomo comercial: fazer uma varredura inteligente por todo o cômodo, evitando objetos e paredes.

<p align="center">
  <img width="500" height="500" src=/imagens/funcionamento.jpg>
</p>

Esta varredura, como ilustrada acima, funciona em duas direções: começa com um zigue-zague em uma direção e, após atingir a parede oposta, faz o mesmo movimento no sentido perpendicular ao anterior, garantindo que todos os pontos da área sejam percorridos. 

## Quem?

* Amanda Malimpensa | [AmandaMK015](https://github.com/AmandaMK015)
* Camila Quadros | [millamsq](https://github.com/millamsq)
* Rafael Matumoto | [rmatumoto](https://github.com/rmatumoto)
* Ricardo Peixoto | [Nemly23](https://github.com/Nemly23)

## Como?
Uma IMU (MinIMU-9 v2, POLOLU) para localização e navegação no ambiente de trabalho e três sensores ultrassônicos (HC-SR04) para detecção de objetos/colisões.

### Lista de componentes

|               Componente               | Quantidade |
|:--------------------------------------:|:----------:|
|            Beaglebone Black            |      1     |
|              Cabo Ethernet             |      1     |
|            Bateria LIPO 7.4V           |      1     |
|            Fonte externa 5V            |      1     |
|            IMU (MinIMU-9 v2)           |      1     |
|      Sensor ultrassônico (HC-SR04)     |      3     |
|            Ponte H (modelo)            |      1     |
| Conversor lógico bidirecional (modelo) |      1     |
|                Motor DC                |      2     |
|           Botão de 2 estados           |      1     |
|                  Chave                 |      1     |

### Esquemático do circuito
![Circuito](/imagens/esquematico.jpg)

### Andamento do projeto

#### Inicialização
Instalação do Debian em cartão SD para [boot](https://beagleboard.org/getting-started) na placa.

---
#### Configuração da IDE

---
#### Comunicação
Comunicação entre _target_ e _host_ por SSH: 
> ssh debian@\<IP>

Como é feita a compilação cruzada, o arquivo executável é enviado para o _target_:
> scp \<arquivo.exe> debian@\<IP>:/home/debian

---
#### Bibliotecas de terceiros
Adequação da biblioteca de [PWM](https://github.com/yigityuce/BlackLib), que apresenta incompatibilidade com kernel 3.7+, para versão atual (4.14).

1. Endereçamento direto do pino para PWM:
```c
(...)
case P8_13:
	{
		searchResult = "/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip7";
		break;
	}
(...)
```

2. Diretório para troca de modo dos pinos GPIO - _default_ ou _pwm_:
```c
(...)
case P8_13:
	{
		searchResult = "/sys/devices/platform/ocp/ocp:P8_13_pinmux/state";
		break;
	}
(...)
```

---
#### Motores
Foi utilizada uma ponte H para controle da velocidade e sentido de rotação dos motores DC.

A alimentação fornecida pela Beaglebone Black, de 3.3 V, não é suficiente para os dois motores; colocada uma bateria de 7.4 V e um regulador de tensão (_step down_) para adequar entrada (5 V).

---
#### Sensores ultrassônicos
Utilizados detecção de paredes e objetos. Os sensores ultrassônicos HC-SR04 funcionam com um sinal _trigger_ que ativa o envio de uma série de pulsos; estas ondas, ao colidirem em objetos ao longo de seu caminho, são rebatidas e os sensores as recebem de volta (evento determinado na porta _echo_). O intervalo de tempo entre estes dois eventos corresponde à distância entre o robô e o obstáculo detectado.

<p align="center">
  <img width="500" height="300" src=/imagens/ultrassonico.jpg>
</p>

Para envio do sinal, a porta _trigger_ do sensor deve estar em nível lógico alto por 10 us, voltando a nível baixo em seguida. Isto habilita o envio de um trem de oito pulsos a 40 kHz e o sinal _echo_ é setado; inicia-se a contagem de tempo neste instante. Quando estas ondas atingem um objeto e retornam ao sensor, _echo_ torna-se zero e o cronômetro para, marcando o tempo _t_. Considerando a velocidade do som no ar (c) igual a 346.3 m/s, a distância entre o robô e o objeto é

<p align="center" href="https://www.codecogs.com/eqnedit.php?latex=d=\frac{t\times&space;c}{2}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?d=\frac{t\times&space;c}{2}" title="d=\frac{t\times c}{2}" /></p>

São monitoradas as distâncias frontais e laterais no robô. A frontal impede que ocorra uma colisão; as laterais, além de indicarem a distância que devem manter da parede, também servem para identificação de objetos próximos que devem ser evitados durante a trajetória linear.

----
#### IMU
As direções que o robô seguem são definidas pelo giroscópio. A velocidade angular medida pelo sensor é convertida para os ângulos de cada eixo, mas usa-se somente o de _yaw_, em torno do eixo z. 

O norte do carrinho é sempre a direção para a qual ele aponta. Para o alinhamento correto com as direções de giro e passo, muda-se a referência deste norte. Assim, nas funções `<turn_90>` e `<follow_direction>`, respectivamente, temos:

```c
(...)
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
(...)
```

```c
(...)
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
(...)
```

Em ambos os casos, deve ser feito um ajuste neste ângulo de referência para mantê-lo no intervalo [-180°, 180°]:

```c
(...)
if (angle_ref > 180)
{
	angle_ref -= 360;
}

if (angle_ref < -180)
{
	angle_ref += 360;
}
(...)
```

---
#### Lógica de funcionamento
O robô realiza uma série de movimentos sequenciais; o conjunto destas 9 funções dita a varredura completa pela área de trabalho:

<p align="center">
  <img width="600" height="600" src=/imagens/funcoes.jpg>
</p>

Inicialmente, ele deve ser sempre colocado junto a uma parede, esquerda ou direita, de onde ele mantém uma distância fixa. Então, como ilustrado anteriormente, segue em linha reta de um lado ao outro, até atingir a parede oposta, onde para uma vez. Realiza uma rotação de 90° (o sentido depende da parede em que inicia), dá um passo fixo para alinhamento com a próxima linha de varredura, gira novamente, posicionando-se a 180° da direção inicial e faz a trajetória contrária, voltando para o lado de onde saiu.

Esta sequência é repetida até que encontre a parede oposta; quando identifica paredes em sequência, marca o inínio deste mesmo padrão de varredura na direção cruzada. O robô finaliza sua execução quando se depara com as paredes sequenciais pela segunda vez.
