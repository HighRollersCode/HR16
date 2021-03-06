#include "MyRobot.h"
#include <math.h>
#include <time.h>
/*
#define _USE_MATH_DEFINES
#define MIN_WIDTH 120
#define Y_IMAGE_RES 240
#define VIEW_ANGLE 34.8665269
#define AUTO_STEADY_STATE 1.9
*/
RobotDemo * RobotDemo::TheRobot = NULL;
/*

void parseCommandInputs(int argc, const char* argv[], ProgParams &params);
void printCommandLineUsage();
void initializeParams(ProgParams& params);
double diffClock(timespec start, timespec end);
cv::Mat ThresholdImage(cv::Mat img);
void findTarget(cv::Mat original, cv::Mat thresholded, Target& targets, const ProgParams& params);
void NullTargets(Target& target);
void CalculateDist(Target& targets);
void *VideoCap(void *args);

float spd = 0;
int minR = 0;
int maxR = 30;
int minG = 80; //160 for ip cam, 80 to support MS webcam
int maxG = 255;
int minB = 0;
int maxB = 30;

//GLOBAL CONSTANTS
const double PI = 3.141592653589793;

//Target Ratio Ranges
double MinHRatio = 1.5;
double MaxHRatio = 6.6;

double MinVRatio = 1.5;
double MaxVRatio = 8.5;

int MAX_SIZE = 255;
int test = 0;
//Some common colors to draw with
const cv::Scalar RED = cv::Scalar(0, 0, 255),
			BLUE = cv::Scalar(255, 0, 0),
			GREEN = cv::Scalar(0, 255, 0),
			ORANGE = cv::Scalar(0, 128, 255),
			YELLOW = cv::Scalar(0, 255, 255),
			PINK = cv::Scalar(255, 0,255),
			WHITE = cv::Scalar(255, 255, 255);



//GLOBAL MUTEX LOCK VARIABLES
pthread_mutex_t targetMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t matchStartMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t frameMutex = PTHREAD_MUTEX_INITIALIZER;


//Thread Variables
pthread_t MJPEG;
pthread_t AutoCounter;


//Store targets in global variable
Target targets;
cv::Mat frame;

//Global Timestamps for auto
struct timespec autoStart, autoEnd;


//Control process thread exectution
bool progRun;*/
	//auto grip = NetworkTable::GetTable("grip");
RobotDemo::RobotDemo(void)
{

	TheRobot = this;

	commandForward = 0;
	commandArmShooter = 0;
	commandTurn = 0;
	commandLift = 0;

	leftStick = new Joystick(0);
	rightStick = new Joystick(1);			// create the joysticks
	turretStick = new Joystick(2);

	DriveTrain = new Drivetrain();
	/*cv::VideoCapture vcap;
	while (!vcap.open(0, 160,120))
	{
		usleep(1000000);
	}
	Wait(1.000);
	vcap.release();
	Wait(1.000);*/
	const char * const JAVA = "/usr/local/frc/JRE/bin/java";
	char *GRIP_ARGS[5] = { "java", "-jar", "/home/lvuser/grip.jar", "/home/lvuser/project.grip", NULL };
	if (fork() == 0)
	{
		if (execv(JAVA, GRIP_ARGS) == -1)
		{
			perror("Error running GRIP");
		}
	}
}
RobotDemo::~RobotDemo(void)
{
	TheRobot = NULL;
}

void RobotDemo::Autonomous(void)
{}
void RobotDemo::UpdateInputs()

{
		//float deadzone = .1;

		commandForward = leftStick->GetY();
		commandTurn = rightStick->GetY();
		commandArmShooter = turretStick->GetZ();
		commandLift = turretStick->GetY();
		//commandTurn = -rightStick->GetRawButton(2);


		/*if(rightStick->GetY() == deadzone || rightStick->GetY() > 0 )
		{
			commandTurn = rightStick->GetY()-rightStick->GetY();
		}
		if(rightStick->GetY() == -deadzone || rightStick->GetY() < 0 )
		{
					commandTurn = rightStick->GetY()+rightStick->GetY();
		}*/
		if(turretStick->GetRawButton(6))
		{
			commandArmShooter = (turretStick->GetZ()-1)*.5f;
		}
		else if(turretStick->GetRawButton(7))
		{
			commandArmShooter = (-turretStick->GetZ()+1)*.5f;
		}
		else
		{
			commandArmShooter = 0;
		}


}
/*
double diffClock(timespec start, timespec end)
{
 return	(end.tv_sec - start.tv_sec) + (double) (end.tv_nsec - start.tv_nsec)/ 1000000000.0f;
}
*/
///////////////////FUNCTIONS/////////////////////

