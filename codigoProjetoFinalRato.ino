/*Trabalho final da Disciplina Eletronica para Computacao
   SCC0180 - Professor: Eduardo Valle Simoes
   Alunos:  Brendon Hudson Cardoso
            Jonas Wendel
            Alexis de Almeida Coutinho
   Rato robo feito com arduino e sensores que detectam obstaculos e
   a presenca de pessoas e desvia.
*/

#include <Servo.h>

//Servo motor
Servo motor; // Definindo o nome do servo motor
#define pinmotor 8

//Definindo pinos PWM para acionar a tracao do motor
//controlado por uma ponte H
#define frente 10
#define re 11

//Sensor Ultrassonico
// Portas de emissao de ondas
const int TRIG1 = 2;  //Frente
const int TRIG2 = 4;  //Direita
const int TRIG3 = 6;  //Esquerda

// Portas de recepccao de ondas
const int ECHO1 = 3;  //Frente
const int ECHO2 = 5;  //Direita
const int ECHO3 = 7;  //Esquerda

//Variaveis utilizadas nos calculos de verificacao espacial do Rato
unsigned long tempo = 0;
unsigned long dFrente = 0;
unsigned long dDireita = 0;
unsigned long dEsquerda = 0;

//P/gerar n aleatorio na hora de testar as viradas pra barreiras na frente
long randNumber;
//------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);         // Iniciando a serial
  motor.attach(pinmotor);     // Definindo porta do servo motor
  
  //Sensores ultrassonicos (distancia)
  pinMode(TRIG1, OUTPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(ECHO2, INPUT);
  pinMode(ECHO3, INPUT);

  //Motor de tracao
  pinMode(frente, OUTPUT);
  pinMode(re, OUTPUT);
  randomSeed(analogRead(0));

}

void loop() {
  //Calculo primeiro a distancia de obstaculos nos tres lados
  //para verificar o que fazer
  tempo = calculaTempo(TRIG3, ECHO3); //Esquerda
  dEsquerda = tempoParaCentimetro(tempo);
  Serial.print("E: ");
  Serial.print(dEsquerda);
  Serial.print("\t");

  tempo = calculaTempo(TRIG1, ECHO1); //Da frente
  dFrente = tempoParaCentimetro(tempo);
  Serial.print("F: ");
  Serial.print(dFrente);
  Serial.print("\t");
  
  tempo = calculaTempo(TRIG2, ECHO2); //Direita
  dDireita = tempoParaCentimetro(tempo);
  Serial.print("D: ");
  Serial.print(dDireita);
  Serial.print("\n\n\n");

  //Verifica se ha' obstaculos a sua FRENTE
  if (dFrente <= 45 && dFrente != 0) {
    //Se tiver, verifica o melhor lado para virar
    //em ordem aleatoria
    switch (randNumber) {
      case 0:
        if (dEsquerda >= 30 && dEsquerda != 0) motor.write(70); //Esquerda
        else if (dDireita >= 30) motor.write(110);              //Direita
        //Se nao tiver pra onde virar
        else {
          analogWrite(frente, 0); //Desliga tracao pra frente
          analogWrite(re, 255);   //Liga tracao pra tras
        }
        break;
      case 1:
        if (dDireita >= 30 && dDireita != 0) motor.write(110);  //Direita
        else if (dEsquerda >= 30) motor.write(70);              //Esquerda
        //Se nao tiver pra onde virar
        else {
          analogWrite(frente, 0); //Desliga tracao pra frente
          analogWrite(re, 255);   //Liga tracao pra tras
        }
        break;
    }
    delay(50); 
  }

  //Se nao houver nada na frente, verifica obstaculos que possam
  //estar se aproximando aos lados (direito e esquerdo)

  else if (dEsquerda <= 40 && dEsquerda != 0) {
    motor.write(110); //Vira pra direita
    delay(50);
  }

  else if (dDireita <= 40 && dDireita != 0) {
    motor.write(70); //Vira pra esquerda
    delay(50);
  }

  //Ultimo caso, se qualquer sensor detectar obstaculo muito perto, anda pra tras
  else if (dFrente < 15 || dDireita < 15 || dEsquerda < 15) {
    analogWrite(frente, 0);
    analogWrite(re, 255);
    delay(50);
  }

  //Se nao, continue a nadar, a nadar...*andar
  else {
    //Acerta a direcao do servo
    motor.write(90);
    
    //Caso nao haja obstaculos, Hamtaro continuara andando reto
    analogWrite(re, 0); //Desliga a re' caso esteja ligada
    analogWrite(frente, 255); //Anda para frente
    
    //Calcula numeros randomicos caso haja obstaculos a' frente
    //para aleatoriamente verificar os lados possiveis p/virar
    randNumber = random(0, 2);
    delay(50);
  }
}

// Funcao que converte o tempo para a distancia
long tempoParaCentimetro(unsigned long microseconds) {
  return microseconds / 58;
}

// Funcao que verifica quanto tempo leva pro pulso voltar para o receptor de ondas
long calculaTempo(int TRIG, int ECHO) {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);     //  Enviando o pulso
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  tempo = pulseIn(ECHO, HIGH);  // Recebendo o pulso
  return tempo;
}
