# ROBÔ ASPIRADOR DE PÓ...

Projeto desenvolvido para disciplina Sistemas Embarcados.

![O robô](/imagens/robo.jpeg)

## ... que não aspira pó de verdade.
Queremos simular o funcionamento de um aspirador de pó autônomo comercial: fazer uma varredura inteligente por todo o cômodo, evitando objetos e paredes.

![Varredura](/imagens/funcionamento.jpg | width=100)

## Como?
Uma IMU para localização e navegação no ambiente de trabalho e quatro sensores ultrassônicos para detecção de objetos/colisões.

## Quem?

* Amanda Malimpensa | [AmandaMK015](AmandaMK015)
* Camila Quadros | []()
* Rafael Matumoto | [rmatumoto](https://github.com/rmatumoto)
* Ricardo Peixoto | [Nemly23](https://github.com/Nemly23)

## Andamento do projeto

- [x] Comunicação entre placa e host - SSH.
- [x] Conserto da biblioteca da [PWM](https://github.com/yigityuce/BlackLib) (incompatibilidade com kernel 3.7+).
- [x] Teste do PWM; problema com nível da tensão: 3.3V e não 5, como desejado. Resolvido com conversor de nível lógico 3.3/5V.
- [x] Ponte H. Alimentação pela BBB não é suficiente; colocada uma bateria de 8 V e um regulador de tensão (step down) para adequar entrada.
- [x] Sensores ultrassônicos em funcionamento. 
- [x] Lógica de funcionamento/varredura do espaço.
- [x] Leitura do magnetômetro e giroscópio.
- [x] Integração dos componentes.
- [ ] Vídeo relatório.