/**
 * This function uses the law of lense projection to
 * estimate the distance to an object of known height only
 * using a single camera.
 *
 * This function uses only the vertical target height, the
 * pixel height of the image, and the view angle of the
 * camera lense.
 */
/*
void CalculateDist(Target& targets)
{
	//vertical target is 32 inches fixed
	double targetHeight = 32.0;

	//get vertical pixels from targets
	int height = targets.VerticalTarget.height;

	//d = Tft*FOVpixel/(2*Tpixel*tanΘ)
	targets.targetDistance = Y_IMAGE_RES * targetHeight
			/ (height * 12 * 2 * tan(VIEW_ANGLE * PI / (180 * 2)));
}
*/
/**
 * This function scans through an image and determins
 * if rectangles exist which match the profile of a
 * "Hot Goal".
 *
 * The "Hot Goal" is specific to the 2014 FRC game
 * and is identified as a horizontal and vertical target
 * in the same frame, with known width and height.
 */
/*
void findTarget(cv::Mat original, cv::Mat thresholded, Target& targets, const ProgParams& params)
{

	cv::vector<cv::Vec4i> hierarchy;
	cv::vector<cv::vector<cv::Point> > contours;

	//Find rectangles
	cv::findContours(thresholded, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	if(params.Debug)
	{
	cout << "Contours: " << contours.size() << endl;
	cout << "Hierarchy: " << hierarchy.size() << endl;
	}

	//run through all contours and remove small contours
	unsigned int contourMin = 6;
	for (cv::vector<cv::vector<cv::Point> >::iterator it = contours.begin();
			it != contours.end();)
	{
		//cout<<"Contour Size: "<<it->size()<<endl;
		if (it->size() < contourMin)
			it = contours.erase(it);

		else
			++it;

	}

	//Vector for Min Area Boxes
	cv::vector<cv::RotatedRect> minRect(contours.size());

	/// Draw contours
	cv::Mat drawing = cv::Mat::zeros(original.size(), CV_8UC3);

	NullTargets(targets);

	//run through large contours to see if they are our targerts
	if (!contours.empty() && !hierarchy.empty())
	{

		for (unsigned int i = 0; i < contours.size(); i++)
		{
			//capture corners of contour
			minRect[i] = cv::minAreaRect(cv::Mat(contours[i]));

			if(params.Visualize)
			{

				//if(hierarchy[i][100] != -1)
				//drawContours(original, contours, i, RED, 2, 8, hierarchy, 0,Point());

				//draw a minimum box around the target in green
				cv::Point2f rect_points[4];
				minRect[i].points(rect_points);
				for (int j = 0; j < 4; j++)
					line(original, rect_points[j], rect_points[(j + 1) % 4], BLUE, 1, 8);
			}
			//define minAreaBox
			cv::Rect box = minRect[i].boundingRect();

			double WHRatio = box.width / ((double) box.height);
			double HWRatio = ((double) box.height) / box.width;

			//check if contour is vert, we use HWRatio because it is greater that 0 for vert target
			if ((HWRatio > MinVRatio) && (HWRatio < MaxVRatio))
			{
				targets.VertGoal = true;
				targets.VerticalTarget = box;
				targets.VerticalAngle = minRect[i].angle;
				targets.VerticalCenter = cv::Point(box.x + box.width / 2, box.y + box.height / 2);
				targets.Vertical_H_W_Ratio = HWRatio;
				targets.Vertical_W_H_Ratio = WHRatio;

			}
			//check if contour is horiz, we use WHRatio because it is greater that 0 for vert target
			else if ((WHRatio > MinHRatio) && (WHRatio < MaxHRatio))
			{
				targets.HorizGoal = true;
				targets.HorizontalTarget = box;
				targets.HorizontalAngle = minRect[i].angle;
				targets.HorizontalCenter = cv::Point(box.x + box.width / 2,	box.y + box.height / 2);
				targets.Horizontal_H_W_Ratio = HWRatio;
				targets.Horizontal_W_H_Ratio = WHRatio;
			}

			if (targets.HorizGoal && targets.VertGoal)
			{
				targets.HotGoal = true;

				//determine left or right
				if (targets.VerticalCenter.x < targets.HorizontalCenter.x) //target is right
					targets.targetLeftOrRight = 1;
				else if (targets.VerticalCenter.x > targets.HorizontalCenter.x) //target is left
					targets.targetLeftOrRight = -1;

				targets.lastTargerLorR = targets.targetLeftOrRight;

			}

			if(params.Debug)
			{
				cout<<"Contour: "<<i<<endl;
				cout<<"\tX: "<<box.x<<endl;
				cout<<"\tY: "<<box.y<<endl;
				cout<<"\tHeight: "<<box.height<<endl;
				cout<<"\tWidth: "<<box.width<<endl;
				cout<<"\tangle: "<<minRect[i].angle<<endl;
				cout<<"\tRatio (W/H): "<<WHRatio<<endl;
				cout<<"\tRatio (H/W): "<<HWRatio<<endl;
				cout<<"\tArea: "<<box.height*box.width<<endl;
			}

			//ID the center in yellow
			cv::Point center(box.x + box.width / 2, box.y + box.height / 2);
			cv::line(original, center, center, YELLOW, 3);
			cv::line(original, cv::Point(320/2, 240/2), cv::Point(320/2, 240/2), YELLOW, 3);

		}
		//if(params.Visualize)
			//imshow("Contours", original); //Make a rectangle that encompasses the target
	}
	else
	{
		cout << "No Contours" << endl;
		targets.targetLeftOrRight = 0;
	}

	if(params.Visualize)
				imshow("Contours", original); //Make a rectangle that encompasses the target

	pthread_mutex_lock(&matchStartMutex);
	if (!targets.matchStart)
		targets.hotLeftOrRight = targets.targetLeftOrRight;
	pthread_mutex_unlock(&matchStartMutex);

}

*/
/**
 * This function performs numerous filtering on
 * a color image in order to only return
 * areas of interest based on their color
 *
 */
