#ifndef RUNWAY_H
#define RUNWAY_H

#include "raylib.h"

struct Runway {
    Rectangle area;
    bool isOccupied;
    int maxPlaneSize;
};

struct ParkingSpot {
    Rectangle area;
    bool isOccupied;
};

#endif // RUNWAY_H