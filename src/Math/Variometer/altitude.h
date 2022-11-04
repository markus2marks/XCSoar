/*
    altitude.h: Altitude estimation via barometer/accelerometer fusion
*/

# pragma once

#include "filters.h"
#include "algebra.h"
#include <chrono>


 using namespace std::chrono;

class AltitudeEstimator {

  private:
    // required parameters for the filters used for the estimations
    // sensor's standard deviations
    double sigmaAccel;
    double sigmaGyro;
    double sigmaBaro;
    // Acceleration markov chain model state transition constant
    double ca;
    // Zero-velocity update acceleration threshold
    double accelThreshold;
    // gravity
    double g = 9.81;
    // For computing the sampling period
    uint32_t previousTime =  duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    // required filters for altitude and vertical velocity estimation
    KalmanFilter kalman;
    ComplementaryFilter complementary;
    // Estimated past vertical acceleration
    double pastVerticalAccel = 0;
    double pastVerticalVelocity = 0;
    double pastAltitude = 0;
    double pastGyro[3] = {0, 0, 0};
    double pastAccel[3] = {0, 0, 0};
    // estimated altitude and vertical velocity
    double estimatedAltitude = 0;
    double estimatedVelocity = 0;

  public:

    AltitudeEstimator(double sigmaAccel, double sigmaGyro, double sigmaBaro,
                      double ca, double accelThreshold);

    void estimate(double accel[3], double gyro[3], double baroHeight, uint32_t timestamp);

    double getAltitude();

    double getVerticalVelocity();

    double getVerticalAcceleration();

    double getHeight(double atmospheric, double seaLevel);

}; // class AltitudeEstimator
