#include "MyRobot.h"

class ShutdownJetsonCommand : public Command
{
public:
	ShutdownJetsonCommand() {}
	virtual ~ShutdownJetsonCommand() {}

protected:
	virtual void Initialize()
	{
		RobotDemo::Get()->Shutdown_Jetson();
	}
	virtual void Execute() {

	}
	virtual bool IsFinished() { return false;}
	virtual void End() {}
	virtual void Interrupted() {}

};

RobotDemo * RobotDemo::TheRobot = NULL;



RobotDemo::RobotDemo(void)
{
	TheRobot = this;

	//printf("Make Smartdash\r\n");
	SmartDashboard::init();

	commandLeft = 0;
	commandArmShooter = 0;
	commandRight = 0;
	commandLift = 0;
	commandTurret = 0;
	commandintake = 0;
	commandintakelift = 0;
	connectionattempts = 0;
	intele = 0;
	leftStick = new Joystick(0);
	rightStick = new Joystick(1);			// create the joysticks
	turretStick = new Joystick(2);
	DriveTrain = new Drivetrain();
	Intake = new IntakeClass();
	Arm = new ArmClass();
	CollManager = new CollisionManager(Intake, Arm);
	printf("Basic Initialization\r\n");
	TargClient = new TargetingSystemClient();
	TargClient->Connect(JETSON_IP,JETSON_PORT);
	printf("TargClient Initialized\r\n");
	AutonomousControl = new Auton(DriveTrain,Arm,&DriverStation::GetInstance(),Intake,CollManager,TargClient);

	Comp = new Compressor();
	Comp->SetClosedLoopControl(true);
	CompRelay = new Relay(1,Relay::Direction::kBothDirections);

	ReConnectTimer = new Timer();
	ReConnectTimer->Reset();
	ReConnectTimer->Start();

	SmartDashTimer = new Timer();
	SmartDashTimer->Reset();
	SmartDashTimer->Start();

	SafeTimer = new Timer();
	SafeTimer->Reset();
	SafeTimer->Start();

	ArmIntakeTimer = new Timer();
	ArmIntakeTimer->Reset();
	ArmIntakeTimer->Start();

	MatchTimer = new Timer();
	LightOutputPwm = new PWM(12);

	JetsonConnected = false;

	prevIntakeArm = false;
	curIntakeArm = false;

	ConnectionPrevTog = false;
	ConnectionCurTog = false;

	DisConnectionPrevTog = false;
	DisConnectionCurTog = false;

	Auto_Index = 0;
	m_ScriptSystem = 0;
	Init_Script_System();

	int connectionattempts = 0;
	printf("Will block to connect... \r\n");
	while ((connectionattempts < 5) && (TargClient->Get_Connected()  == false))
	{
		if(ReConnectTimer->Get() > 2)
		{
			TargClient->Connect(JETSON_IP,JETSON_PORT);
			ReConnectTimer->Reset();
			connectionattempts++;
		}
	}

	//std::shared_ptr<USBCamera> camera(new USBCamera("cam0", true));
	//camera->SetExposureManual(50); // change this value
	//camera->SetBrightness(20); // change this value
	//CameraServer::GetInstance()->StartAutomaticCapture("cam0");
}

