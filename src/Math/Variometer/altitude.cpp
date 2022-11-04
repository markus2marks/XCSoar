/*
    altitude.cpp: Altitude estimation via barometer/accelerometer fusion
*/

#include "filters.h"
#include "algebra.h"
#include "altitude.h"
#include <iostream>


AltitudeEstimator::AltitudeEstimator(double sigmaAccel, double sigmaGyro, double sigmaBaro,
                                     double ca, double accelThreshold)
:kalman(ca, sigmaGyro, sigmaAccel), complementary(sigmaAccel, sigmaBaro, accelThreshold)
{
      this->sigmaAccel = sigmaAccel;
      this->sigmaGyro = sigmaGyro;
      this->sigmaBaro = sigmaBaro;
      this->ca = ca;
      this->accelThreshold = accelThreshold;
}

void AltitudeEstimator::estimate(double accel[3], double gyro[3], double baroHeight, uint32_t timestamp)
{
        double deltat = (double)(timestamp-previousTime) / 1000.0f;

        double verticalAccel = kalman.estimate(pastGyro,
                                              pastAccel,
                                              deltat);

        complementary.estimate(& estimatedVelocity,
                               & estimatedAltitude,
                               baroHeight,
                               pastAltitude,
                               pastVerticalVelocity,
                               pastVerticalAccel,
                               deltat);

        // update values for next iteration
        copyVector(pastGyro, gyro);
        copyVector(pastAccel, accel);
        pastAltitude = estimatedAltitude;
        pastVerticalVelocity = estimatedVelocity;
        pastVerticalAccel = verticalAccel;
        previousTime = timestamp;
}

double AltitudeEstimator::getAltitude()
{
        // return the last estimated altitude
        return estimatedAltitude;
}

double AltitudeEstimator::getVerticalVelocity()
{
        // return the last estimated vertical velocity
        return estimatedVelocity;
}

double AltitudeEstimator::getVerticalAcceleration()
{
        // return the last estimated vertical acceleration
        return pastVerticalAccel;
}

/*!
 * \brief Read approximate altitude
 * \param seaLevel
 *      Sea level in hPa
 * \return
 *      Altitude (double)
 */
double AltitudeEstimator::getHeight(double atmospheric, double seaLevel)
{
    // In Si units for Pascal
    double ret = 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
    return ret;
}
