#include "Airplane.h"
#include <math.h>
#include "raymath.h"
#include "Global.h"
#include "Structures.h"

Airplane::Airplane(Texture2D tex, int sz, Color clr, OrbitZone* zone, Vector2 radarCenter, float radarRadius)
        : texture(tex), size(sz), color(clr),
          speed(PLANE_SPEEDS.at(sz)),
          movementSpeed(PARKING_SPEEDS.at(sz)),
          assignedOrbit(zone),
          isFlying(true),
          orbitAngle((float)GetRandomValue(0, 360))
{
    float angle = GetRandomValue(0, 360) * DEG2RAD;
    float radius = GetRandomValue(20, radarRadius - 20);
    radarPosition = {
            radarCenter.x + radius * cosf(angle),
            radarCenter.y + radius * sinf(angle)
    };

    if (assignedOrbit) {
        position.x = assignedOrbit->center.x + assignedOrbit->radius * cosf(orbitAngle * DEG2RAD);
        position.y = assignedOrbit->center.y + assignedOrbit->radius * sinf(orbitAngle * DEG2RAD);
        assignedOrbit->occupied = true;
    }
}

void Airplane::UpdateOrbit() {
    if (assignedOrbit && isFlying) {
        orbitAngle += speed * GetFrameTime();
        if (orbitAngle >= 360) orbitAngle -= 360;

        position.x = assignedOrbit->center.x + assignedOrbit->radius * cosf(orbitAngle * DEG2RAD);
        position.y = assignedOrbit->center.y + assignedOrbit->radius * sinf(orbitAngle * DEG2RAD);
        rotation = orbitAngle - 180;
    }
}

void Airplane::Draw() const {
    float scale = PLANE_SCALES.at(size);
    Rectangle dest = {
            position.x,
            position.y,
            texture.width * scale,
            texture.height * scale
    };
    Vector2 origin = { dest.width/2, dest.height/2 };
    DrawTexturePro(texture,
                   {0,0,(float)texture.width,(float)texture.height},
                   dest,
                   origin,
                   rotation,
                   color);
}

void Airplane::StartLanding(const LandingStrip& strip) {
    isLanding = true;
    isFlying = false;

    ParkingSpot* targetSpot = nullptr;
    for (auto& spot : parkingSpots) {
        if (!spot.occupied) {
            targetSpot = &spot;
            spot.occupied = true;
            break;
        }
    }
    if (targetSpot) {
        targetSpot->occupied = true;
        currentPath.clear();
        currentPath.push_back(strip.startPoint);
        currentPath.push_back(strip.endPoint);
        currentPath.insert(currentPath.end(), strip.pathToParking.begin(), strip.pathToParking.end());
        currentPath.push_back({targetSpot->area.x + 3, targetSpot->area.y + 32});

        currentPathIndex = 0;
        assignedOrbit->occupied = false;
    }
}

void Airplane::UpdateMovement() {
    if (isParked) {
        return;
    }
    if (currentPathIndex >= currentPath.size()) {
        isParked = true;
        Global::parkedCount++;
        TraceLog(LOG_INFO, "Plane parked. Total parked: %d", Global::parkedCount);
        if (Global::parkedCount >= Global::parkedNeeded) {
            CompleteLevel();
        }
        return;
    }
    Vector2 target = currentPath[currentPathIndex];
    Vector2 direction = Vector2Subtract(target, position);
    float distance = Vector2Length(direction);

    if (distance < 2.0f) {
        currentPathIndex++;
    }

    position = Vector2Add(position,
                          Vector2Scale(Vector2Normalize(direction), movementSpeed * GetFrameTime()));
    rotation = atan2f(direction.y, direction.x) * RAD2DEG + 90;
}