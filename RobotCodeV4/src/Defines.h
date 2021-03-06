#ifndef DEFINES_H
#define DEFINES_H

#define PRACTICE_BOT 1

//change to 0 for joys
//right click robotcodev2
//run as
//wpilib c++ deploy
#define USING_GAMEPAD 0
#define USING_MXP 0
#define USING_GYRO 1


#define Tal_Drive_Left_1 0
#define Tal_Drive_Left_2 1

#define Tal_Drive_Right_1 2
#define Tal_Drive_Right_2 3

#define Tal_ArmShooter_Left 4
#define Tal_ArmShooter_Right 5
#define Tal_ArmLifter 6
#define Tal_ArmTurret 7

#define Tal_Intake_Roller 8
#define Tal_Intake_Lift 9


#define Sol_Shifter_In 0 //On Solenoid block tubes plug into port 1
#define Sol_Shifter_Out 1 //On Solenoid block tubes plug into port 6

#define Sol_PTO_Enable 2 //On Solenoid block tubes plug into port 2
#define Sol_PTO_Disable 3 //On Solenoid block tubes plug into port 5

#define Sol_Shot_Extend 4 //On Solenoid block tubes plug into port 3
#define Sol_Shot_Retract 5 //On Solenoid block tubes plug into port 4


#define Encoder_Drive_Left_1 0
#define Encoder_Drive_Left_2 1
#define Encoder_Drive_Right_1 2
#define Encoder_Drive_Right_2 3

#define Encoder_Arm_Turret_1 4
#define Encoder_Arm_Turret_2 5

#define Encoder_Arm_Lift_1 6
#define Encoder_Arm_Lift_2 7

#define Encoder_Intake_Lift_1 8
#define Encoder_Intake_Lift_2 9


#define Preset_Intake_Floor -500
#define Preset_Intake_Defense -10
#define Preset_Intake_Intake -435//-448
#define Preset_Intake_Down -500     // value to use when reseting the intake encoder in-match
#define Preset_Intake_Tower_Shot -195
#define INTAKE_LIFT_MIN -500
#define INTAKE_LIFT_MAX 10

#define Preset_Arm_Floor -5850
#define Preset_Arm_Defense -795
#define Preset_Arm_Intake -5760
#define Preset_Arm_Back_Shot -2826
#define Preset_Arm_Far_Shot -2826
#define Preset_Arm_Tower_Shot -1275
#define Preset_Arm_Safe_Zone -5190
#define Preset_Arm_Down -5700		// value to use when resetting the arm lift encoder in-match

#define Preset_Tower_Shot_Shooter_Wheels -0.45f

#define Intake_Lift_Limit_Switch 10
#define TURRET_CENTER_LIMIT_SWITCH 11

//
//  Encoder values to limit the arm movement to keep the arm within the 15" envelope around the robot
//

// The turret is zero'd when it is centered pointing forward.
#define ARM_TURRET_DEGREES_PER_TICK (90.0f / (1210.0f* (60.0f/24.0f)))
#define ARM_TURRET_MIN_ENCODER	(-1650 * (60.0f/24.0f))   // we need a shot to the left in the spybot two-ball at -1500
#define ARM_TURRET_MAX_ENCODER	(1200 * (60.0f/24.0f))

#define ARM_TURRET_CORRECTION_P 0.001f   // motor power per tick to correct invalid turret configurations
#define ARM_LIFT_CORRECTION_P 0.05f	    // motor power per degree to correct invalid lift configurations

#define ARM_LIFT_DEGREES_PER_TICK   (90.0f / 5000.0f)
#define ARM_LIFT_MIN_WHEN_CENTERED 		-20.5f
#define ARM_LIFT_MAX_WHEN_CENTERED 		 120.0f //140.8f
#define ARM_LIFT_MIN_WHEN_FWD 			15.0f
#define ARM_LIFT_MAX_WHEN_FWD 			120.0f  //140.8f
#define ARM_LIFT_MIN_WHEN_SIDEWAYS  	20.0f
#define ARM_LIFT_MAX_WHEN_SIDEWAYS		147.0f  //120.0f

#endif

