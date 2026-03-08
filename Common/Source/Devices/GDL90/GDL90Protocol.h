/*
 * LK8000 Tactical Flight Computer -  WWW.LK8000.IT
 * Released under GNU/GPL License v.2 or later
 * See CREDITS.TXT file for authors and copyrights
 *
 * File:   GDL90Protocol.h
 * Author: Bruno de Lacheisserie
 */

#ifndef GDL90_PROTOCOL_H
#define GDL90_PROTOCOL_H

#include <cstdint>
#include <cstddef>

// GDL90 Message IDs
#define GDL90_ID_TRAFFIC_REPORT 0x14
#define GDL90_ID_OWNSHIP_REPORT 0x0A

class GDL90Parser {
public:
  GDL90Parser();
  ~GDL90Parser();
  
  bool ParseMessage(const uint8_t* buffer, size_t length);

private:
  bool ParseTrafficReport(const uint8_t* buffer, size_t length);
  // Parser state variables
};

#endif // GDL90_PROTOCOL_H
