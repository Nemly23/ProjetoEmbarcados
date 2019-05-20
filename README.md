# ROBÔ ASPIRADOR DE PÓ...

Projeto desenvolvido para disciplina Sistemas Embarcados.

## ... que não aspira pó de verdade.
Queremos simular o funcionamento de um aspirador de pó autônomo comercial: fazer uma varredura inteligente por todo o cômodo, evitando objetos e paredes.

## Como?
Um magnetômetro para localização no ambiente e quatro sensores ultrassônicos para detecção de objetos/colisões.

## Andamento do projeto

### Tarefas completas

* Comunicação entre placa e host - SSH.
* Conserto da biblioteca da [PWM](https://github.com/yigityuce/BlackLib) (incompatibilidade com kernel 3.7+).
* Teste do PWM; problema com nível da tensão: 3.3V e não 5, como desejado. Resolvido com conversor de nível lógico 3.3/5V.
* Ponte H. Alimentação pela BBB não é suficiente; colocada uma bateria de 8 V e um regulador de tensão (step down) para adequar entrada.
* Sensor ultrassônico em funcionamento! 
* Lógica de funcionamento.

### Próximos desafios

* Leitura do magnetômetro (I2C).
