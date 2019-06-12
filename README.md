# ROBÔ ASPIRADOR DE PÓ...

Projeto desenvolvido para disciplina Sistemas Embarcados.

![O robô](/imagens/robo.jpeg)

## ... que não aspira pó de verdade.
Queremos simular o funcionamento de um aspirador de pó autônomo comercial: fazer uma varredura inteligente por todo o cômodo, evitando objetos e paredes.

<p align="center">
  <img width="500" height="500" src=/imagens/funcionamento.jpg>
</p>

Esta varredura, como ilustrada acima, funciona em duas direções: começa com um zigue-zague em uma direção e, após atingir a parede oposta, faz o mesmo movimento no sentido perpendicular ao anterior, garantindo que todos os pontos da área sejam percorridos. 

O robô em movimento segue uma sequência de funções que indica como deverá se comportar. Eventos detectados durante as varreduras (presença de paredes, objetos próximos, rotações de 90°) ativam _flags_ que, por sua vez, marcam a transição das funções. 

Continuar funcionamento... 

Vídeo de funcionamento: ...

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

## Quem?

* Amanda Malimpensa | [AmandaMK015](https://github.com/AmandaMK015)
* Camila Quadros | [millamsq](https://github.com/millamsq)
* Rafael Matumoto | [rmatumoto](https://github.com/rmatumoto)
* Ricardo Peixoto | [Nemly23](https://github.com/Nemly23)

## Andamento do projeto

- [x] Comunicação entre placa e host - SSH.
- [x] Conserto da biblioteca da [PWM](https://github.com/yigityuce/BlackLib) (incompatibilidade com kernel 3.7+).
- [x] PWM.
- [x] Ponte H. Alimentação pela BBB não é suficiente; colocada uma bateria de 7.4 V e um regulador de tensão (step down) para adequar entrada.
- [x] Sensores ultrassônicos em funcionamento. 
- [x] Lógica de funcionamento/varredura do espaço.
- [x] Leitura do magnetômetro e giroscópio.
- [x] Integração dos componentes.
- [x] Vídeo relatório.
