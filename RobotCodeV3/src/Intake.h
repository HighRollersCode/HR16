/*
 * Intake.h
 *
 *  Created on: Jan 30, 2016
 *      Author: 987
 */

#ifndef SRC_INTAKE_H_
#define SRC_INTAKE_H_

#include "WPILib.h"

class IntakeClass
{
public:
	IntakeClass(int Intake_PWM, int IntakeLift_PWM, int LiftEncoder_PWM, int LiftEncoder2_PWM);
	virtual ~IntakeClass();

	//void Intake_In();
	//void Intake_Out();
	//void Intake_Off();
	void Motors(float intake, float intakelift);
	int GetLiftEncoder();
	int GetTurretEncoder();

	Talon *Intake;
	Talon *IntakeLift;

	Encoder *LiftEncoder;

	void SendData();
};

#endif /* SRC_INTAKE_H_ */
