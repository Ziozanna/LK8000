/*
   LK8000 Tactical Flight Computer -  WWW.LK8000.IT
   Released under GNU/GPL License v.2 or later
   See CREDITS.TXT file for authors and copyrights
*/

#ifndef GA_DIRECT_TO_H
#define GA_DIRECT_TO_H

struct NMEA_INFO;
struct DERIVED_INFO;

// --- Calc/Task functions (GADirectTo.cpp) ---

// Slide the XTE origin forward until GPS track aligns with bearing to fix.
void GA_UpdateDirectToOriginForCourseCapture(NMEA_INFO* Basic);

// Check arrival at the off-task fix; transition back to task leg when inside radius.
void GA_CheckDirectToOffTaskArrival(NMEA_INFO* Basic);

// DistanceToNext wrapper: fills WaypointDistance/Bearing/ZoomDistance for the DirectTo fix.
// Returns true if GA DirectTo is active and the calculation was handled (caller must unlock + return).
bool GA_ComputeDirectToDistanceBearing(NMEA_INFO* Basic, DERIVED_INFO* Calculated);

// OverTargets wrapper: returns DirectToWaypointIndex when GA DirectTo is active, else -1.
int GA_GetDirectToNavIndex();

// devGenericAutopilot wrapper: overrides prev/next indices with the DirectTo fix when active.
void GA_ApplyDirectToAutopilotOverride(int& prev_index, int& next_index);

// Find the first remaining task WP (from ActiveTaskPoint onwards) that is logically
// ahead of the given position. Caller must hold CritSec_TaskData.
// Returns the Task array index of that WP, or ActiveTaskPoint as fallback.
int GA_FindNextForwardTaskWP(double from_lat, double from_lon);

// --- Dialog functions (dlgDirectToCountdown.cpp) ---

// Task-point Direct To (from Target dialog): counts down, then advances ActiveTaskPoint.
bool ShowDirectToCountdownDialog(int new_tp);

// Off-task waypoint Direct To (from WayQuick): counts down, then sets DirectToWaypointIndex.
bool ShowDirectToOffTaskDialog(int wp_index);

// Pan-mode Direct To: builds Oracle description, then activates DirectTo to RESWP_PANPOS.
bool ShowDirectToFromPanDialog(int wp_index, double pan_lat, double pan_lon);

#endif
