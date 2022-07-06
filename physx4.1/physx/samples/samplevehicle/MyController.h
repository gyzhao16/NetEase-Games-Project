#include "PhysXSample.h"
#include "SampleVehicle_VehicleController.h"
#include "common/PxPhysXCommonConfig.h"
#include "foundation/PxVec3.h"
#include "vehicle/PxVehicleSDK.h"
#include "vehicle/PxVehicleUpdate.h"
#include "vehicle/PxVehicleUtilControl.h"
#include "RenderPhysX3Debug.h"
#include "RendererColor.h"

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
		_q = q;
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

	void drawTrack(PxScene* mScene);
	
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

	void setVehicle(PxVehicleWheels* vehicle) { m_vehicle = vehicle; }
	void setRenderer(RenderPhysX3Debug* renderer) { m_renderer = renderer; }
	void setPath(const std::vector<PxVec3> _paths) { m_paths = _paths; }

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

	std::vector<Track> m_tracks;
	std::vector<PxVec3> m_paths;

	PID_Controller* m_pid_accel;
	PID_Controller* m_pid_steer;

	PxVehicleWheels* m_vehicle;
	RenderPhysX3Debug* m_renderer;
};