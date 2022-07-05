#include "PhysXSample.h"
#include "SampleVehicle_VehicleController.h"
#include "common/PxPhysXCommonConfig.h"
#include "foundation/PxVec3.h"
#include "vehicle/PxVehicleSDK.h"
#include "vehicle/PxVehicleUpdate.h"
#include "vehicle/PxVehicleUtilControl.h"

using namespace physx;

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

	void Init();

	void Update(float dtime);

	void reverseMode() { autonomousModeOn = !autonomousModeOn; }
	bool isAutonomousModeOn() { return autonomousModeOn; }

	// get mycontroller data
	bool getAccel() { return accel; }
	bool getBrake() { return brake; }
	bool getHandbrake() { return handbrake; }
	bool getSteerLeft() { return steerLeft; }
	bool getSteerRight() { return steerRight; }
	bool getGearUp() { return gearUp; }
	bool getGearDown() { return gearDown; }

	void setVehicle(PxVehicleWheels* vehicle) { m_vehicle = vehicle; }
	void setPath(const std::vector<PxVec3> _paths) { m_paths = _paths; }

private:
	// Autonomous Mode
	bool autonomousModeOn;

	// simulate keyboard inputs
	bool accel;
	bool brake;
	bool handbrake;
	bool steerLeft;
	bool steerRight;
	bool gearUp;
	bool gearDown;

	std::vector<Track> m_tracks;
	std::vector<PxVec3> m_paths;

	PID_Controller* m_pid_accel;
	PID_Controller* m_pid_steer;

	PxVehicleWheels* m_vehicle;
};