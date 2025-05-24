// Structures.h
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "raylib.h"
#include <unordered_map>
#include <vector> // Добавлен заголовок для std::vector

extern std::vector<class Airplane> airplanes;
extern std::vector<struct OrbitZone> orbitZones;
extern std::vector<struct ParkingSpot> parkingSpots;

namespace Global {
    extern int level;
    extern int parkedNeeded;
    extern int parkedCount;
}
void ResetGame();

const int NUM_ORBIT_ZONES = 8;
const int NUM_PARKING_SPOTS = 14;

struct OrbitZone {
    Vector2 center;
    float radius;
    bool occupied;
};


const std::unordered_map<int, float> PLANE_SPEEDS = {
        {1, 50.0f},  // Size 1: высокая скорость
        {2, 40.0f},
        {3, 30.0f},
        {4, 20.0f},
        {5, 10.0f}   // Size 5: низкая скорость
};
const std::unordered_map<int, float> PARKING_SPEEDS = {
        {1, 120.0f}, // Размер 1: высокая скорость
        {2, 100.0f},
        {3, 80.0f},
        {4, 60.0f},
        {5, 40.0f}   // Размер 5: низкая скорость
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
    Vector2 startPoint;      // Начало полосы
    Vector2 endPoint;        // Конец полосы
    std::vector<Vector2> pathToParking; // Путь ОТ КОНЦА ПОЛОСЫ до парковки
    bool occupied = false;
};

extern std::vector<ParkingSpot> parkingSpots; // Глобальное объявление

#endif