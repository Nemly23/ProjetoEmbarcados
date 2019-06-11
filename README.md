# ROBÔ ASPIRADOR DE PÓ...

Projeto desenvolvido para disciplina Sistemas Embarcados.

![O robô](/imagens/robo.jpeg)

## ... que não aspira pó de verdade.
Queremos simular o funcionamento de um aspirador de pó autônomo comercial: fazer uma varredura inteligente por todo o cômodo, evitando objetos e paredes.

![Varredura](/imagens/funcionamento.jpg)

## Como?
Uma IMU (MinIMU-9 v2, POLOLU) para localização e navegação no ambiente de trabalho e três sensores ultrassônicos (HC-SR04) para detecção de objetos/colisões.

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
