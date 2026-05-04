/**
 * @file sensors.hpp
 * @authors Lateisha Shahril, Samuel Thompson
 * @description: Sensor class implementation
 * @date YYYY/MM/DD
 * @modified:
 */
#include "sensors.hpp"

bool Sensors::motionOK() { return acti.run(); }
bool Sensors::positionOK() { return acti.run(); }
bool Sensors::rtcTriggered() { return ::rtc.tick(); }