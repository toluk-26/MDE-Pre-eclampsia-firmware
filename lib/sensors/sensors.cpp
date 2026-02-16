#include "sensors.h"
#include <MAX30105.h>

MAX30105 max30101;

void initSensors() {
	max30101.begin();
	max30101.setup();
}

bool isUserStill() {
	// Use IMU thresholds
	return true; // placeholder
}

bool isPositionValid() {
	// Forehead orientation logic
	return true;
}

BPStatus measureBP() {
	// PPG acquisition + algorithm
	return BP_OK;
}
