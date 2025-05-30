// Airplane.h
#ifndef AIRPLANE_H
#define AIRPLANE_H

#include "raylib.h"
#include "Structures.h"
#include <vector>    // Для std::vector
#include <cstddef>   // Для size_t

class Airplane {
public:
    Texture2D texture;
    Vector2 position;
    float speed;
    float rotation;
    int size;
    bool isFlying;
    bool isParked = false;
    int currentStrip;
    int parkingSpot;
    Color color;
    float orbitRadius;
    bool isLanding = false;
    bool isParking = false;
    std::vector<Vector2> currentPath;
    size_t currentPathIndex = 0;
    float movementSpeed = 1.0f;
    OrbitZone* assignedOrbit;
    float orbitAngle;

    Vector2 radarPosition;

    Airplane(Texture2D tex, int sz, Color clr, OrbitZone* zone, Vector2 radarCenter, float radarRadius);
    void UpdateOrbit();
    void Draw() const;
    void StartLanding(const LandingStrip& strip);
    void UpdateMovement();
};


#endif