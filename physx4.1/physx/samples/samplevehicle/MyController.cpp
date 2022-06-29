#include "MyController.h"

MyController::MyController() {

}

MyController::~MyController() {

}

void MyController::Update() {
	
	accel = true;
	brake = false;
	handbrake = false;
	steerLeft = false;
	steerRight = false;
	gearUp = false;
	gearDown = false;
}