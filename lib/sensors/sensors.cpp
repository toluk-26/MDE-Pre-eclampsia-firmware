/**
 * @file sensors.hpp
 * @authors Lateisha Shahril, Samuel Thompson
 * @description: Sensor class implementation
 * @date YYYY/MM/DD
 * @modified:
 */

#include "sensors.hpp"

bool Sensors::runActigraph() { return ak.run(); }

bool Sensors::rtcTriggered() { return rtc.tick(); }

bloodPressure Sensors::measureBP() { return ppg.run(); }
