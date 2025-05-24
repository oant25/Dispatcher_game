#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "raylib.h"
#include "runway.h"

class Aircraft {
private:
    Vector2 targetPosition;
    Texture2D texture;
    Vector2 position;
    float rotation;
    float scale;
    float speed;
    int planeSize; // 0 (smallest) to 4 (largest)
    Runway* assignedRunway;
    ParkingSpot* parkingSpot;
    bool isFlying;
    bool isLanding;
    bool isParked;
    bool isOnRunway;              // Находится на ВПП
    bool isMovingToParking;       // Двигается к парковке

    bool RequestLanding(Runway* runways, ParkingSpot* parkingSpots, int runwayCount, int parkingCount);

public:
    void SetTarget(Vector2 target); // Объявление метода
    void StartLanding();
    Aircraft(Texture2D tex, Vector2 startPos, float sc, float spd, int size);
    void Update();
    void Draw() const;
    bool RequestLanding(Runway* runways, int count);
    bool IsParked() const;
    bool IsLanding() const;
    void Land(Runway& runway);    // Начать посадку на ВПП
    void MoveToParking(ParkingSpot& spot); // Начать движение к парковке

};

#endif // AIRCRAFT_H