#include "MyController.h"

struct PID_Calibration
{
	PID_Calibration()
	{
		kP = kI = kD = 1.0f;
		LowIntegral = -FLT_MAX;
		HighIntegral = FLT_MAX;
		LowValue = -FLT_MAX;
		HighValue = FLT_MAX;
	}
	float kP;
	float kI;
	float kD;
	float LowIntegral;
	float HighIntegral;
	float LowValue;
	float HighValue;
};


class PID_Controller
{
public:
	PID_Controller(const PID_Calibration& param)
	{
		SetCalibration(param);
		Reset();
	}

	PID_Controller(float _kP, float _kI, float _kD, float lowIntegral, float highIntegral, float low, float high)
	{
		PID_Calibration param;
		param.kP = _kP;
		param.kI = _kI;
		param.kD = _kD;
		param.LowIntegral = low;
		param.HighIntegral = high;
		param.LowValue = low;
		param.HighValue = high;
		SetCalibration(param);
		Reset();
	}

	~PID_Controller() {}

	void Reset()
	{
		mPrevError = 0.0f;
		mIntegral = 0.0f;
	}

	void SetCalibration(const PID_Calibration& param)
	{
		mParam = param;
	}

	float Compute(float dt, float Current, float Target)
	{
		float e = Target - Current;
		mIntegral += e * dt;
		mIntegral = std::max(std::min(mIntegral, mParam.HighIntegral), mParam.LowIntegral);
		float output = mParam.kP * e + mParam.kI * mIntegral + mParam.kD * (e - mPrevError) / dt;
		mPrevError = e;
		return std::max(std::min(output, mParam.HighValue), mParam.LowValue);
	}

	void SetLimits(float lower, float high)
	{
		mParam.LowValue = lower;
		mParam.HighValue = high;
	}

private:
	PID_Calibration mParam;
	float			mPrevError;
	float			mIntegral;
};

std::vector<PxVec3> CatmullRom_Smoothing(const std::vector<PxVec3>& points, float dlen, float alpha, float tension)
{
	std::vector<PxVec3> smoothed;
	if (points.size() <= 1)
	{
		return std::move(smoothed);
	}

	for (size_t i = 0; i < points.size() - 1; ++i)
	{
		PxVec3 p0 = i == 0 ? 2.0f * points[0] - points[1] : points[i - 1];
		PxVec3 p1 = points[i];
		PxVec3 p2 = points[i + 1];
		PxVec3 p3 = i == points.size() - 2 ? 2.0f * points[points.size() - 1] - points[points.size() - 2] : points[i + 2];

		if ((p1 - p2).magnitudeSquared() < dlen * dlen)
		{
			smoothed.push_back(p1);
			continue;
		}

		float t01 = powf((p0 - p1).magnitude(), alpha);
		float t12 = powf((p1 - p2).magnitude(), alpha);
		float t23 = powf((p2 - p3).magnitude(), alpha);

		PxVec3 m1 = (1.0f - tension) *
			(p2 - p1 + t12 * ((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)));
		PxVec3 m2 = (1.0f - tension) *
			(p2 - p1 + t12 * ((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)));

		PxVec3 a = 2.0f * (p1 - p2) + m1 + m2;
		PxVec3 b = -3.0f * (p1 - p2) - m1 - m1 - m2;
		PxVec3 c = m1;
		PxVec3 d = p1;

		int n = (int)((p1 - p2).magnitude() / dlen + 0.5f);
		for (int i = 0; i < n; ++i)
		{
			float t = i * 1.0f / n;
			PxVec3 point = a * t * t * t + b * t * t + c * t + d;
			smoothed.push_back(point);
		}
	}

	smoothed.push_back(points.back());
	return std::move(smoothed);
}

AutonomousController::AutonomousController()
{
	autonomousModeOn = true;
	m_pid_accel = new PID_Controller(0.5f, 0.08f, 0.1f, -10.0f, 10.0f, -1.0f, 1.0f);
	m_pid_steer = new PID_Controller(1.0f, 0.15f, 0.1f, -1.0f, 1.0f, -1.0f, 1.0f);
	m_vehicle = nullptr;
	m_renderer = nullptr;
	currentSpeed = 0.0f;
}

AutonomousController::~AutonomousController()
{

}

