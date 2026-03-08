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

class GDL90Parser {
public:
  GDL90Parser();
  ~GDL90Parser();
  
  bool ParseMessage(const uint8_t* buffer, size_t length);

private:
  // Parser state variables
};

#endif // GDL90_PROTOCOL_H
