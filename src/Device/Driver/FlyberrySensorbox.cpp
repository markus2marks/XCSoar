/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2021 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Device/Driver/FlyberrySensorbox.hpp"
#include "Device/Driver.hpp"
#include "Device/Util/NMEAWriter.hpp"
#include "NMEA/Checksum.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/InputLine.hpp"
#include "Math/KalmanFilter1d.hpp"
#include "Math/Variometer/altitude.h"
#include "Math/Variometer/algebra.h"
#include "Math/Variometer/filters.h"
#include <iostream>

struct sensor_data
{
  uint32_t counter;
  uint8_t id;
  uint32_t timestamp;
  int32_t sensor1;
  int32_t sensor2;
  int32_t sensor3;
  float gx;
  float gy;
  float gz;
  float ax;
  float ay;
  float az;
  float quat_w;
  float quat_x;
  float quat_y;
  float quat_z;
};

class FlyberrySensorboxDevice : public AbstractDevice {
  Port &port;

  public:
    FlyberrySensorboxDevice(Port &_port):port(_port)
    {
      kalman_filter.SetAccelerationVariance(0.3);
      setQNH = false;
    }

    /* virtual methods from class Device */
    bool DataReceived(std::span<const std::byte> line, struct NMEAInfo &info) noexcept override;

  private:
    struct sensor_data* data;
    bool setQNH;
    KalmanFilter1d kalman_filter;

    double acceleration[AXIS] = {0.0, 0.0, 0.0};
    double gyrometer[AXIS] = {0.0, 0.0, 0.0};
    AltitudeEstimator altitude = AltitudeEstimator(ACCEL_SIGMA, GYRO_SIGMA, BARO_SIGMA, CA, ACCEL_SIGMA);

    void computeVario(struct NMEAInfo &info);
    void getImuVals();
    float verticalVelocity = 0;
};

static inline double
ComputeNoncompVario(const double pressure, const double d_pressure)
{
  static constexpr double FACTOR(-2260.389548275485);
  static constexpr double EXPONENT(-0.8097374740609689);
  return FACTOR * pow(pressure, EXPONENT) * d_pressure;
}



void FlyberrySensorboxDevice::getImuVals()
{
    // Copy gyro values back out in rad/sec
    gyrometer[0] = data->gx;
    gyrometer[1] = data->gy;
    gyrometer[2] = data->gz;

    acceleration[0] = data->ax;
    acceleration[1] = data->ay;
    acceleration[2] = data->az;
}


void FlyberrySensorboxDevice::computeVario(struct NMEAInfo &info)
{
  // e.g. PRS 17CBA
  static uint32_t timestamp_temp = 0;

  //std::cout <<  std::to_string(data->sensor1) << " " <<  std::to_string(data->sensor2) << " " <<  std::to_string(data->sensor3) << std::endl;
//  std::cout <<  std::to_string(data->ax) << " " <<  std::to_string(data->ay) << " " <<  std::to_string(data->az) << std::endl;


  AtmosphericPressure te_pressure = AtmosphericPressure::Pascal((double)data->sensor2 * 1.0);

  kalman_filter.Update(te_pressure.GetHectoPascal(), 0.2, ((double)data->timestamp - timestamp_temp) / 1000.0);

  info.ProvideTotalEnergyVario(ComputeNoncompVario(kalman_filter.GetXAbs(), kalman_filter.GetXVel()));

  timestamp_temp = data->timestamp;

  info.ProvideStaticPressure(AtmosphericPressure::HectoPascal((double)data->sensor3 / 100.0));

  info.ProvidePressureAltitude((double)data->sensor3 / 100.0);



  double baroHeight = altitude.getHeight((float)data->sensor2/ 100.0 , 1013.25);
  getImuVals();
  altitude.estimate(acceleration, gyrometer, baroHeight, data->timestamp);
 // velocity = moving_average(altitude.getVerticalVelocity());
  verticalVelocity = altitude.getVerticalVelocity();
//  std::cout << data->sensor2 << " " << baroHeight << " " << verticalVelocity << std::endl;

}


bool FlyberrySensorboxDevice::DataReceived(std::span<const std::byte> line, struct NMEAInfo &info) noexcept
{
  static uint8_t array[100];

  for(unsigned i = 0; i < line.size_bytes(); i++)
  {
    array[i] = (uint8_t)line.data()[i];
  }
  data = (struct sensor_data*)array;
  this->computeVario(info);


  double airspeed_pressure = (double)data->sensor1;
  if(airspeed_pressure < 0)
  {
    airspeed_pressure = 0;
  }
  info.ProvideDynamicPressure(AtmosphericPressure::Pascal(airspeed_pressure));


  info.alive.Update(info.clock);
  info.static_pressure_available.Update(info.clock);

  if(setQNH == false)
  {
    AtmosphericPressure pres = info.static_pressure.HectoPascal(data->sensor3 / 100.0);
    setQNH = info.settings.ProvideQNH(pres, info.clock);
  }
  return true;
}

static Device * FlyberrySensorboxDeviceCreateOnPort([[maybe_unused]] const DeviceConfig &config, Port &port)
{
  return new FlyberrySensorboxDevice(port);
}

const struct DeviceRegister flyberrysensorbox_driver = {
  _T("Flyberry_Sensorbox"),
  _T("Flyberry Sensorbox"),
  DeviceRegister::RAW_GPS_DATA,
  FlyberrySensorboxDeviceCreateOnPort,
};