void RobotDemo::Jetson_Connection()
{
	if(TargClient->Get_Connected() == false && ReConnectTimer->Get() > 5)
	{
		TargClient->Connect(JETSON_IP,JETSON_PORT);
		ReConnectTimer->Reset();
	}
	JetsonConnected = true;
}
RobotDemo::~RobotDemo(void)
{
	TheRobot = NULL;
}
void RobotDemo::Shutdown_Jetson(void)
{
	TargClient->Shutdown_Jetson();
}
void RobotDemo::Disabled()
{
	while(IsDisabled())
	{
		Jetson_Connection();
		Send_Smartdashboard_Data();
	}
	Wait(0.001);
}
void RobotDemo::LightUpdate()
{
	if(Arm->GetLifterEncoder() > 0)
	{
		TargClient->FlipEnable();
	}
	else
	{
		TargClient->FlipDisable();
	}
	//when the arm is low, we turn off tracking and clear image
	if(Arm->GetLifterEncoder() < Preset_Arm_Safe_Zone)
	{
		TargClient->EqualizeEnable();
	}
	else
	{
		TargClient->EqualizeDisable();
	}
	if(Arm->GetLifterEncoder() < Preset_Arm_Safe_Zone)
	{
		Arm->TargetingLights->Set(Relay::kOff);
	}
	else
	{
		Arm->TargetingLights->Set(Relay::kForward);
	}
}
void RobotDemo::Autonomous(void)
{
	//DriveTrain->gyro->Reset();
		//HRLogger::Log("FMS ATTATCHED %d\n",DriverStation::GetInstance()->IsFMSAttached());
		//HRLogger::Log("AUTO BEGIN \n");
		Load_Scripts();
		printf("loaded\n");
		Arm->isauto = true;
		m_ScriptSystem->Run_Auto_Script(0);
		printf("ransettings\n");
		AutonomousControl->Auto_Start();
		printf("startfunction\n");
		m_ScriptSystem->Run_Auto_Script(Auto_Index);
		//HRLogger::Log("runauto\n");
		AutonomousControl->Auto_End();
		//HRLogger::Log("autoend\n");
}
void RobotDemo::UpdateInputs()
{
	commandLeft = leftStick->GetY();
	commandRight = rightStick->GetY();
	commandLift = turretStick->GetY()*.85f;
	commandTurret = turretStick->GetX(); //*.5f
	prevIntakeArm = curIntakeArm;
	curIntakeArm = rightStick->GetTrigger();

	//commandArmShooter = turretStick->GetZ();
	commandArmShooter = 0;


	if(CollManager->currentMode == CollisionManager::RobotMode::Intake)
	{
		if(curIntakeArm == true && prevIntakeArm == false)
		{
			Arm->ArmPIDController->Enable();
			Arm->SetArm(Preset_Arm_Intake);
			SafeTimer->Reset();
			SafeTimer->Start();
			ArmIntakeTimer->Reset();
			ArmIntakeTimer->Start();
		}
		else if(curIntakeArm == false && prevIntakeArm == true)
		{
			if(ArmIntakeTimer->Get() > .02f)
			{
				Arm->ArmPIDController->Enable();
				Arm->SetArm(Preset_Arm_Safe_Zone);
				SafeTimer->Reset();
				SafeTimer->Start();
				ArmIntakeTimer->Reset();
				ArmIntakeTimer->Start();
			}
		}
	}
	if(SafeTimer->Get() > 1.0)
	{
		Arm->ArmPIDController->Disable();
		SafeTimer->Reset();
		SafeTimer->Stop();
	}
	if(rightStick->GetTrigger())
	{
		commandintake = -1.0f;
		commandArmShooter = .85f;
	}
	else if(rightStick->GetRawButton(3))
	{
		commandintake = 0.5f;
	}
	else
	{
		commandintake = 0;
	}
	if(leftStick->GetRawButton(3))
	{
		commandintakelift = -.7f;
	}
	else if(leftStick->GetRawButton(2))
	{
		commandintakelift = .7f;
	}
	else
	{
		commandintakelift = 0;
	}
	if(turretStick->GetRawButton(6))
	{
		// when the arm is raised to a batter shot, if we are not ignoring encoders, use low speed
		if ((Arm->BypassEncoderLimits == false) && (fabs(Arm->GetLifterEncoder()) < 1300))
		{
			commandArmShooter = Preset_Tower_Shot_Shooter_Wheels;
		}
		else
		{
			commandArmShooter = -1.0f;
		}
	}
	else if(turretStick->GetRawButton(7))
	{
		commandArmShooter = 1.0f;
	}
	/*if(turretStick->GetRawButton(6))
	{
		commandArmShooter = (turretStick->GetZ()-1);
	}
	else if(turretStick->GetRawButton(7))
	{
		commandArmShooter = (-turretStick->GetZ()+1);
	}*/
}