// update vehicle controls according to current situation
void AutonomousController::update(float dtime) {
	const PxRigidDynamic* actor = m_vehicle->getRigidDynamicActor();
	PxTransform pose = actor->getGlobalPose();
	if (m_tracks.size() > 1000)
	{
		m_tracks.erase(m_tracks.begin());
	}
	if (m_tracks.empty() || (m_tracks[m_tracks.size() - 1].p - pose.p).magnitudeSquared() > 0.01f)
	{
		m_tracks.emplace_back(dtime, pose.p, pose.q);
	}

	// set keyboard iputs
	accelPressed = false;
	brakePressed = false;
	handbrakePressed = false;
	steerleftPressed = false;
	steerrightPressed = false;
	gearUpPressed = false;
	gearDownPressed = false;

	// set gamepad inputs
	accel = 0.0f;
	brake = 0.0f;
	handbrake = false;
	steer = 0.0f;
	gearUp = false;
	gearDown = false;

	m_vehicleController->setForwardMode((PxVehicleDrive4W*)m_vehicle);

	if (!m_targets.empty())
	{
		float dist = (m_targets[0] - pose.p).magnitude();
		if (dist > 3.0f)
		{
			PxVec3 forward = pose.q.rotate(PxVec3(0, 0, 1));
			PxVec3 up = pose.q.rotate(PxVec3(0, 1, 0));
			PxVec3 target = (m_targets[0] - pose.p).getNormalized();

			float steer_dir = forward.cross(target).dot(up) < 0.0f ? 1.0f : -1.0f;
			float dp = forward.dot(target);

			if (numFrames > 0) {
				backup();
			}
			else {
				if (dp > 0.0f) { // target is ahead

					// set accel & brake
					float input = m_pid_accel->Compute(dtime, currentSpeed, 50.0f);
					accel = physx::PxClamp(input, 0.0f, 1.0f);
					brake = physx::PxClamp(-input, 0.0f, 1.0f);

					// set steer
					input = -PxAbs(m_pid_steer->Compute(dtime, dp, 1.0f));
					steer = steer_dir * input;
				}
				else { // target is behind, perform a backup
					numFrames = 30;

					if (steer_dir < 0.0f) {
						backupSteerleft = true;
						backupSteerright = false;
					}
					else {
						backupSteerleft = false;
						backupSteerright = true;
					}
				}
			}
		}
		else
		{
			m_targets.erase(m_targets.begin());
		}
	}
	else
	{
		brake = 1.0f;
	}

	setCurrentSpeed(3.6f * m_vehicle->computeForwardSpeed());
}

void AutonomousController::backup() {
	if (currentSpeed > 0.5f && !m_vehicleController->isReverseMode()) {
		brake = 1.0f;
	}
	else {
		numFrames--;
		m_vehicleController->setReverseMode((PxVehicleDrive4W*)m_vehicle);
		brake = 1.0f;
		steer = backupSteerleft == true ? -0.5f : 0.5f;
	}
}

