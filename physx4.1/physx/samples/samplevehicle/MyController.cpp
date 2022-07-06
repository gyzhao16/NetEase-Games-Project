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
void AutonomousController::update(float dtime) {
	
	// pid controller to be implemented
	PxTransform pose = m_vehicle->getRigidDynamicActor()->getGlobalPose();
	if (m_tracks.size() > 1000)
	{
		m_tracks.erase(m_tracks.begin());
	}
	m_tracks.emplace_back(dtime, pose.p, pose.q);
	//DrawTrack(pose.p, pose.q);

	accel = 1.0f;
	brake = 0.0f;
	handbrake = false;
	steer = 0.0f;
	gearUp = false;
	gearDown = false;
}

void AutonomousController::drawTrack(PxScene* mScene) {
	PxSceneReadLock scopedLock(*mScene);
	const RendererColor colorPurple(255, 0, 255);

	if (m_tracks.size() > 1) {
		for (int i = 1; i < m_tracks.size(); ++i) {
			m_renderer->addLine(m_tracks[i - 1].p, m_tracks[i].p, colorPurple);
		}
	}
	/*for (auto ele: m_tracks) {
		m_renderer->addLine(ele.p, ele.p + PxVec3(0, 1, 0), colorPurple);
	}*/
}