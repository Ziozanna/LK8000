/*
 * LK8000 Tactical Flight Computer -  WWW.LK8000.IT
 * Released under GNU/GPL License v.2 or later
 * See CREDITS.TXT file for authors and copyrights
 *
 * File:   GDL90Protocol.cpp
 */

#include "GDL90Protocol.h"
#include "externs.h"

GDL90Parser::GDL90Parser() {}

GDL90Parser::~GDL90Parser() {}

bool GDL90Parser::ParseMessage(const uint8_t* buffer, size_t length) {
  if (!buffer || length < 2) {
    return false;
  }

  switch (buffer[0]) {
    case GDL90_ID_TRAFFIC_REPORT:
      return ParseTrafficReport(buffer, length);
    case GDL90_ID_OWNSHIP_REPORT:
      return true; // TODO
    default:
      return false;
  }
}

bool GDL90Parser::ParseTrafficReport(const uint8_t* buffer, size_t length) {
  if (!buffer || length < 28) {
    return false;
  }

  const uint32_t icao =
      (static_cast<uint32_t>(buffer[1]) << 16) |
      (static_cast<uint32_t>(buffer[2]) << 8) |
      static_cast<uint32_t>(buffer[3]);

  int32_t raw_lat =
      (static_cast<int32_t>(buffer[4]) << 16) |
      (static_cast<int32_t>(buffer[5]) << 8) |
      static_cast<int32_t>(buffer[6]);
  if (raw_lat & 0x800000) raw_lat |= 0xFF000000;
  const double lat = raw_lat * (180.0 / 8388608.0);

  int32_t raw_lon =
      (static_cast<int32_t>(buffer[7]) << 16) |
      (static_cast<int32_t>(buffer[8]) << 8) |
      static_cast<int32_t>(buffer[9]);
  if (raw_lon & 0x800000) raw_lon |= 0xFF000000;
  const double lon = raw_lon * (180.0 / 8388608.0);

  const uint16_t alt_raw =
      (static_cast<uint16_t>(buffer[11]) << 4) |
      ((static_cast<uint16_t>(buffer[12]) >> 4) & 0x0F);
  const int32_t altitude_ft = static_cast<int32_t>(alt_raw) * 25 - 1000;

  int flarmSlot = -1;
  for (int i = 0; i < FLARM_MAX_TRAFFIC; ++i) {
    if (GPS_INFO.FLARM_Traffic[i].RadioId == icao) {
      flarmSlot = i;
      break;
    }
    if (flarmSlot == -1 && GPS_INFO.FLARM_Traffic[i].Status == LKT_EMPTY) {
      flarmSlot = i;
    }
  }

  if (flarmSlot >= 0) {
    FLARM_TRAFFIC& t = GPS_INFO.FLARM_Traffic[flarmSlot];
    t.RadioId = icao;
    t.Latitude = lat;
    t.Longitude = lon;
    t.Altitude = altitude_ft * 0.3048;
    t.Status = LKT_REAL;
    t.Time_Fix = GPS_INFO.Time;
  }

  return true;
}