#Contador de Ciclistas Rev 1

- GPL v3.0
- Através de um Arduino em conjunto com um sensor ultrassonico(HC-SR04) construimos um prototipo capaz de contabilizar o número de ciclistas que passam ao longo de um tempo 't', e salvamos essa quantidade 'n' num cartão SD.
O sistema possui watchdogs para evitar o travamento e loops infinitos e uma bateria que possibilite uma autonomia de pelo menos 24h.
Após os dados serem salvos como .csv, eles podem ser adquiridos a partir do cartão SD que fica no dispositivo.
