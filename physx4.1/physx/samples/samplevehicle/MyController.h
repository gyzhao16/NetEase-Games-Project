#include "PhysXSample.h"
#include "SampleVehicle_VehicleController.h"
#include "common/PxPhysXCommonConfig.h"
#include "foundation/PxVec3.h"
#include "vehicle/PxVehicleSDK.h"
#include "vehicle/PxVehicleUpdate.h"
#include "vehicle/PxVehicleUtilControl.h"

using namespace physx;

class MyController {

public:
	MyController();
	~MyController();

	void Init();

	void Update();

	// get mycontroller data
	bool getAccel() { return accel; }
	bool getBrake() { return brake; }
	bool getHandbrake() { return handbrake; }
	bool getSteerLeft() { return steerLeft; }
	bool getSteerRight() { return steerRight; }
	bool getGearUp() { return gearUp; }
	bool getGearDown() { return gearDown; }

private:
	PxVec3 p; // current position
	PxQuat q; // current direction

	// simulate keyboard inputs
	bool accel;
	bool brake;
	bool handbrake;
	bool steerLeft;
	bool steerRight;
	bool gearUp;
	bool gearDown;
};