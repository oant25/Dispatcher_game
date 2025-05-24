// Structures.h
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "raylib.h"
#include <unordered_map>
#include <vector> // Добавлен заголовок для std::vector

struct OrbitZone {
    Vector2 center;
    float radius;
    bool occupied;
};

// Structures.h
const std::unordered_map<int, float> PLANE_SCALES = {
        {1, 0.02f},  // Размер 1: маленький
        {2, 0.03f},  // Размер 2: средний
        {3, 0.04f},  // Размер 3: большой
        {4, 0.05f},  // Размер 4: очень большой
        {5, 0.06f}    // Размер 5: максимальный
};

struct Strip {
    Rectangle area;
    bool occupied;
    int number;
};

struct ParkingSpot {
    Rectangle area;
    bool occupied;
    int number;
};

struct LandingStrip {
    Vector2 startPoint;
    Vector2 endPoint;
    std::vector<Vector2> pathToParking;
    bool occupied = false;
};

#endif