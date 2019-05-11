# ROBÔ ASPIRADOR DE PÓ...

Projeto desenvolvido para disciplina Sistemas Embarcados.

## ... que não aspira pó de verdade.
Queremos simular o funcionamento de um aspirador de pó autônomo comercial: fazer uma varredura inteligente por todo o cômodo, evitando objetos e paredes.

## Como?
Uma unidade inercial (IMU) para localização no ambiente e quatro sensores ultrassônicos para detecção de objetos/colisões.

## Andamento do projeto

### Tarefas completas

* Comunicação entre placa e host - SSH.
* Conserto da biblioteca da [PWM](https://github.com/yigityuce/BlackLib) (incompatibilidade com kernel 3.7+).
* Teste do PWM; problema com nível da tensão: 3.3V e não 5, como desejado.
* Ponte H. Alimentação pela BBB não é suficiente; colocada uma bateria de 8 V e um regulador de tensão (step down) para adequar entrada.

### Próximos desafios

* **PWM**: circuito com transistor para elevar tensão para 5 V, mas não funcionou bem. Adquirir placas de conversão de nível lógico para contornar o problema.
* Contagem de tempo para cálculo da distância pelo sensor US: por hardware (utilizando as portas de *timer* da BBB) ou usando **PRU** (Programmable Real-Time Unit)