/*
cv::Mat ThresholdImage(cv::Mat original)
{
	//Local Temp Image
	cv::Mat thresholded;

	//Threshold image to remove non-green objects
	cv::inRange(original, cv::Scalar(minB, minG, minR), cv::Scalar(maxB, maxG, maxR), thresholded);

	//smooth edges
	cv::blur(thresholded, thresholded, cv::Size(3, 3));

	//Additional filtering if needed
	//Canny(thresholded, thresholded, 100, 100, 3);
	//blur(thresholded, thresholded, Size(5, 5));

	//return image
	return thresholded;

}
*/
/**
 * This functions "zeros", the targets identified
 * so that a clean slate can be used to determine
 *] if the next image contains targets as well.
 */
/*
void NullTargets(Target& target)
{

	target.HorizontalAngle = 0.0;
	target.VerticalAngle = 0.0;
	target.Horizontal_W_H_Ratio = 0.0;
	target.Horizontal_H_W_Ratio = 0.0;
	target.Vertical_W_H_Ratio = 0.0;
	target.Vertical_H_W_Ratio = 0.0;
	target.targetDistance = 0.0;
	target.targetLeftOrRight = 0;
	target.lastTargerLorR = 0;

	target.HorizGoal = false;
	target.VertGoal = false;
	target.HotGoal = false;

}
void initializeParams(ProgParams& params)
{
	params.Debug = false;
	params.From_Camera = false;
	params.From_File = false;
	params.Timer = false;
	params.Visualize = false;
	params.Process = true;
	params.USB_Cam = false;

}
*/
/**
 * This function parses the command line inputs and determines
 * the runtime parameters the program should use as specified
 * by the user.
 */
/*
void parseCommandInputs(int argc, const char* argv[], ProgParams& params)
{
	//todo: define all input flags
	if (argc < 2)
	{ // Check the value of argc. If not enough parameters have been passed, inform user and exit.
		printCommandLineUsage();
		exit(0);
	}
	else
	{ // if we got enough parameters...

		initializeParams(params);

		for (int i = 1; i < argc; i++)
		{ *//* We will iterate over argv[] to get the parameters stored inside.
		 * Note that we're starting on 1 because we don't need to know the
		 * path of the program, which is stored in argv[0] */
