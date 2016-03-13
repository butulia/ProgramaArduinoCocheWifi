#include <Servo.h>
#include <DistanceSRF04.h>
DistanceSRF04 SensorDistancia;
const int MIAtras = 2;
int MIAdelante = 3;
int MDAtras = 4;
int MDAdelante = 5;
int LED1 = 6;
int LED2 = 7;
int val, kkl, lkf, letra;
int lx = 90;
int distance;
const char InicioComando = '.';
const char ComandoEstablecerServoVertical = 'v';
const char ComandoEstablecerServoHorizontal = 'h';
const char EstablecerPotenciaMaxima = 'p';
const char EstablecerPotenciaRelativaIzquierdo = 'i';
const char EstablecerPotenciaRelativaDerecho = 'd';
const char EstablecerPotenciaGiro = 'g';
const int PotenciaMinimaMotor = 15;
const int DistanciaMinima = 6;
const long SegundosInicializacionWIFI = 40;

int potenciaMotor = 100;
int potenciaIzquierdo = 100;
int potenciaDerecho = 100;
int potenciaGiro = 50;
boolean estaAvanzando = false;
boolean inicializadaWIFI = false;

//Servos
Servo servoX; 
Servo servoY; 
int mkk;

///////////////////////////////////////
int potenciaRelativaMotor(int potenciaRelativa)
{
  return map(potenciaMotor, 0, 100, 0, potenciaRelativa);
}
int calcularCicloMotor(int porcentajePotencia)
{
  int dutyCicle = map(porcentajePotencia, 0, 100, 0, 255);
  if(dutyCicle < 0)
    return 0;
  else if(dutyCicle > 255)
    return 255;
  else
    return dutyCicle;
}
void parar(void)
{
  digitalWrite(MIAtras, LOW);
  digitalWrite(MIAdelante, LOW);
  digitalWrite(MDAtras, LOW);
  digitalWrite(MDAdelante, LOW);
  estaAvanzando = false;
  Serial.println("Parar");
}
void avanzar(void)
{
  digitalWrite(MIAtras, LOW);
  digitalWrite(MDAtras, LOW);
  //digitalWrite(MIAdelante, HIGH);
  //digitalWrite(MDAdelante, HIGH);
  analogWrite(MIAdelante, calcularCicloMotor(potenciaRelativaMotor(potenciaIzquierdo)));
  analogWrite(MDAdelante, calcularCicloMotor(potenciaRelativaMotor(potenciaDerecho)));
  estaAvanzando = true;
  
  Serial.print("Avanzando al ");Serial.print(potenciaMotor);Serial.println(" %");
  Serial.print("Ciclo de trabajo izquierdo ");Serial.print(calcularCicloMotor(potenciaRelativaMotor(potenciaIzquierdo)));Serial.println("/255");
  Serial.print("Ciclo de trabajo derecho ");Serial.print(calcularCicloMotor(potenciaRelativaMotor(potenciaDerecho)));Serial.println("/255");
}
void retroceder(void)
{
  //digitalWrite(MIAtras, HIGH);
  analogWrite(MIAtras, calcularCicloMotor(potenciaRelativaMotor(potenciaIzquierdo)));
  digitalWrite(MIAdelante, LOW);
  //digitalWrite(MDAtras, HIGH);
  analogWrite(MDAtras, calcularCicloMotor(potenciaRelativaMotor(potenciaDerecho)));
  digitalWrite(MDAdelante, LOW);
  Serial.println("Retrocediendo");
}
void derecha(void)
{
  digitalWrite(MIAtras, LOW);
  //digitalWrite(MIAdelante, HIGH);
  analogWrite(MIAdelante, calcularCicloMotor(potenciaGiro));
  //digitalWrite(MDAtras, HIGH);
  analogWrite(MDAtras, calcularCicloMotor(potenciaGiro));
  digitalWrite(MDAdelante, LOW);
}
void izquierda(void)
{
  //digitalWrite(MIAtras, HIGH);
  analogWrite(MIAtras, calcularCicloMotor(potenciaGiro));
  digitalWrite(MIAdelante, LOW);
  digitalWrite(MDAtras, LOW);
  //digitalWrite(MDAdelante, HIGH);
  analogWrite(MDAdelante, calcularCicloMotor(potenciaGiro));
}

void servoX_write(int grados)
{
  if (grados < 170 && grados > 10)
    servoX.write(grados);
  else if (grados <= 10)  servoX.write(10);
  else servoX.write(170);
}

void servoY_write(int grados)
{
  if (grados < 170 && grados > 10)
    servoY.write(grados);
  else if (grados <= 10)  servoY.write(10);
  else servoY.write(170);
}

void servo_right(int grados)
{
  servoX_write(servoX.read() - grados);
}

void servo_left(int grados)
{
  servoX_write(servoX.read() + grados);
}

void servo_down(int grados)
{
  servoY_write(servoX.read() + grados);
}

void servo_up(int grados)
{
  servoY_write(servoX.read() - grados);
}

void esperarASerial()
{
  int count=0;
  while(count < 20 && !Serial.available())
  {
    delay(1);
    count++;
  }
}

