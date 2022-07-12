#include "PhysXSample.h"
#include "SampleVehicle_VehicleController.h"
#include "common/PxPhysXCommonConfig.h"
#include "foundation/PxVec3.h"
#include "vehicle/PxVehicleSDK.h"
#include "vehicle/PxVehicleUpdate.h"
#include "vehicle/PxVehicleUtilControl.h"
#include "RenderPhysX3Debug.h"
#include "RendererColor.h"
#include <ctime>
#include <stdio.h>
#include <float.h>
#include <vector>

using namespace physx;
using namespace SampleRenderer;

class PID_Controller;

namespace physx
{
class PxVehicleWheels;
}

struct Track
{
	Track(float _t, PxVec3 _p, PxQuat _q)
	{
		time = _t;
		p = _p;
		q = _q;
	}

	float time;
	PxVec3 p;
	PxQuat q;
};

enum ControllerMode
{
	MANUAL_MODE = 0,
	TARGET_MODE,
	BACKUP_MODE,
	TRAJECTORY_MODE,
};

class AutonomousController {

public:

	AutonomousController();
	~AutonomousController();

	void update(PxF32 dtime);

	void drawTarget(PxScene* mScene);

	void drawTrack(PxScene* mScene);

	void drawPngFile(PxScene* mScene);
	
	ControllerMode		getControllerMode() const { return controllerMode; }
	void				setControllerMode(ControllerMode mode) { controllerMode = mode; }

	std::vector<Track> getVehicleTrack() { return m_tracks; }

	// get gamepad input data
	PxF32 getAccel()     { return accel; }
	PxF32 getBrake()     { return brake; }
	bool  getHandbrake() { return handbrake; }
	PxF32 getSteer()     { return steer; }
	bool  getGearUp()    { return gearUp; }
	bool  getGearDown()  { return gearDown; }

	// get keyboard input data
	bool getAccelPressed()      { return accelPressed; }
	bool getBrakePressed()      { return brakePressed; }
	bool getHandbrakePressed()  { return handbrakePressed; }
	bool getSteerleftPressed()  { return steerleftPressed; }
	bool getSteerrightPressed() { return steerrightPressed; }
	bool getGearUpPressed()     { return gearUpPressed; }
	bool getGearDownPressed()   { return gearDownPressed; }

	void setCurrentSpeed(PxF32 speed) { currentSpeed = speed * 3.6f; }

	void setVehicle(PxVehicleWheels* vehicle) { m_vehicle = vehicle; }
	void setRenderer(RenderPhysX3Debug* renderer) { m_renderer = renderer; }
	void setVehicleController(SampleVehicle_VehicleController* vehicleController) { m_vehicleController = vehicleController; }

	void setPath(const std::vector<PxVec3> _paths) { m_targets = _paths; }
	void setTarget(PxVec3 target);
	void addTarget(PxVec3 target);

	void backup();

private:

	void updateManualMode(PxF32 dtime);
	void updateTargetMode(PxF32 dtime);
	void updateBackupMode(PxF32 dtime);
	void updateTrajectoryMode(PxF32 dtime);

	// drive mode
	ControllerMode controllerMode;

	// simulate gamepad inputs
	PxF32 accel;
	PxF32 brake;
	bool  handbrake;
	PxF32 steer;
	bool  gearUp;
	bool  gearDown;

	// simulate keyboard inputs
	bool accelPressed;
	bool brakePressed;
	bool handbrakePressed;
	bool steerleftPressed;
	bool steerrightPressed;
	bool gearUpPressed;
	bool gearDownPressed;

	// number of frames performing a backup
	int numFrames;

	// backup steer
	bool backupSteerleft;
	bool backupSteerright;

	// current speed in kmh
	PxF32 currentSpeed;

	std::vector<Track> m_tracks;
	std::vector<PxVec3> m_targets;
	std::vector<PxVec3> m_paths_smoothed;

	PID_Controller* m_pid_accel;
	PID_Controller* m_pid_steer;

	PxVehicleWheels* m_vehicle;
	RenderPhysX3Debug* m_renderer;

	SampleVehicle_VehicleController* m_vehicleController;
};

bool cmpX(Track& t1, Track& t2);
bool cmpZ(Track& t1, Track& t2);
void svpng(FILE* fp, unsigned int w, unsigned int h, const unsigned char* img, int alpha);
