
#ifndef MYROBOT_H
#define MYROBOT_H

#include "WPILib.h"

#include "DriveTrain.h"
#include <unistd.h>
//#include "networktables/NetworkTable.h"
//#include "opencv2/opencv.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <pthread.h>



using namespace std;
/*
struct ProgParams
{
	string ROBOT_IP;
	string ROBOT_PORT;
	string CAMERA_IP;
	string IMAGE_FILE;

	bool From_Camera;
	bool From_File;
	bool Visualize;
	bool Timer;
	bool Debug;
	bool Process;
	bool USB_Cam;
};
struct Target
{
	cv::Rect HorizontalTarget;
	cv::Rect VerticalTarget;

	double HorizontalAngle;
	double VerticalAngle;
	double Horizontal_W_H_Ratio;
	double Horizontal_H_W_Ratio;
	double Vertical_W_H_Ratio;
	double Vertical_H_W_Ratio;

	cv::Point HorizontalCenter;
	cv::Point VerticalCenter;

	bool HorizGoal;
	bool VertGoal;
	bool HotGoal;
	bool matchStart;
	bool validFrame;

	//camera bool
	bool cameraConnected;

	int targetLeftOrRight;
	int lastTargerLorR;
	int hotLeftOrRight;
	double targetDistance;

};
*/
class RobotDemo: public SampleRobot
{
public:

	float commandForward;
	float commandArmShooter;
	float commandTurn;
	float commandLift;
	float commandTurret;




	Joystick *rightStick;			// joystick 2
	Joystick *leftStick;			// joystick 1
	Joystick *turretStick;			// joystick 3
	Drivetrain *DriveTrain;

	RobotDemo(void);
	~RobotDemo(void);
/*
	const char * const JAVA = "/usr/local/frc/JRE/bin/java";
	const char * const GRIP_JAR = "/home/lvuser/grip.jar";
	const char * const GRIP_PROJECT = "/home/lvuser/find-contours.grip";

	NetworkTable *grip;
*/
	static RobotDemo * Get() { return TheRobot; }

	void Autonomous(void);
	void UpdateInputs();
	void OperatorControl(void);


	//Threaded Video Capture Function





	protected:

	static RobotDemo * TheRobot;
};

#endif