void procesar_comando()
{
  int potencia = 0;
  //Serial.println("Entra a proceasar comando");
  esperarASerial();
  
  lkf = Serial.read();
  //Serial.print("leido en procesar comando ");
  //Serial.println(lkf);
  switch (lkf)
  {
    case ComandoEstablecerServoVertical:   
      //Serial.println("entra en vertical "); 
      servoY_write(180 - leer_numero_serial());
      break;
    case ComandoEstablecerServoHorizontal:
      //Serial.println("entra en horizontal ");
      servoX_write(180 - leer_numero_serial());
      break;
    case EstablecerPotenciaMaxima:
      potencia = leer_numero_serial();
      if(potencia > 0 && potencia < PotenciaMinimaMotor)
      {
        potenciaMotor = PotenciaMinimaMotor;
      }
      else if (potencia > 100)
      {
        potenciaMotor = 100;
      }
      else
      {
        potenciaMotor = potencia;
      }
      Serial.print("Potencia de motor establecida al ");Serial.print(potenciaMotor);Serial.println(" %");
      break;
    case EstablecerPotenciaRelativaIzquierdo:
      potenciaIzquierdo = leer_numero_serial();
      Serial.print("Potencia relativa del motor izquiedo ");Serial.print(potenciaIzquierdo);Serial.println(" %");
      break;
    case EstablecerPotenciaRelativaDerecho:
      potenciaDerecho = leer_numero_serial();
      Serial.print("Potencia relativa del motor derecho ");Serial.print(potenciaDerecho);Serial.println(" %");
      break;
    case EstablecerPotenciaGiro:
      potencia = leer_numero_serial();      
      if(potencia > 0 && potencia < PotenciaMinimaMotor)
      {
        potenciaGiro = PotenciaMinimaMotor;
      }
      else if (potencia > 100)
      {
        potenciaGiro = 100;
      }
      else
      {
        potenciaGiro = potencia;
      }
      Serial.print("Potencia de giro ");Serial.print(potenciaGiro);Serial.println(" %");
      break;
  }
}

// Si no lee nada devuelve 0
int leer_numero_serial()
{
  //Serial.println("entra en leer numero ");
  int lectura= -1;
  int numero = 0;
  int digito;
  
  esperarASerial();
  lectura = Serial.read();
  //Serial.print("primera lectura  ");
  //Serial.println(lectura);
  
  while(lectura != -1)
  {
    digito = lectura - '0';
    //Serial.print("digito  ");
    //Serial.println(digito);
    numero = (numero * 10) + digito;
    //Serial.print("numero  ");
    //Serial.println(numero);
    esperarASerial();
    lectura = Serial.read();
    //Serial.print("Siguiente lectura  ");
    //Serial.println(lectura);
  }
  
  return numero;
}


void setup()
{
  SensorDistancia.begin(8, 9);
  servoX.attach(10);//水平舵机接10脚
  servoY.attach(11);//上下舵机接11脚
  servoX.write(90);//输出舵机初始位置为90度
  servoY.write(90);//输出舵机初始位置为90度
  pinMode(MIAtras, OUTPUT);
  pinMode(MIAdelante, OUTPUT);
  pinMode(MDAtras, OUTPUT);
  pinMode(MDAdelante, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  inicializadaWIFI = false;
  estaAvanzando = false;
  for (kkl = 0; kkl < SegundosInicializacionWIFI; kkl++)
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    delay(500);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    delay(500);
  }
  Serial.begin(9600);
  lkf = 0;
}

void loop()
{
  /*
  distance = SensorDistancia.getDistanceCentimeter();
  if (distance <= 5 & distance > 1)
  {
    izquierda();
    delay(100);
    avanzar();
    distance = 0;
  }
  if(millis() % 5000 == 0)
  {
    distance = SensorDistancia.getDistanceCentimeter();
    Serial.print("Distancia: ");Serial.print(distance);Serial.println(" cm");
  }*/
  
  
  if(!inicializadaWIFI && millis() > (SegundosInicializacionWIFI * 1000))
  {
    for (kkl = 0; kkl < 30; kkl++)
    {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      delay(50);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      delay(50);
    }
    inicializadaWIFI = true;    
  }
  
  
  if(estaAvanzando)
  {
    distance = SensorDistancia.getDistanceCentimeter();
    if(distance <= DistanciaMinima)
    {
      parar();
    }
  }
  
  if (Serial.available())
  {
    lkf = 0;
    lkf = Serial.read();
    //Serial.print("leido: ");
    //Serial.println(lkf);
    switch (lkf)
    {
      case 'w':
        avanzar();
        break;
      case 's':
        retroceder();
        break;
      case 'd':
        derecha();
        break;
      case 'a':
        izquierda();
        break;
      case '0':
        parar();
        break;

      case 'i':
        servo_up(2);
        break;
      case 'k':
        servo_down(2);
        break;
      case 'j':
        servo_left(2);
        break;
      case 'l':
        servo_right(2);
        break;

      case 'n':
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, HIGH);
        break;
      case 'm':
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        break;
        
      case InicioComando:
        procesar_comando();
        break;
    }
  }
}