void AutonomousController::drawTarget(PxScene* mScene) {
	
	PxSceneReadLock scopedLock(*mScene);
	const RendererColor colorYellow(255, 255, 0);

	if (!m_targets.empty()) {
		for (auto ele : m_targets) {
			m_renderer->addLine(ele, ele + PxVec3(0, 5, 0), colorYellow);
		}
	}

	const RendererColor colorGreen(0, 255, 0);
	if (m_paths_smoothed.size() > 1) {
		for (size_t i = 0; i < m_paths_smoothed.size() - 1; ++i) {
			m_renderer->addLine(m_paths_smoothed[i] + PxVec3(0, 0.5f, 0), m_paths_smoothed[i+1] + PxVec3(0, 0.5f, 0), colorGreen);
		}
	}
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

void AutonomousController::drawPngFile(PxScene* mScene) {

	PxSceneReadLock scopedLock(*mScene);

	std::time_t currentTime = std::time(0);
	std::string filename = std::to_string(currentTime) + ".png";
	FILE* fp = fopen(filename.data(), "wb");

	std::vector<Track> tmp_tracks = m_tracks;
	sort(tmp_tracks.begin(), tmp_tracks.end(), cmpX);
	float x_min = tmp_tracks[0].p.x, x_max = tmp_tracks[tmp_tracks.size() - 1].p.x, x_interval = x_max - x_min;
	sort(tmp_tracks.begin(), tmp_tracks.end(), cmpZ);
	float z_min = tmp_tracks[0].p.z, z_max = tmp_tracks[tmp_tracks.size() - 1].p.z, z_interval = z_max - z_min;

	unsigned char rgb[256 * 256 * 3], *p = rgb;
	
	for (int i = 0; i < 256; ++i) {
		for (int j = 0; j < 256; ++j) {
			*p++ = 0;
			*p++ = 0;
			*p++ = 0;
		}
	}
	for (auto ele : m_tracks) {
		int x = (ele.p.x - x_min) / x_interval * 255.0f + 0.5f;
		int z = (ele.p.z - z_min) / z_interval * 255.0f + 0.5f;
		rgb[(z * 256 + x) * 3] = 255;
		rgb[(z * 256 + x) * 3 + 1] = 255;
		rgb[(z * 256 + x) * 3 + 2] = 255;
	}
	
	svpng(fp, 256, 256, rgb, 0);
	fclose(fp);
}

void AutonomousController::setTarget(PxVec3 target)
{
	m_targets.clear();
	m_targets.push_back(target);
}

void AutonomousController::addTarget(PxVec3 target)
{
	m_targets.push_back(target);
	
	m_paths_smoothed.clear();
	if (m_targets.size() > 1)
	{
		m_paths_smoothed = CatmullRom_Smoothing(m_targets, 0.5f, 1.0f, 0.0f);
	}
}

bool cmpX(Track& t1, Track& t2) {
	return t1.p.x < t2.p.x;
}

bool cmpZ(Track& t1, Track& t2) {
	return t1.p.z < t2.p.z;
}

void svpng(FILE* fp, unsigned int w, unsigned int h, const unsigned char* img, int alpha) {
	static const unsigned t[] = { 0, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
		/* CRC32 Table */    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };
	unsigned int a = 1, b = 0, c, p = w * (alpha ? 4 : 3) + 1, x, y, i;   /* ADLER-a, ADLER-b, CRC, pitch */
#define SVPNG_PUT(u) fputc(u, fp)
#define SVPNG_U8A(ua, l) for (i = 0; i < l; i++) SVPNG_PUT((ua)[i]);
#define SVPNG_U32(u) do { SVPNG_PUT((u) >> 24); SVPNG_PUT(((u) >> 16) & 255); SVPNG_PUT(((u) >> 8) & 255); SVPNG_PUT((u) & 255); } while(0)
#define SVPNG_U8C(u) do { SVPNG_PUT(u); c ^= (u); c = (c >> 4) ^ t[c & 15]; c = (c >> 4) ^ t[c & 15]; } while(0)
#define SVPNG_U8AC(ua, l) for (i = 0; i < l; i++) SVPNG_U8C((ua)[i])
#define SVPNG_U16LC(u) do { SVPNG_U8C((u) & 255); SVPNG_U8C(((u) >> 8) & 255); } while(0)
#define SVPNG_U32C(u) do { SVPNG_U8C((u) >> 24); SVPNG_U8C(((u) >> 16) & 255); SVPNG_U8C(((u) >> 8) & 255); SVPNG_U8C((u) & 255); } while(0)
#define SVPNG_U8ADLER(u) do { SVPNG_U8C(u); a = (a + (u)) % 65521; b = (b + a) % 65521; } while(0)
#define SVPNG_BEGIN(s, l) do { SVPNG_U32(l); c = ~0U; SVPNG_U8AC(s, 4); } while(0)
#define SVPNG_END() SVPNG_U32(~c)
	SVPNG_U8A("\x89PNG\r\n\32\n", 8);           /* Magic */
	SVPNG_BEGIN("IHDR", 13);                    /* IHDR chunk { */
	SVPNG_U32C(w); SVPNG_U32C(h);               /*   Width & Height (8 bytes) */
	SVPNG_U8C(8); SVPNG_U8C(alpha ? 6 : 2);     /*   Depth=8, Color=True color with/without alpha (2 bytes) */
	SVPNG_U8AC("\0\0\0", 3);                    /*   Compression=Deflate, Filter=No, Interlace=No (3 bytes) */
	SVPNG_END();                                /* } */
	SVPNG_BEGIN("IDAT", 2 + h * (5 + p) + 4);   /* IDAT chunk { */
	SVPNG_U8AC("\x78\1", 2);                    /*   Deflate block begin (2 bytes) */
	for (y = 0; y < h; y++) {                   /*   Each horizontal line makes a block for simplicity */
		SVPNG_U8C(y == h - 1);                  /*   1 for the last block, 0 for others (1 byte) */
		SVPNG_U16LC(p); SVPNG_U16LC(~p);        /*   Size of block in little endian and its 1's complement (4 bytes) */
		SVPNG_U8ADLER(0);                       /*   No filter prefix (1 byte) */
		for (x = 0; x < p - 1; x++, img++)
			SVPNG_U8ADLER(*img);                /*   Image pixel data */
	}
	SVPNG_U32C((b << 16) | a);                  /*   Deflate block end with adler (4 bytes) */
	SVPNG_END();                                /* } */
	SVPNG_BEGIN("IEND", 0); SVPNG_END();        /* IEND chunk {} */
}