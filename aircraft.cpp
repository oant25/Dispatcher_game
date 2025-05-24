#include "aircraft.h"
#include <cmath>
#include "raymath.h" // Добавьте эту строку в начало файла


void Aircraft::Land(Runway& runway) {
    if (!runway.isOccupied) {
        targetPosition = {
                runway.area.x + runway.area.width / 2,
                runway.area.y + runway.area.height / 2
        };
        isFlying = false;
        isLanding = true;
        isOnRunway = false;
        runway.isOccupied = true;
    }
}

void Aircraft::MoveToParking(ParkingSpot& spot) {
    if (!spot.isOccupied) {
        targetPosition = {
                spot.area.x + spot.area.width / 2,
                spot.area.y + spot.area.height / 2
        };
        isMovingToParking = true;
        spot.isOccupied = true;
    }
}
Aircraft::Aircraft(Texture2D tex, Vector2 startPos, float sc, float spd, int size)
        : texture(tex), position(startPos), rotation(0.0f), scale(sc), speed(spd),
          planeSize(size), assignedRunway(nullptr), parkingSpot(nullptr),
          isFlying(true), isLanding(false), isParked(false) {}

void Aircraft::Update() {
    if (isFlying && !isLanding) {
        static float angle = 0.0f;
        angle += speed * GetFrameTime();
        position.x = 700 + cosf(angle) * 300; // Центр экрана: 700, 300
        position.y = 300 + sinf(angle) * 300;
        rotation = atan2f(cosf(angle), -sinf(angle)) * RAD2DEG+90;
    }

    if (isLanding) {
        // Движение к центру ВПП
        Vector2 direction = Vector2Subtract(targetPosition, position);
        float distance = Vector2Length(direction);

        if (distance > 5.0f) {
            direction = Vector2Normalize(direction);
            position.x += direction.x * speed * GetFrameTime() * 100;
            position.y += direction.y * speed * GetFrameTime() * 100;
            rotation = atan2f(direction.y, direction.x) * RAD2DEG + 90;
        } else {
            isLanding = false;
            isOnRunway = true;
        }
    }

    if (isMovingToParking) {
        // Движение к парковке
        Vector2 direction = Vector2Subtract(targetPosition, position);
        float distance = Vector2Length(direction);

        if (distance > 2.0f) {
            direction = Vector2Normalize(direction);
            position.x += direction.x * speed * GetFrameTime() * 50;
            position.y += direction.y * speed * GetFrameTime() * 50;
            rotation = atan2f(direction.y, direction.x) * RAD2DEG + 90;
        } else {
            isMovingToParking = false;
            isParked = true;
        }
    }
}

void Aircraft::Draw() const {
    if (!isParked) {
        Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle dest = { position.x, position.y, texture.width * scale, texture.height * scale };
        Vector2 origin = { (texture.width * scale) / 2, (texture.height * scale) / 2 };
        DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
    }
}
// aircraft.cpp
void Aircraft::SetTarget(Vector2 target) {
    targetPosition = target;
}

void Aircraft::StartLanding() {
    isFlying = false;
    isLanding = true;
}

bool Aircraft::RequestLanding(Runway* runways, ParkingSpot* parkingSpots, int runwayCount, int parkingCount) {
    for (int i = 0; i < runwayCount; i++) {
        if (!runways[i].isOccupied && planeSize <= runways[i].maxPlaneSize) {
            assignedRunway = &runways[i];
            assignedRunway->isOccupied = true;

            // Ищем свободное парковочное место
            for (int j = 0; j < parkingCount; j++) {
                if (!parkingSpots[j].isOccupied) {
                    parkingSpot = &parkingSpots[j];
                    parkingSpot->isOccupied = true;
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

bool Aircraft::IsParked() const { return isParked; }
bool Aircraft::IsLanding() const { return isLanding; }