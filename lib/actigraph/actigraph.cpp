/*
Filename: actigraph.cpp
Author: Samuel Thompson
Description: actigraph.hpp implimentation. READ HEADER FILE COMMENTS.
Date: 2026/02/09
Modified:
*/

#include "actigraph.hpp"

#include "accelerometer.hpp"
#include <vector>

actigraph::actigraph(int windowLength, int windowCount, float movementThreshold,
                     int maxHeadAngle)
    : xl(), windowLength(windowLength), windowCount(windowCount),
      maxHeadAngle(maxHeadAngle), movementThreshold(movementThreshold) {}

bool actigraph::run() {
    data.clear(); // clear accelerometer data
    xl.start();   // turn on accelerometer

    // builds windows
    for (unsigned int window = 0; window < windowCount; window++) {

#ifdef DEBUG
        Serial.print("building Window #");
        Serial.println(window);
#endif

        std::vector<std::vector<float>> window_samples = {};
        int timestamp = millis();

        // builds a single window
        while ((millis() - timestamp) < (1000 * windowLength)) {
            window_samples.push_back(xl.getxyzg());
            delay(10); // delay to avoid duplicate data entries
        }
        data.push_back(window_samples);
    }
    xl.stop(); // turn off accelerometer

#ifdef DEBUG
    Serial.println("done building windows");
    Serial.println("checking angle and movement");
#endif

    bool angleOK = goodAngle();
    bool moveOK = goodMovement();

#ifdef DEBUG
    Serial.print("angle good? ");
    Serial.println(angleOK);
    Serial.print("movement good? ");
    Serial.println(moveOK);
#endif

    return (angleOK && moveOK);
}

bool actigraph::goodAngle() {
    std::vector<float> pitches; // forward / backward head tilt
    std::vector<float> yaws;    // left / right head rotation (looking)

    // traverse windows
    for (unsigned int window = 0; window < data.size(); window++) {

        // traverse measurements in window
        for (unsigned int xyz = 0; xyz < data[window].size(); xyz++) {
            float x = data[window][xyz][0];
            float y = data[window][xyz][1];
            float z = data[window][xyz][2];

            // Device is not deployed or body is unacceptably positioned
            if (z > 0) {
#ifdef DEBUG
                Serial.print("device is inverted, z= ");
                Serial.println(z);
#endif
                return false;
            }
            pitches.push_back(pitch(x, y, z));
            yaws.push_back(yaw(x, y, z));
        }
    }

    // calculates average angles
    float meanPitch = mean(pitches);
    float meanYaw = mean(yaws);

#ifdef DEBUG
    Serial.print("Mean Pitch: ");
    Serial.println(meanPitch);
    Serial.print("Mean Yaw: ");
    Serial.println(meanYaw);
#endif

    // if angles are within threshold, return true, else false
    if (abs(meanPitch) < maxHeadAngle && abs(meanYaw) < maxHeadAngle) {
        return true;
    }
    return false;
}

bool actigraph::goodMovement() {
    // perform check on each window
    for (unsigned int window = 0; window < data.size(); window++) {
        std::vector<float> windowMag = {};

        // convert from x y z vectors to magnitude
        for (unsigned int sample = 0; sample < data[window].size(); sample++) {
            float x = data[window][sample][0];
            float y = data[window][sample][1];
            float z = data[window][sample][2];
            windowMag.push_back(sqrt((x * x) + (y * y) + (z * z)));
        }

        float mSD = standardDeviation(windowMag);

#ifdef DEBUG
        Serial.print("Window #");
        Serial.print(window);
        Serial.print(" Standard Deviation: ");
        Serial.println(mSD);
#endif

        if (mSD > movementThreshold) {
#ifdef DEBUG
            Serial.print("Window #");
            Serial.print(window);
            Serial.println("'s SD IS TOO BIG");
#endif
            return false; // return false if any window fails
        }
    }
    return true; // if all are under threshold, movement is good
}

float actigraph::pitch(float x, float y, float z) {
    float pitch = 0.0;
#if defined(usb_c_to_chin)
    pitch = atan2(-x, -z);
#elif defined(usb_c_to_crown)
    pitch = atan2(x, -z);
#elif defined(usb_c_to_left_ear)
    pitch = atan2(y, -z);
#elif defined(usb_c_to_right_ear)
    pitch = atan2(-y, -z);
#endif
    pitch = pitch * (180.0 / 3.14159); // convert to degrees
    return pitch;
}

float actigraph::yaw(float x, float y, float z) {
    float yaw = 0.0;
#if defined(usb_c_to_chin)
    yaw = atan2(-y, -z);
#elif defined(usb_c_to_crown)
    yaw = atan2(y, -z);
#elif defined(usb_c_to_left_ear)
    yaw = atan2(x, -z);
#elif defined(usb_c_to_right_ear)
    yaw = atan2(-x, -z);
#endif
    yaw = yaw * (180.0 / 3.14159); // convert to degrees
    return yaw;
}

float actigraph::mean(const std::vector<float> &vals) {
    float mean = 0.0;
    for (unsigned int i = 0; i < vals.size(); i++) { // total sum of values
        mean += vals[i];
    }
    if (vals.size() > 0) { // avoid divide by zero
        mean = mean / vals.size();
    }
    return mean;
}

float actigraph::standardDeviation(const std::vector<float> &vals) {
    if (vals.size() < 2) { // avoid divide by zero
        return 0.0;
    }

    float avg = mean(vals);
    float standardDeviation = 0.0;

    for (unsigned int i = 0; i < vals.size(); i++) { // squared sum
        standardDeviation += (vals[i] - avg) * (vals[i] - avg);
    }

    standardDeviation /= (vals.size() - 1);

    standardDeviation = sqrt(standardDeviation);
    return standardDeviation;
}