/*
			if ((string(argv[i]) == "-f") && (i + 1 < argc)) //read from file
			{
				// We know the next argument *should* be the filename:
				params.IMAGE_FILE = string(argv[i + 1]);
				params.From_Camera = false;
				params.From_File = true;
				i++;
			}
			else if ((string(argv[i]) == "-c") && (i + 1 < argc)) //camera IP
			{
				params.CAMERA_IP = string(argv[i + 1]);
				params.From_Camera = true;
				params.From_File = false;
				params.USB_Cam = false;
				i++;
			}
			else if (string(argv[i]) == "-u") //use USB Camera
			{
				//params.CAMERA_IP = string(argv[i + 1]);
				params.From_Camera = true;
				params.From_File = false;
				params.USB_Cam = true;
			}
			else if ((string(argv[i]) == "-s") && (i + 1 < argc)) //robot TCP SERVER IP
			{
				params.ROBOT_IP = string(argv[i + 1]);
				i++;
			}
			else if ((string(argv[i]) == "-p") && (i + 1 < argc)) //robot TCP SERVER PORT
			{
				params.ROBOT_PORT = string(argv[i + 1]);
				i++;
			}
			else if (string(argv[i]) == "-t") //Enable Timing
			{
				params.Timer = true;
			}
			else if (string(argv[i]) == "-np") //no processing
			{
				params.Process = false;
			}
			else if (string(argv[i]) == "-v") //Enable Visual output
			{
				params.Visualize = true;
			}
			else if (string(argv[i]) == "-debug") //Enable debug output
			{
				params.Debug = true;
			}
			else if (string(argv[i]) == "-d") //Default Params
			{
				params.ROBOT_PORT = string(argv[i + 1]);
				return;
			}
			else if (string(argv[i]) == "-help") //help
			{
				//todo: cout help on commands
				printCommandLineUsage();
				exit(0);
			}
			else
			{
				std::cout
						<< "Not enough or invalid arguments, please try again.\n";
				printCommandLineUsage();
				exit(0);
			}

		}

	}
}

*/
/**
 * This function uses FFMPEG codec apart of openCV to open a
 * MJPEG stream and buffer it. This function should be ran
 * in its own thread so it can run as fast as possibe and store frames.
 *
 * A mutable lock should be used in another thread to copy the latest frame
 *
 * Note: Opening the stream blocks execution. Also
 * Based on my own tests it appears the beaglebone can capture
 * frames at 30fps with 320 x 240 resolution, however
 * the framerate needs to be reduced to allow for processing time.
 *
 * Only run the camera as 10FPS, with a 10kbs limit per frame
 */
