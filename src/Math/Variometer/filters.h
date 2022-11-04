/*
   filters.h: Filter class declarations
 */

#pragma once

//#include <cmath>
#include <math.h>
#include <stdint.h>

#include "algebra.h"

class KalmanFilter {
  private:
    double currentState[3] = {0, 0, 1};
    double currErrorCovariance[3][3] = {{100, 0, 0},{0, 100, 0},{0, 0, 100}};
    double H[3][3] = {{9.81, 0, 0}, {0, 9.81, 0}, {0, 0, 9.81}};
    double previousAccelSensor[3] = {0, 0, 0};
    double ca;
    double sigmaGyro;
    double sigmaAccel;

    void getPredictionCovariance(double covariance[3][3], double previousState[3], double deltat);

    void getMeasurementCovariance(double covariance[3][3]);

    void predictState(double predictedState[3], double gyro[3], double deltat);

    void predictErrorCovariance(double covariance[3][3], double gyro[3], double deltat);

    void updateGain(double gain[3][3], double errorCovariance[3][3]);

    void updateState(double updatedState[3], double predictedState[3], double gain[3][3], double accel[3]);

    void updateErrorCovariance(double covariance[3][3], double errorCovariance[3][3], double gain[3][3]);

  public:

    KalmanFilter(double ca, double sigmaGyro, double sigmaAccel);

    double estimate(double gyro[3], double accel[3], double deltat);

}; // Class KalmanFilter

class ComplementaryFilter {

  private:

    // filter gain
    double gain[2];
    // Zero-velocity update
    double accelThreshold;
    static const uint8_t ZUPT_SIZE = 12;
    uint8_t ZUPTIdx;
    double   ZUPT[ZUPT_SIZE];

    double ApplyZUPT(double accel, double vel);

  public:

    ComplementaryFilter(double sigmaAccel, double sigmaBaro, double accelThreshold);

    void estimate(double * velocity, double * altitude, double baroAltitude,
                  double pastAltitude, double pastVelocity, double accel, double deltat);
}; // Class ComplementaryFilter
