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

class AutonomousController {

public:
	AutonomousController();
	~AutonomousController();

	void update(float dtime);

	void drawTarget(PxScene* mScene);

	void drawTrack(PxScene* mScene);

	void drawPngFile(PxScene* mScene);
	
	std::vector<Track> getVehicleTrack() { return m_tracks; }

	void reverseMode() { autonomousModeOn = !autonomousModeOn; }
	bool isAutonomousModeOn() { return autonomousModeOn; }

	// get mycontroller data
	PxF32 getAccel() { return accel; }
	PxF32 getBrake() { return brake; }
	bool getHandbrake() { return handbrake; }
	PxF32 getSteer() { return steer; }
	bool getGearUp() { return gearUp; }
	bool getGearDown() { return gearDown; }

	void setCurrentSpeed(PxF32 speed) { currentSpeed = speed * 3.6f; }

	void setVehicle(PxVehicleWheels* vehicle) { m_vehicle = vehicle; }
	void setRenderer(RenderPhysX3Debug* renderer) { m_renderer = renderer; }
	void setPath(const std::vector<PxVec3> _paths) { m_routes = _paths; }  
	void setTarget(PxVec3 target);

private:
	// Autonomous Mode
	bool autonomousModeOn;

	// simulate gamepad inputs
	PxF32 accel;
	PxF32 brake;
	bool handbrake;
	PxF32 steer;
	bool gearUp;
	bool gearDown;

	// current speed in kmh
	PxF32 currentSpeed;

	std::vector<Track> m_tracks;
	std::vector<PxVec3> m_routes;

	PID_Controller* m_pid_accel;
	PID_Controller* m_pid_steer;

	PxVehicleWheels* m_vehicle;
	RenderPhysX3Debug* m_renderer;
};

bool cmpX(Track& t1, Track& t2);
bool cmpZ(Track& t1, Track& t2);
void svpng(FILE* fp, unsigned int w, unsigned int h, const unsigned char* img, int alpha);