#include "Airplane.h"
#include <math.h>
#include "raymath.h"

Airplane::Airplane(Texture2D tex, int sz, Color clr, OrbitZone* zone, Vector2 radarCenter, float radarRadius)
        : texture(tex), size(sz), color(clr),
          speed(PLANE_SPEEDS.at(sz)),
          assignedOrbit(zone),
          isFlying(true),
          orbitAngle((float)GetRandomValue(0, 360))
{
    // Генерация случайной позиции на радаре
    float angle = GetRandomValue(0, 360) * DEG2RAD;
    float radius = GetRandomValue(20, radarRadius - 20); // Отступ от краев
    radarPosition = {
            radarCenter.x + radius * cosf(angle),
            radarCenter.y + radius * sinf(angle)
    };

    // Инициализация позиции на орбите
    if (assignedOrbit) {
        position.x = assignedOrbit->center.x + assignedOrbit->radius * cosf(orbitAngle * DEG2RAD);
        position.y = assignedOrbit->center.y + assignedOrbit->radius * sinf(orbitAngle * DEG2RAD);
        assignedOrbit->occupied = true;
    }
}

void Airplane::UpdateOrbit() {
    if (assignedOrbit && isFlying) {
        // Обновление угла с учетом времени кадра
        orbitAngle += speed * GetFrameTime();
        if (orbitAngle >= 360) orbitAngle -= 360;

        // Расчет новой позиции
        position.x = assignedOrbit->center.x + assignedOrbit->radius * cosf(orbitAngle * DEG2RAD);
        position.y = assignedOrbit->center.y + assignedOrbit->radius * sinf(orbitAngle * DEG2RAD);

        // Поворот текстуры (0° = вверх)
        rotation = orbitAngle -180;

        }
}


// Airplane.cpp
void Airplane::Draw() const {
    // Получаем масштаб из конфигурации
    float scale = PLANE_SCALES.at(size); // Используем .at() для безопасности

    // Отрисовка текстуры
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
    currentPath = {
            strip.startPoint,
            strip.endPoint
    };
    currentPath.insert(currentPath.end(), strip.pathToParking.begin(), strip.pathToParking.end());
    currentPathIndex = 0;

    // Освобождаем орбиту для других самолетов
    if (assignedOrbit) assignedOrbit->occupied = false;
}

void Airplane::UpdateMovement() {
    if (currentPathIndex >= currentPath.size()) return;

    Vector2 target = currentPath[currentPathIndex];
    Vector2 direction = Vector2Subtract(target, position);
    float distance = Vector2Length(direction);


    if (distance < 2.0f) {
        currentPathIndex++;
        if (currentPathIndex >= currentPath.size()) {
            isLanding = false;
            isParking = true;
            return;
        }
        target = currentPath[currentPathIndex];
        direction = Vector2Subtract(target, position);
    }

    position = Vector2Add(position,
                          Vector2Scale(Vector2Normalize(direction), movementSpeed));
    rotation = atan2f(direction.y, direction.x) * RAD2DEG + 90;
}