/*
   filters.cpp: Filter class implementations
 */

//#include <cmath>
#include <stdlib.h> // XXX eventually use fabs() instead of abs() ?

#include "filters.h"
#include <iostream>

void KalmanFilter::getPredictionCovariance(double covariance[3][3], double previousState[3], double deltat)
{
    // required matrices for the operations
    double sigma[3][3];
    double identity[3][3];
    identityMatrix3x3(identity);
    double skewMatrix[3][3];
    skew(skewMatrix, previousState);
    double tmp[3][3];
    // Compute the prediction covariance matrix
    scaleMatrix3x3(sigma, pow(sigmaGyro, 2), identity);
    matrixProduct3x3(tmp, skewMatrix, sigma);
    matrixProduct3x3(covariance, tmp, skewMatrix);
    scaleMatrix3x3(covariance, -pow(deltat, 2), covariance);
}

void KalmanFilter::getMeasurementCovariance(double covariance[3][3])
{
    // required matrices for the operations
    double sigma[3][3];
    double identity[3][3];
    identityMatrix3x3(identity);
    double norm;
    // Compute measurement covariance
    scaleMatrix3x3(sigma, pow(sigmaAccel, 2), identity);
    vectorLength(& norm, previousAccelSensor);
    scaleAndAccumulateMatrix3x3(sigma, (1.0/3.0)*pow(ca, 2)*norm, identity);
    copyMatrix3x3(covariance, sigma);
}

void KalmanFilter::predictState(double predictedState[3], double gyro[3], double deltat)
{
    // helper matrices
    double identity[3][3];
    identityMatrix3x3(identity);
    double skewFromGyro[3][3];
    skew(skewFromGyro, gyro);
    // Predict state
    scaleAndAccumulateMatrix3x3(identity, -deltat, skewFromGyro);

    matrixDotVector3x3(predictedState, identity, currentState);
    std::cout << currentState[0] << " " << currentState[1] << " " << currentState[2]  << std::endl;
    normalizeVector(predictedState);
}

void KalmanFilter::predictErrorCovariance(double covariance[3][3], double gyro[3], double deltat)
{
    // required matrices
    double Q[3][3];
    double identity[3][3];
    identityMatrix3x3(identity);
    double skewFromGyro[3][3];
    skew(skewFromGyro, gyro);
    double tmp[3][3];
    double tmpTransposed[3][3];
    double tmp2[3][3];
    // predict error covariance
    getPredictionCovariance(Q, currentState, deltat);
    scaleAndAccumulateMatrix3x3(identity, -deltat, skewFromGyro);
    copyMatrix3x3(tmp, identity);
    transposeMatrix3x3(tmpTransposed, tmp);
    matrixProduct3x3(tmp2, tmp, currErrorCovariance);
    matrixProduct3x3(covariance, tmp2, tmpTransposed);
    scaleAndAccumulateMatrix3x3(covariance, 1.0, Q);
}

void KalmanFilter::updateGain(double gain[3][3], double errorCovariance[3][3])
{
    // required matrices
    double R[3][3];
    double HTransposed[3][3];
    transposeMatrix3x3(HTransposed, H);
    double tmp[3][3];
    double tmp2[3][3];
    double tmp2Inverse[3][3];
    // update kalman gain
    // P.dot(H.T).dot(inv(H.dot(P).dot(H.T) + R))
    getMeasurementCovariance(R);
    matrixProduct3x3(tmp, errorCovariance, HTransposed);
    matrixProduct3x3(tmp2, H, tmp);
    scaleAndAccumulateMatrix3x3(tmp2, 1.0, R);
    invert3x3(tmp2Inverse, tmp2);
    matrixProduct3x3(gain, tmp, tmp2Inverse);
}

void KalmanFilter::updateState(double updatedState[3], double predictedState[3], double gain[3][3], double accel[3])
{
    // required matrices
    double tmp[3];
    double tmp2[3];
    double measurement[3];
    scaleVector(tmp, ca, previousAccelSensor);
    subtractVectors(measurement, accel, tmp);
    // update state with measurement
    // predicted_state + K.dot(measurement - H.dot(predicted_state))
    matrixDotVector3x3(tmp, H, predictedState);
    subtractVectors(tmp, measurement, tmp);
    matrixDotVector3x3(tmp2, gain, tmp);
    sumVectors(updatedState, predictedState, tmp2);
    normalizeVector(updatedState);
}

