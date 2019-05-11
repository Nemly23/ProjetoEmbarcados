# ROB� ASPIRADOR DE P�...

Projeto desenvolvido para disciplina Sistemas Embarcados.

## ... que n�o aspira p� de verdade.
Queremos simular o funcionamento de um aspirador de p� aut�nomo comercial: fazer uma varredura inteligente por todo o c�modo, evitando objetos e paredes.

## Como?
Uma unidade inercial (IMU) para localiza��o no ambiente e quatro sensores ultrass�nicos para detec��o de objetos/colis�es.

## Andamento do projeto

### Tarefas completas

* Comunica��o entre placa e host - SSH.
* Conserto da biblioteca da [PWM](https://github.com/yigityuce/BlackLib) (incompatibilidade com kernel 3.7+).
* Teste do PWM; problema com n�vel da tens�o: 3.3V e n�o 5, como desejado.
* Ponte H. Alimenta��o pela BBB n�o � suficiente; colocada uma bateria de 8 V e um regulador de tens�o (step down) para adequar entrada.

### Pr�ximos desafios

* **PWM**: circuito com transistor para elevar tens�o para 5 V, mas n�o funcionou bem. Adquirir placas de convers�o de n�vel l�gico para contornar o problema.
* Contagem de tempo para c�lculo da dist�ncia pelo sensor US: por hardware (utilizando as portas de *timer* da BBB) ou usando **PRU** (Programmable Real-Time Unit)