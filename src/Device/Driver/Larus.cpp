/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2022 The XCSoar Project
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

/**
* see Documentation https://github.com/larus-breeze/documentation_and_utilities,
* for the driver you need https://github.com/larus-breeze/standards-larus-NMEA-protocol
* and an emulator is here https://github.com/larus-breeze/SIL_flight_sensor_emulator
*/


#include "Device/Driver/Larus.hpp"
#include "Device/Driver.hpp"
#include "Device/Util/NMEAWriter.hpp"
#include "NMEA/Checksum.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/Derived.hpp"
#include "NMEA/InputLine.hpp"
#include "Units/System.hpp"
#include "Operation/Operation.hpp"
#include "LogFile.hpp"

using std::string_view_literals::operator""sv;

class LarusDevice : public AbstractDevice {
  Port &port;

public:
  LarusDevice(Port &_port):port(_port) {}

  /* virtual methods from class Device */
  bool ParseNMEA(const char *line, NMEAInfo &info) override;
//  void OnCalculatedUpdate(const MoreData &basic,
//                  const DerivedInfo &calculated) override;
private:
  bool PLARA(NMEAInputLine &line, NMEAInfo &info);
  bool PLARB(NMEAInputLine &line, NMEAInfo &info);
  bool PLARD(NMEAInputLine &line, NMEAInfo &info);
  bool PLARV(NMEAInputLine &line, NMEAInfo &info);
  bool PLARW(NMEAInputLine &line, NMEAInfo &info);
  bool HCHDT(NMEAInputLine &line, NMEAInfo &info);
 
 };

/**
 * Parses non-negative floating-point angle value in degrees.
 */
static bool
ReadBearing(NMEAInputLine &line, Angle &value_r)
{
    double value;
    if (!line.ReadChecked(value))
        return false;

    if (value < 0 || value > 360)
        return false;

    value_r = Angle::Degrees(value).AsBearing();
    return true;
}

bool LarusDevice::ParseNMEA(const char *_line, NMEAInfo &info) {
  if (!VerifyNMEAChecksum(_line))
    return false;

  NMEAInputLine line(_line);
  const auto type = line.ReadView();
  if (type.starts_with("$PLAR"sv)) {
    switch (type[5]) {
    case 'A':
      return PLARA(line, info);
    case 'B':
      return PLARB(line, info);
    case 'D':
      return PLARD(line, info);
    case 'V':
      return PLARV(line, info);
    case 'W':
      return PLARW(line, info);
    default:
      break;
    }
  }
  else if (type == "$HCHDT"sv)
    return HCHDT(line, info);

  return false;
}

bool
LarusDevice::HCHDT(NMEAInputLine &line, NMEAInfo &info)
{
    /*
   * Heading sentence
     *
     *        1   2 3
     *        |   | |
     * $HCHDT,x.x,a*hh<CR><LF>
     * 
     * State of Heading
     *
     * Field Number:
     * 1)  Heading 
     * 2)  Type: (T)rue or (M)agnetic
     * 3)  Checksum
    */
  double value;
  if (line.ReadChecked(value)) {
    if (value >= 0 && value <= 360) {
      switch (line.ReadOneChar()) {
      case 'T':
          info.attitude.heading = Angle::Degrees(value);
          info.attitude.heading_available.Update(info.clock);
        return true;
      case 'M':
      default:
        return false; // false means: an other (general) parser should look
      }
    }
  }
  return false;
}

bool
LarusDevice::PLARA(NMEAInputLine &line, NMEAInfo &info)
{
    /*
     * Attitude-Sentence
     *
     *        1   2   3   4
     *        |   |   |   |
     * $PLARA,x.x,x.x,x.x*hh<CR><LF>
     * 
     * This sentence gives information about the current attitude. The different fields
     * have the following meaning:
     * 
     * Field Number:
     * 1)  Roll angle (positive while turning right)
     * 2)  Pitch angle (positive when nose up)
     * 3)  Yaw angle (true heading)
     * 4)  Checksum
    */
  double value;
  if (line.ReadChecked(value)) {
    if (value >= -180 && value <= 180) {
      info.attitude.bank_angle = Angle::Degrees(value);
      info.attitude.bank_angle_available.Update(info.clock);
    }
  }
  if (line.ReadChecked(value)) {
    if (value >= -90 && value <= 90) {
      info.attitude.pitch_angle = Angle::Degrees(value);
      info.attitude.pitch_angle_available.Update(info.clock);
    }
  }
  if (line.ReadChecked(value)) {
    if (value >= 0 && value <= 360) {
      info.attitude.heading = Angle::Degrees(value);
      info.attitude.heading_available.Update(info.clock);
    }
  }
    return true;
}