/*
void *VideoCap(void *args)
{
	//copy passed in variable to programStruct
	ProgParams *struct_ptr = (ProgParams *) args;

	if (struct_ptr->From_File)
	{
		cout<<"Loading Image from file"<<endl;

		//read img and store it in global variable
		pthread_mutex_lock(&frameMutex);
		frame = cv::imread(struct_ptr->IMAGE_FILE);
		pthread_mutex_unlock(&frameMutex);

		if (!frame.empty())
		{
			cout<<"File Loaded: Starting Processing Thread"<<endl;
			progRun = true;
		}
		else
		{
			cout<<"Error Loading File"<<endl;
			exit(0);
		}


	}

	else if(struct_ptr->From_Camera)
	{
		//create timer variables
		struct timespec start, end, bufferStart, bufferEnd;

		//seconds to wait for buffer to clear before we start main process thread
		int waitForBufferToClear = 12;

		//start timer to time how long it takes to open stream
		clock_gettime(CLOCK_REALTIME, &start);

		cv::VideoCapture vcap;


		// For IP cam this works on a AXIS M1013
		// For USB cam this works on Microsoft HD 3000


		std::string videoStreamAddress;
		if (struct_ptr->USB_Cam)
		{

			int videoStreamAddress = 0; //represents /dev/video0

			std::cout<<"Trying to connect to Camera stream... at: "<<videoStreamAddress<<std::endl;

			int count =1;

			//open the video stream and make sure it's opened
			//We specify desired frame size and fps in constructor
			//Camera must be able to support specified framesize and frames per second
			//or this will set camera to defaults
			while (!vcap.open(videoStreamAddress, 320,240,10.0f))
			{
				std::cout << "Error connecting to camera stream, retrying " << count<< std::endl;
				count++;
				usleep(1000000);
			}

			//After Opening Camera we need to configure the returned image setting
			//all opencv v4l2 camera controls scale from 0.0 - 1.0

			//vcap.set(CV_CAP_PROP_EXPOSURE_AUTO, 1);
			vcap.set(CV_CAP_PROP_EXPOSURE_ABSOLUTE, 0.1);
			vcap.set(CV_CAP_PROP_BRIGHTNESS, 1);
			vcap.set(CV_CAP_PROP_CONTRAST, 0);

			cout<<vcap.get(CV_CAP_PROP_FRAME_WIDTH)<<endl;
			cout<<vcap.get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;

		}
		else //connect to IP Cam
		{
			std::string videoStreamAddress = "http://" + struct_ptr->CAMERA_IP +"/mjpg/video.mjpg";

			std::cout<<"Trying to connect to Camera stream... at: "<<videoStreamAddress<<std::endl;

			int count = 1;

			//open the video stream and make sure it's opened
			//image settings, resolution and fps are set via axis camera webpage
			while (!vcap.open(videoStreamAddress))
			{

				std::cout << "Error connecting to camera stream, retrying " << count<< std::endl;
				count++;
				usleep(1000000);
			}

		}

		//Stream started
		cout << "Successfully connected to Camera Stream" << std::endl;

		//set true boolean
		pthread_mutex_lock(&targetMutex);
		targets.cameraConnected = true;
		pthread_mutex_unlock(&targetMutex);

		//end clock to determine time to setup stream
		clock_gettime(CLOCK_REALTIME, &end);

		cout << "It took " << diffClock(start,end) << " seconds to set up stream " << endl;

		clock_gettime(CLOCK_REALTIME, &bufferStart);


		cout<<"Waiting for stream buffer to clear..."<<endl;


		//run in continuous loop
		while (true)
		{
			//start timer to get time per frame
			clock_gettime(CLOCK_REALTIME, &start);

			//read frame and store it in global variable
			pthread_mutex_lock(&frameMutex);
			vcap.read(frame);
			pthread_mutex_unlock(&frameMutex);

			//end timer to get time per frame
			clock_gettime(CLOCK_REALTIME, &end);


			if(struct_ptr->Timer)
				cout << "It took FFMPEG " << diffClock(start,end) << " seconds to grab stream \n";


			//end timer to get time since stream started
			clock_gettime(CLOCK_REALTIME, &bufferEnd);
			double bufferDifference = diffClock(bufferStart, bufferEnd);

			//The stream takes a while to start up, and because of it, images from the camera
			//buffer. We don't have a way to jump to the end of the stream to get the latest image, so we
			//run this loop as fast as we can and throw away all the old images. This wait, waits some number of seconds
			//before we are at the end of the stream, and can allow processing to begin.
			if ((bufferDifference >= waitForBufferToClear) && !progRun)
			{
				cout<<"Buffer Cleared: Starting Processing Thread"<<endl;
				progRun = true;

			}
			usleep(1000); //sleep for 5ms
		}

	}

	return NULL;
}
void printCommandLineUsage()
{
	cout<<"Usage: 2168_Vision  [Input]  [Options] \n\n";

	cout<<setw(10)<<left<<"Inputs:  Choose Only 1"<<endl;

	cout<<setw(10)<<left<<"";
	cout<<setw(20)<<left<<"-f <file location>";
	cout<<"Process image at <file location>"<<endl;
	cout<<setw(30)<<""<<"ex: -f /home/image.jpg"<<endl;

	cout<<setw(10)<<left<<"";
	cout<<setw(20)<<left<<"-c <ip address>";
	cout<<"Use IP camera at <ip address>"<<endl;
	cout<<setw(30)<<""<<"ex: -c 10.21.68.2"<<endl;

	cout<<setw(10)<<left<<"";
	cout<<setw(20)<<left<<"-u";
	cout<<"Use USB cam at /dev/video0"<<endl;

	cout<<endl<<endl;
	cout<<setw(10)<<left<<"Options:  Choose Any Combination"<<endl;


	cout<<setw(10)<<left<<"";
	cout<<setw(20)<<left<<"-t";
	cout<<"Enable Timing Print Outs"<<endl;

	cout<<setw(10)<<left<<"";
	cout<<setw(20)<<left<<"-v";
	cout<<"Enable Visual Output"<<endl;
	cout<<setw(30)<<""<<"Uses X11 forwarding to show processed image"<<endl;

	cout<<setw(10)<<left<<"";
	cout<<setw(20)<<left<<"-np";
	cout<<"No Processing: Disable Processing Thread"<<endl;

	cout<<setw(10)<<left<<"";
	cout<<setw(20)<<left<<"-debug";
	cout<<"Enable Debug Print Outs"<<endl;

	cout<<setw(10)<<left<<"";
	cout<<setw(20)<<left<<"-help";
	cout<<"Prints this menu"<<endl;


}
*/
void RobotDemo::OperatorControl(void)
{
	/*
ProgParams params;

	params.ROBOT_IP = "10.9.87.2";
	params.ROBOT_PORT = "0";
	params.CAMERA_IP = "";
	params.IMAGE_FILE ="";

	params.From_Camera = true;
	params.From_File = false;
	params.Visualize = true;
	params.Timer = true;
	params.Debug = true;
	params.Process = true;
	params.USB_Cam = true;

	cv::Mat img, thresholded, output;

			//initialize variables so processing loop is false;
			targets.matchStart = false;
			targets.validFrame = false;
			targets.hotLeftOrRight = 0;
			progRun = false;

			//struct timespec start, end;

			//pthread_create(&MJPEG, NULL, VideoCap, &params);
			 * */

	while (IsOperatorControl())
	{
//	SmartDashboard::init();
		/*
	if (params.Process && progRun)
			{
				//start clock to determine our processing time;
				clock_gettime(CLOCK_REALTIME, &start);

				pthread_mutex_lock(&frameMutex);
				if (!frame.empty())
				{
					frame.copyTo(img);
					pthread_mutex_unlock(&frameMutex);

					thresholded = ThresholdImage(img);

					//Lock Targets and determine goals
					pthread_mutex_lock(&targetMutex);
					findTarget(img, thresholded, targets, params);
					CalculateDist(targets);

					if(params.Debug)
					{
						cout<<"Vert: "<<targets.VertGoal<<endl;
						cout<<"Horiz: "<<targets.HorizGoal<<endl;
						cout<<"Hot Goal: "<<targets.HotGoal<<endl;
						cout<<"Dist:" <<targets.targetDistance<<endl<<endl;
					}
					pthread_mutex_unlock(&targetMutex);

					clock_gettime(CLOCK_REALTIME, &end);

					if(params.Timer)
						cout << "It took " << diffClock(start,end) << " seconds to process frame \n";


				}

				pthread_mutex_unlock(&frameMutex);

				if(params.Visualize)
					cv::waitKey(5);

			}

			usleep(1000); //20000 sleep for 5ms); // run 40 times a second
*/
	//	auto areas = grip->GetNumberArray("targets/area", llvm::ArrayRef<double>());

	//	for (auto area : areas)
		//{
			//std::cout << "Got contour with area=" << area << std::endl;
		//}
		UpdateInputs();
		DriveTrain->StandardArcade(-commandForward, -commandTurn, -commandArmShooter, -commandLift);
		DriveTrain->Shifter_Update(rightStick->GetTrigger());
		DriveTrain->Arm_Update(turretStick->GetTrigger());
		Wait(0.002);
	}

/*
	auto areas = grip->GetNumberArray("myContoursReport/area", llvm::ArrayRef<double>()),
	         xs    = grip->GetNumberArray("myContoursReport/x",    llvm::ArrayRef<double>()),
	         ys    = grip->GetNumberArray("myContoursReport/y",    llvm::ArrayRef<double>());

	    for (int i = 0; i < areas.size(); i++) {
	        double area = areas[i], x = xs[i], y = ys[i];
	        std::cout << "Got contour: area=" << area << ", x=" << x << ", y=" << y << std::endl;
	    }
	    */
}
START_ROBOT_CLASS(RobotDemo);
