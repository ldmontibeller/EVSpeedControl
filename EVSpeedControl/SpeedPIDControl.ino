#include <math.h>
#include <TimerThree.h>
#include <PID_v1.h>

#define PIN_PWM 9
#define HALL_PIN 0 //interruption 0 is the same as digital pin 2
#define PIN_THROTTLE 3 //Ver qual entrada analógica corresponde
#define TOP_SPEED 260 //Top speed desired in RPM

double Setpoint, Input, Output;
double Kp = 2, Ki = 5, Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

unsigned long Analog_Setpoint = 0;
double FanSpeed = 0;
float sample_time = 0.5; //seconds
volatile unsigned long elapsedTimeInMicroseconds = 0;
int numberOfMagneticPoles = 23; //Number of magnetic poles that are measured by the Hall sensor.

void setup()
{
	TCCR5B = TCCR5B & 0b11111000 | 0x01; //timer sem prescaler
	pinMode(PIN_PWM, OUTPUT);
	Serial.begin(9600);
	attachInterrupt(HALL_PIN, readTime, FALLING);
	Timer3.initialize(sample_time*1000000.0);
	Timer3.attachInterrupt(tfPCfan);
	Input = 0.0;
	Analog_Setpoint = analogRead(PIN_THROTTLE);
	myPID.SetMode(AUTOMATIC);
	delay(1000);
}

void loop()
{

}

void tfPCfan()
{
	FanSpeed = (pow(elapsedTimeInMicroseconds, -1)*60000000.0) / numberOfMagneticPoles;

	Setpoint = map(Analog_Setpoint, 0, 1023, 0, TOP_SPEED);
	Input = FanSpeed;
	myPID.Compute();
	analogWrite(PIN_PWM, Output);

	Serial.print(Setpoint);
	Serial.print(" RPM\t");
	Serial.print(FanSpeed, 3);
	Serial.print(" RPM\t\r\n");
}

void readTime()
{
	static unsigned long previousTimeInMicroseconds = 0;
	unsigned long thisTime = micros();
	elapsedTimeInMicroseconds = thisTime - previousTimeInMicroseconds;
	previousTimeInMicroseconds = thisTime;
}