void KalmanFilter::updateErrorCovariance(double covariance[3][3], double errorCovariance[3][3], double gain[3][3])
{
    // required matrices
    double identity[3][3];
    identityMatrix3x3(identity);
    double tmp[3][3];
    double tmp2[3][3];
    // update error covariance with measurement
    matrixProduct3x3(tmp, gain, H);
    matrixProduct3x3(tmp2, tmp, errorCovariance);
    scaleAndAccumulateMatrix3x3(identity, -1.0, tmp2);
    copyMatrix3x3(covariance, tmp2);
}


KalmanFilter::KalmanFilter(double ca, double sigmaGyro, double sigmaAccel)
{
    this->ca = ca;
    this->sigmaGyro = sigmaGyro;
    this->sigmaAccel = sigmaAccel;
}

double KalmanFilter::estimate(double gyro[3], double accel[3], double deltat)
{
    double predictedState[3];
    double updatedState[3];
    double errorCovariance[3][3];
    double updatedErrorCovariance[3][3];
    double gain[3][3];
    double accelSensor[3];
    double tmp[3];
    double accelEarth;
    //scaleVector(accel, 9.81, accel); // Scale accel readings since they are measured in gs
    // perform estimation
    // predictions
    predictState(predictedState, gyro, deltat);
    predictErrorCovariance(errorCovariance, gyro, deltat);
    // updates
    updateGain(gain, errorCovariance);
    updateState(updatedState, predictedState, gain, accel);
    updateErrorCovariance(updatedErrorCovariance, errorCovariance, gain);
    // Store required values for next iteration
    copyVector(currentState, updatedState);
    copyMatrix3x3(currErrorCovariance, updatedErrorCovariance);
    // return vertical acceleration estimate
    scaleVector(tmp, 9.81, updatedState);
    subtractVectors(accelSensor, accel, tmp);
    copyVector(previousAccelSensor, accelSensor);
    dotProductVectors(& accelEarth, accelSensor, updatedState);
    return accelEarth;
}


double ComplementaryFilter::ApplyZUPT(double accel, double vel)
{
    // first update ZUPT array with latest estimation
    ZUPT[ZUPTIdx] = accel;
    // and move index to next slot
    uint8_t nextIndex = (ZUPTIdx + 1) % ZUPT_SIZE;
    ZUPTIdx = nextIndex;
    // Apply Zero-velocity update
    for (uint8_t k = 0; k < ZUPT_SIZE; ++k) {
        if (abs(ZUPT[k]) > accelThreshold) return vel;
    }
    return 0.0;
}


ComplementaryFilter::ComplementaryFilter(double sigmaAccel, double sigmaBaro, double accelThreshold)
{
    // Compute the filter gain
    gain[0] = sqrt(2 * sigmaAccel / sigmaBaro);
    gain[1] = sigmaAccel / sigmaBaro;
    // If acceleration is below the threshold the ZUPT counter
    // will be increased
    this->accelThreshold = accelThreshold;
    // initialize zero-velocity update
    ZUPTIdx = 0;
    for (uint8_t k = 0; k < ZUPT_SIZE; ++k) {
        ZUPT[k] = 0;
    }
}

void ComplementaryFilter::estimate(double * velocity, double * altitude, double baroAltitude,
        double pastAltitude, double pastVelocity, double accel, double deltat)
{
    // Apply complementary filter
    *altitude = pastAltitude + deltat*(pastVelocity + (gain[0] + gain[1]*deltat/2)*(baroAltitude-pastAltitude))+
        accel*pow(deltat, 2)/2;
    *velocity = pastVelocity + deltat*(gain[1]*(baroAltitude-pastAltitude) + accel);
    // Compute zero-velocity update
    *velocity = ApplyZUPT(accel, *velocity);
}