bool
LarusDevice::PLARB(NMEAInputLine &line, NMEAInfo &info)
{
    /*
   * Battery voltage sentence
     *
     *        1     2
     *        |     |
     * $PLARB,xx.xx*hh<CR><LF>
     * 
     * State of Battery
     *
     * Field Number:
     * 1)  battery voltage in Volt
     * 2)  Checksum
    */
  double value;
  if (line.ReadChecked(value)) {
    if (value >= 0 && value <= 25) {
//      info.battery_level = value;
//      info.battery_level_available.Update(info.clock);
      info.voltage = value;
      info.voltage_available.Update(info.clock);
    }
  }
  return true;
}

bool
LarusDevice::PLARD(NMEAInputLine &line, [[maybe_unused]] NMEAInfo &info)
{
   /*
   * Instant air density sentence
     *
     *        1      2 3
     *        |      | |
     * $PLARD,xxxx.x,a*hh<CR><LF>
     * 
     * This sentence gives information about the instant air density at the
     * current altitude. The different fields have the following meaning:
     *
     * Field Number:
     * 1)  Instant air density in g/m^3.
     * 2)  a = (M)easured or (E)stimated
     * 3)  Checksum
    */
  double value;
  if (line.ReadChecked(value)) {
    switch (line.ReadOneChar()) {
    case 'M':
      break;
    case 'E':
      // TODO(Augut2111): is this correct???
      // the density behaves similar to static pressure, but isn't equal

      // info.static_pressure.HectoPascal(value);
      // info.static_pressure_available.Update(info.clock);
      break;
    default:
      break;
    }
  }
  return true;
}

bool
LarusDevice::PLARV(NMEAInputLine &line, NMEAInfo &info)
{
  /*
    * $PLARV,x.x,M,x.x,M,x,M,x.x,K*hh
    *
    * Vario Data: TEK vario, average vario, height (std pressure)
    *             and speed (tas)
    * 
    * Field Number:
    *  1) Total Energy Variometer (TEK vario)
    *  2) Average Climb Rate over one circle
    *  3) Pressure Height
    *  4) True Air Speed (TAS)
   *  5) Checksum
    */

  double value;
  // Parse total energy variometer
  if (line.ReadChecked(value)) {
    info.ProvideTotalEnergyVario(
        Units::ToSysUnit(value, Unit::METER_PER_SECOND));
  }

  // Parse average climb rate, Larus is doing this over one circle!
  if (line.ReadChecked(value)) {
  } // Skip average vario data, TODO(August2111): create a new field for a
      // - Full Circle Average Climbrate(!), make it visible and set it here

  // Parse barometric altitude
  double altitude;
  if (line.ReadChecked(altitude)) {
    altitude = Units::ToSysUnit(value, Unit::METER);
    info.ProvidePressureAltitude(altitude);
  }

  // Parse true airspeed
  if (line.ReadChecked(value))
    info.ProvideTrueAirspeedWithAltitude(
        Units::ToSysUnit(value, Unit::KILOMETER_PER_HOUR), altitude);

  return true;
}

bool
LarusDevice::PLARW(NMEAInputLine &line, NMEAInfo &info)
{
    /*
      * $PLARW,x.x,a,x.x,a,a,a*hh
      *
      * Field Number:
      *  1) wind angle
      *  2) wind speed
      *  3) (A)verage or (I)nstantaneous
      *  4) Status A=valid
      *  5) Checksum
      */

  SpeedVector wind;
  if (!ReadBearing(line, wind.bearing))
        return false;

  double windspeed;
  if (!line.ReadChecked(windspeed))
      return false;
  wind.norm = Units::ToSysUnit(windspeed, Unit::KILOMETER_PER_HOUR);

  switch (line.ReadOneChar()) {
  case 'A':
    info.ProvideExternalWind(wind);
    break;
  case 'I':
    info.ProvideExternalInstantaneousWind(wind);
    break;
  default:
    return false;
  }

  return true;
}

static Device *
LarusCreateOnPort([[maybe_unused]] const DeviceConfig &config, Port &com_port)
{
  return new LarusDevice(com_port);
}

const struct DeviceRegister larus_driver = {
  _T("Larus"),
  _T("Larus"),
  DeviceRegister::SEND_SETTINGS,
  LarusCreateOnPort,
};
