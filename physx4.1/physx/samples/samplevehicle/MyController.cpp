#include "MyController.h"

MyController::MyController() {

}

MyController::~MyController() {

}

// update vehicle controls according to current situation
void MyController::Update() {
	
	// pid controller to be implemented

	accel = true;
	brake = false;
	handbrake = false;
	steerLeft = false;
	steerRight = false;
	gearUp = false;
	gearDown = false;
}