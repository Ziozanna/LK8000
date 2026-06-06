/*
   LK8000 Tactical Flight Computer -  WWW.LK8000.IT
   Released under GNU/GPL License v.2 or later
   See CREDITS.TXT file for authors and copyrights
*/

#include "externs.h"
#include "NavFunctions.h"
#include "AATDistance.h"
#include "GADirectTo.h"

void GA_UpdateDirectToOriginForCourseCapture(NMEA_INFO* Basic) {
  if (!ISGAAIRCRAFT) return;
  if (!DirectToActive || DirectToWaypointIndex < 0) return;
  if (!ValidWayPointFast(DirectToWaypointIndex)) return;

  double dist = 0., bearing_to_fix = 0.;
  DistanceBearing(Basic->Latitude, Basic->Longitude,
                  WayPointList[DirectToWaypointIndex].Latitude,
                  WayPointList[DirectToWaypointIndex].Longitude,
                  &dist, &bearing_to_fix);

  constexpr double COURSE_CAPTURE_DEG = 15.0;
  if (fabs(AngleLimit180(bearing_to_fix - Basic->TrackBearing)) > COURSE_CAPTURE_DEG) {
    DirectToOriginLat = Basic->Latitude;
    DirectToOriginLon = Basic->Longitude;
  }
}

void GA_CheckDirectToOffTaskArrival(NMEA_INFO* Basic) {
  if (!ISGAAIRCRAFT) return;
  if (!DirectToActive || DirectToWaypointIndex < 0) return;
  if (!ValidWayPointFast(DirectToWaypointIndex)) return;

  double dist = 0., bearing = 0.;
  DistanceBearing(Basic->Latitude, Basic->Longitude,
                  WayPointList[DirectToWaypointIndex].Latitude,
                  WayPointList[DirectToWaypointIndex].Longitude,
                  &dist, &bearing);

  double radius = 500.;
  if (ValidTaskPointFast(ActiveTaskPoint)) {
    radius = (Task[ActiveTaskPoint].AATType == sector_type_t::SECTOR)
             ? Task[ActiveTaskPoint].AATSectorRadius
             : Task[ActiveTaskPoint].AATCircleRadius;
  }

  if (dist < radius) {
    const double fix_lat = WayPointList[DirectToWaypointIndex].Latitude;
    const double fix_lon = WayPointList[DirectToWaypointIndex].Longitude;

    DirectToOriginLat = fix_lat;
    DirectToOriginLon = fix_lon;

    double min_dist = 1e20;
    int nearest_tp = ActiveTaskPoint;
    for (int i = ActiveTaskPoint; i < MAXTASKPOINTS; i++) {
      if (!ValidTaskPointFast(i)) break;
      double d = 0., b = 0.;
      DistanceBearing(fix_lat, fix_lon,
                      WayPointList[Task[i].Index].Latitude,
                      WayPointList[Task[i].Index].Longitude,
                      &d, &b);
      if (d < min_dist) {
        min_dist = d;
        nearest_tp = i;
      }
    }
    ActiveTaskPoint = nearest_tp;
    DirectToWaypointIndex = -1;
  }
}

bool GA_ComputeDirectToDistanceBearing(NMEA_INFO* Basic, DERIVED_INFO* Calculated) {
  if (!ISGAAIRCRAFT) return false;
  if (!DirectToActive || !ValidWayPointFast(DirectToWaypointIndex)) return false;

  DistanceBearing(Basic->Latitude, Basic->Longitude,
                  WayPointList[DirectToWaypointIndex].Latitude,
                  WayPointList[DirectToWaypointIndex].Longitude,
                  &Calculated->WaypointDistance,
                  &Calculated->WaypointBearing);
  Calculated->ZoomDistance = Calculated->WaypointDistance;
  return true;
}

int GA_GetDirectToNavIndex() {
  if (ISGAAIRCRAFT && DirectToActive && ValidWayPointFast(DirectToWaypointIndex)) {
    return DirectToWaypointIndex;
  }
  return -1;
}

void GA_ApplyDirectToAutopilotOverride(int& prev_index, int& next_index) {
  if (ISGAAIRCRAFT && DirectToActive && DirectToWaypointIndex >= 0
      && ValidWayPointFast(DirectToWaypointIndex)) {
    next_index = DirectToWaypointIndex;
    prev_index = -1;
  }
}