void RobotDemo::Send_Smartdashboard_Data(void)
{
	if(SmartDashTimer->Get() > .1f)
	{
		SmartDashTimer->Reset();

		DriveTrain->SendData();
		Intake->SendData();
		Arm->SendData();
		CollManager->SendData();

		SmartDashboard::PutBoolean("Jetson Connection", TargClient->Get_Connected());
		SmartDashboard::PutData("Shutdown Jetson",new ShutdownJetsonCommand());
		SmartDashboard::PutNumber("INTAKELIFT",commandintakelift);
		SmartDashboard::PutNumber("x",TargClient->Get_Target_Distance());
		SmartDashboard::PutNumber("y",TargClient->Get_Target_Angle());
		SmartDashboard::PutBoolean("Limit Enabled", Arm->BypassEncoderLimits);
		SmartDashboard::PutNumber("GetZ", turretStick->GetZ());
		if(MatchTimer != NULL)
		{
			SmartDashboard::PutNumber("MATCHTIMER",MatchTimer->Get());
		}
	}
}
void RobotDemo::ResetState()
{
	Arm->ArmPIDController->Reset();
	Arm->TurretPIDController->Reset();
	Intake->LiftPIDController->Reset();
	Arm->ArmPIDController->Disable();
	Arm->TurretPIDController->Disable();
	Intake->LiftPIDController->Disable();
	CollManager->transitioning = false;
	CollManager->currentMode = CollisionManager::Free;
	Arm->isauto = false;
	DriveTrain->StandardTank(0,0);
	Arm->Tele_Start();

}
void RobotDemo::OperatorControl(void)
{

	ResetState();
	//MatchTimer->Reset();
	while (IsOperatorControl())
	{
		int prevtele = intele;
		intele  = 1;
		if(prevtele == 0 && intele == 1)
		{
			MatchTimer->Reset();
			MatchTimer->Start();
		}


		if(MatchTimer->Get() > 134.9)
		{
			if(DriverStation::GetInstance().IsFMSAttached())
			{
				Shutdown_Jetson();
			}
		}

		// If we are disabled in OperatorControl, make sure the jetson connection is live
		// This will only retry the connection once every few seconds.
		if (IsDisabled())
		{
			ResetState();
			Jetson_Connection();
		}
		//Test the lights
		float val = 2000.0f * 0.5f*(turretStick->GetZ() + 1.0f);
		LightOutputPwm->SetRaw((short)val);


		Send_Smartdashboard_Data();
		UpdateInputs();
		TargClient->Update();
		DriveTrain->StandardTank(-commandLeft, -commandRight);
		Arm->Update(commandLift, -commandArmShooter, -commandTurret, turretStick->GetTrigger(), turretStick->GetRawButton(11),
				turretStick->GetRawButton(2),TargClient->Get_Target_Distance(),TargClient->Get_Target_Angle(),
				TargClient->Get_Cal_X(),TargClient->Get_Cal_Y(), TargClient->Get_TargetArea());
		Arm->UpdateEmergency(turretStick->GetRawButton(8));
		Intake->Update(-commandintake, -commandintakelift);
		DriveTrain->Shifter_Update(leftStick->GetTrigger(), leftStick->GetRawButton(10),leftStick->GetRawButton(11));
		DriveTrain->PTO_Update(leftStick->GetRawButton(4));
		CollManager->Update(turretStick->GetRawButton(3), rightStick->GetRawButton(4), leftStick->GetRawButton(5), turretStick->GetRawButton(5));
		if(turretStick->GetRawButton(11))
		{
			TargClient->StartCalibrate();
		}
		if (turretStick->GetRawButton(8) && turretStick->GetRawButton(9))
		{
			Shutdown_Jetson();
		}

		if (turretStick->GetRawButton(9))
		{
			// Reset encoders assuming arm is in the UP postion and Intake is UP
			// TRUE ZERO.
			printf("User REZERO high!\r\n");
			Arm->ResetEncoderLifter();
			Arm->ResetEncoderTurret();
			Intake->ResetEncoderLift();
		}
		if (turretStick->GetRawButton(10))
		{
			printf("User REZERO low!\r\n");

			// BEWARE! this code-path for 'emergency' use IN-MATCH and assumes
			// the arm and intakes are in the DOWN position.
			Arm->ResetEncoderTurret();

			// New RESET, arm down, intake down
			Arm->ResetEncoderLifterDown();
			Intake->ResetEncoderLiftDown();

			DriveTrain->ResetEncoders_Timers();
			DriveTrain->Zero_Yaw();
		}
		ConnectionUpdate();
		LightUpdate();
		CompressorUpdate();
		Wait(0.001);
	}
}
void RobotDemo::CompressorUpdate()
{
	if(Comp->Enabled())
	{
		CompRelay->Set(Relay::Value::kForward);
	}
	else
	{
		CompRelay->Set(Relay::Value::kOff);
	}
}
void RobotDemo::ConnectionUpdate()
{
	ConnectionPrevTog = ConnectionCurTog;
	ConnectionCurTog = rightStick->GetRawButton(8);
	DisConnectionPrevTog = DisConnectionCurTog;
	DisConnectionCurTog = rightStick->GetRawButton(9);

	if((!ConnectionPrevTog) && (ConnectionCurTog))
	{
		TargClient->Connect(JETSON_IP,JETSON_PORT);
	}
	if((!DisConnectionPrevTog) && (DisConnectionCurTog))
	{
		TargClient->Disconnect();
	}
}
START_ROBOT_CLASS(RobotDemo);
