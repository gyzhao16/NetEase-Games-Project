#include "MyController.h"

struct PID_Calibration
{
	float kP;
	float kI;
	float kD;
};

class PID_Controller
{
public:
	PID_Controller(float _kP, float _kI, float _kD)
	{
		mParam.kP = _kP;
		mParam.kI = _kI;
		mParam.kD = _kD;
		Reset();
	}

	~PID_Controller() {}

	void Reset()
	{
		mPrevError = 0.0f;
		mIntegral = 0.0f;
		mLower = -FLT_MAX;
		mUpper = FLT_MAX;
	}

	float Compute(float dt, float Current, float Target)
	{
		float e = Target - Current;
		mIntegral += e * dt;
		float output = mParam.kP * e + mParam.kI * mIntegral + mParam.kD * (e - mPrevError) / dt;
		mPrevError = e;
		return std::max(std::min(output, mUpper), mLower);
	}

	void SetLimits(float lower, float upper)
	{
		mLower = lower;
		mUpper = upper;
	}

private:
	PID_Calibration mParam;
	float			mPrevError;
	float			mIntegral;
	float			mLower, mUpper;
};

AutonomousController::AutonomousController()
{
	autonomousModeOn = false;
	m_pid_accel = new PID_Controller(0.1f, 0.1f, 0.1f);
	m_pid_steer = new PID_Controller(0.1f, 0.1f, 0.1f);
	m_vehicle = nullptr;
	m_renderer = nullptr;
}

AutonomousController::~AutonomousController()
{

}

// update vehicle controls according to current situation
void AutonomousController::Update(float dtime) {
	
	// pid controller to be implemented
	PxTransform pose = m_vehicle->getRigidDynamicActor()->getGlobalPose();
	if (m_tracks.size() > 1000)
	{
		m_tracks.erase(m_tracks.begin());
	}
	m_tracks.emplace_back(dtime, pose.p, pose.q);
	DrawTrack();

	accel = true;
	brake = false;
	handbrake = false;
	steerLeft = false;
	steerRight = false;
	gearUp = false;
	gearDown = false;
}

void AutonomousController::DrawTrack() {

}