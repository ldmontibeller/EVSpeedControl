/*
 Name:		EVSpeedControl.ino
 Created:	11/22/2017 12:05:09 PM
 Authors:	Andre Martins Severo, Gabriel Germano Maraschin, Leonardo Drews Montibeller
*/

#include <math.h>
#include <AS5040.h>

#define intPin0 0 //interruption 0 is the same as digital pin 2
#define intPin1 1
#define intPin2 2
#define intPin3 3

// PINS OF AS5040
#define CSpin   19
#define CLKpin  20
#define DOpin   21

AS5040 enc(CLKpin, CSpin, DOpin);

float MotorVelocity = 0;
float Angle = 0; //variable to keep track of position
float AccelerationReference = analogRead(A0);
float FirstWheelVelocity = 0, SecondWheelVelocity = 0, ThirdWheelVelocity = 0, FourthWheelVelocity = 0;
float CarWidth = 0, CarLenght = 0; //Car dimensions
volatile unsigned long ElapsedTimeInMicroseconds = 0;
int NumberOfMagneticPoles = 23; //Number of magnetic poles that are measured by the Hall sensor.

void setup()
{
	Serial.begin(9600);
	attachInterrupt(intPin0, ReadTimeMotor1, FALLING);
	//attachInterrupt(intPin1, ReadTimeMotor2, FALLING);
	//attachInterrupt(intPin2, ReadTimeMotor3, FALLING);
	//attachInterrupt(intPin3, ReadTimeMotor4, FALLING);

	if (!enc.begin())
		Serial.println("Error setting up AS5040");
}

void loop()
{
}

// Referência velocidade
// Ler ângulo
// Cálculo do diferencial
// Medição velocidade
// Controle
//Atuação

void VelocityControl()
{
	AccelerationReference = GetAccelerationReference();

	Angle = GetSteeringAngle();

	CalcDifferential(Angle, AccelerationReference);

	GetVelocity();

	Control();
}

double GetAccelerationReference()
{

	AccelerationReference = map(AccelerationReference, 0, 1023, 0, 1);

	return AccelerationReference;

}

float GetSteeringAngle()
{
	Serial.print(enc.read(), HEX);
	Serial.print("   ");

	Serial.print(enc.status(), BIN);
	Serial.print("   ");

	Serial.print(enc.valid() ? "OK" : "Fault");
	Serial.print("   ");

	Serial.println(enc.Zaxis());

	//transforming 10-bit data into degrees
	Angle = ((float)enc.read() / 1024.0) * 360.0;

	Serial.println("ÂNGULO: ");
	Serial.println(Angle);

	delay(400);

	return Angle;

}

float CalcDifferential(float AngleToCalc, float AccelerationToCalc)
{
	FirstWheelVelocity = AccelerationToCalc * (1 - (tan(AngleToCalc)*(CarWidth / CarLenght)));
	SecondWheelVelocity = AccelerationToCalc * (1 + (tan(AngleToCalc)*(CarWidth / CarLenght)));
	ThirdWheelVelocity = AccelerationToCalc * (1 - (tan(AngleToCalc)*(CarWidth / (2 * CarLenght))));
	FourthWheelVelocity = AccelerationToCalc * (1 + (tan(AngleToCalc)*(CarWidth / (2 * CarLenght))));

	return FirstWheelVelocity, SecondWheelVelocity, ThirdWheelVelocity, FourthWheelVelocity;

}

float GetVelocity() //Airspeed of a unladed swallow
{
	noInterrupts();

	static unsigned long PreviousElapsedTime = 0;

	if (PreviousElapsedTime != ElapsedTimeInMicroseconds)
	{
		MotorVelocity = (pow(ElapsedTimeInMicroseconds, -1)*60000000.0) / NumberOfMagneticPoles;

		Serial.print(MotorVelocity, 3);
		Serial.print(" RPM\r\n");

		PreviousElapsedTime = ElapsedTimeInMicroseconds;
	}

	else
	{
		Serial.print("0 RPM\r\n");
	}

	return MotorVelocity;

	interrupts();

	delay(500);
}

void ReadTimeMotor1()
{
	static unsigned long PreviousTimeInMicroseconds = 0;
	unsigned long ThisTime = micros();

	ElapsedTimeInMicroseconds = ThisTime - PreviousTimeInMicroseconds;
	PreviousTimeInMicroseconds = ThisTime;
}

//float ReadTimeMotor2()
//{
//	static unsigned long PreviousTimeInMicroseconds = 0;
//	unsigned long ThisTime = micros();
//
//	ElapsedTimeInMicroseconds = ThisTime - PreviousTimeInMicroseconds;
//	PreviousTimeInMicroseconds = ThisTime;
//}
//
//float ReadTimeMotor3()
//{
//	static unsigned long PreviousTimeInMicroseconds = 0;
//	unsigned long ThisTime = micros();
//
//	ElapsedTimeInMicroseconds = ThisTime - PreviousTimeInMicroseconds;
//	PreviousTimeInMicroseconds = ThisTime;
//}
//
//float ReadTimeMotor4()
//{
//	static unsigned long PreviousTimeInMicroseconds = 0;
//	unsigned long ThisTime = micros();
//
//	ElapsedTimeInMicroseconds = ThisTime - PreviousTimeInMicroseconds;
//	PreviousTimeInMicroseconds = ThisTime;
//}

void Control()
{

}