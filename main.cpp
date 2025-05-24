#include "raylib.h"
#include "Airplane.h"
#include "Structures.h"
#include <vector>
#include "raymath.h"
const int screenWidth = 1450;
const int screenHeight = 750;

#include "Global.h"


// Инициализация глобальных переменных
std::vector<Airplane> airplanes;
std::vector<OrbitZone> orbitZones(NUM_ORBIT_ZONES);
std::vector<ParkingSpot> parkingSpots(NUM_PARKING_SPOTS);

namespace Global {
    int level = 1;
    int parkedNeeded = 12;
    int parkedCount = 0;
}

void ResetGame() {
    airplanes.clear();
    for (auto& spot : parkingSpots) spot.occupied = false;
    for (auto& zone : orbitZones) zone.occupied = false;
    Global::parkedCount = 0;
    Global::level++;
}



// Глобальные переменные

Color NONE = {0,0,0,0};
Strip landingStrips[4];// Увеличили с 7 до 14

int selectedStripIndex = -1;
int selectedSize = -1;
Color DGRAY = {30,30,30,0};
Color BBLACK = {0,0,0,0};

std::vector<LandingStrip> landingStripsData = {
        { // Большая полоса 1
                .startPoint = {704, 130},
                .endPoint = {162, 133},
                .pathToParking = {{147, 167}, {150, 394}, {265, 420}}
        },
        { // Большая полоса 2
                .startPoint = {715, 248},
                .endPoint = {162, 240},
                .pathToParking = {{436, 267}, {450, 404}, {330, 460}}
        },
        { // Маленькая полоса 1
                .startPoint = {660, 300},
                .endPoint = {180, 300},
                .pathToParking = {{178, 378}, {359, 429}}
        },
        { // Маленькая полоса 2
                .startPoint = {650, 351},
                .endPoint = {180, 350},
                .pathToParking = {{422, 440}}
        }
};

// Обработка выбора
Airplane* selectedPlane = nullptr;
LandingStrip* selectedStrip = nullptr;


void InitializeOrbitZones() {
    Vector2 centers[NUM_ORBIT_ZONES] = {
            {40.0f, 20.0f}, {100.0f, 430.0f},
            {100.0f, 709.0f}, {660.0f, 100.0f},
            {100.0f, 200.0f}, {690.0f, 346.0f},
            {690.0f, 700.0f}, {750.0f, 500.0f}
    };

    for(int i = 0; i < NUM_ORBIT_ZONES; i++) {
        orbitZones[i] = { centers[i], 100.0f, false };
    }
    orbitZones[0] = { centers[0], 150.0f, false };
    orbitZones[1] = { centers[1], 80.0f, false };
    orbitZones[2] = { centers[2], 200.0f, false };
    orbitZones[3] = { centers[3], 150.0f, false };
    orbitZones[4] = { centers[4], 100.0f, false };
    orbitZones[5] = { centers[5], 180.0f, false };
    orbitZones[6] = { centers[6], 100.0f, false };
    orbitZones[7] = { centers[7], 130.0f, false };
}

void InitializeStrips() {
    Rectangle areas[4] = {
            {145, 145, 580, 30},    // Верхняя
            {145, 345, 580, 30},    // Вторая
            {175, 485, 510, 20},    // Третья
            {175, 605, 510, 20}     // Нижняя
    };

    for(int i = 0; i < 4; i++) {
        landingStrips[i] = { areas[i], false, i+1 };
    }
}

void InitializeParking() {
    const float startX = 275.0f; // Начальная позиция X
    const float spacing = 48.0f; // Расстояние между местами
    const int numRows = 2;       // Два ряда парковки

    for(int i = 0; i < NUM_PARKING_SPOTS; i++) {
        // Рассчитываем позицию для двух рядов
        float x = startX + (i % 7) * spacing;
        float y = 408.0f + (i / 7) * 80.0f; // Второй ряд ниже

        parkingSpots[i] = {
                {x, y, 7.0f, 65.0f},
                false,
                i + 1
        };
    }
}
int main() {
    InitWindow(screenWidth, screenHeight, "Air Traffic Control");

    // Загрузка текстур
    Texture2D background = LoadTexture("res/1234.png");
    Texture2D planeTextures[5];
    for(int i = 0; i < 5; i++) {
        planeTextures[i] = LoadTexture(TextFormat("res/s%d.png", i+1));
        if (planeTextures[i].id == 0) {
            TraceLog(LOG_ERROR, "Failed to load texture: res/s%d.png", i+1);
        }
    }
    InitializeOrbitZones();

    InitializeStrips();
    InitializeParking();

    // Настройка кнопок
    const int numButtons = 5;
    Rectangle buttons[numButtons];
    const int buttonHeight = 50;
    const int margin = 10;

    for(int i = 0; i < numButtons; i++) {
        buttons[i] = {
                static_cast<float>(screenWidth - 200 - margin - 320),
                static_cast<float>(440 + i * (buttonHeight + margin)),
                200.0f,
                static_cast<float>(buttonHeight)
        };
    }

    SetTargetFPS(60);

    // Параметры радара
    const int radarSize = screenWidth / 7 + 60;
    Vector2 radarPos = { screenWidth - radarSize - 10 + 100 + 10, screenHeight - radarSize - 10 + 100 };
    float rotationAngle = 0.0f;
    const float rotationSpeed = 1.5f;


    while (!WindowShouldClose()) {


        rotationAngle += rotationSpeed;
        if(rotationAngle >= 360) rotationAngle = 0;

        // Обработка ввода
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();

            // 1. Выбор самолета
            for (auto& plane : airplanes) {
                if (CheckCollisionPointCircle(mousePos, plane.position, 80)) {
                    selectedPlane = &plane;
                    break;
                }
            }

            // 2. Выбор ЛЮБОЙ полосы
            for (auto& strip : landingStripsData) {
                if (CheckCollisionPointRec(mousePos, {strip.startPoint.x, strip.startPoint.y, 100, 100})) {
                    selectedStrip = &strip;
                    break;
                }
            }

            // 3. Назначение посадки
            if (selectedPlane && selectedStrip) {
                // Снимаем ограничения
                selectedPlane->StartLanding(*selectedStrip);
                selectedPlane = nullptr;
                selectedStrip = nullptr;
            }
        }

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();

            for(int i = 0; i < 5; i++) {
                if(CheckCollisionPointRec(mousePos, buttons[i])) {
                    // Ищем свободную зону
                    for(auto& zone : orbitZones) {
                        if(!zone.occupied) {
                            airplanes.emplace_back(
                                    planeTextures[i],
                                    i+1,      // Размер (1-5)
                                    WHITE,
                                    &zone,
                                    radarPos,
                                    radarSize/2-15
                            );
                            break;
                        }
                    }
                }
            }
        }

        // Отрисовка
        BeginDrawing();
        ClearBackground(DGRAY);

        // Фоновая текстура
        DrawTexturePro(
                background,
                {0,0,(float)background.width,(float)background.height},
                {0,0,screenWidth/1.6f,(float)screenHeight},
                {0,0},
                0.0f,
                WHITE
        );

        // Самолеты



        // Парковка
        DrawRectangle(235,390,370,110,BBLACK);
        for(int i = 0; i < 7; i++) {
            DrawRectangle(275 + 48*i, 408, 7, 65, NONE);
        }

        // Радар
        DrawCircleV(radarPos, radarSize/2, Fade(DARKGRAY, 0.3f));
        DrawCircleLines(radarPos.x, radarPos.y, radarSize/2, DARKBROWN);
        DrawCircleSector(radarPos, radarSize/2 - 5, rotationAngle - 10, rotationAngle + 10, 32, Fade(GREEN, 0.3f));
        DrawLineEx(radarPos, (Vector2){
                radarPos.x + (radarSize/2 - 10) * cosf(rotationAngle * DEG2RAD),
                radarPos.y + (radarSize/2 - 10) * sinf(rotationAngle * DEG2RAD)
        }, 3.0f, GREEN);
        DrawCircleLinesV(radarPos, radarSize/2 - 15, GREEN);
        DrawCircleV(radarPos, 5, GREEN);

        // Кнопки
        const char* buttonTexts[5] = {"Size 1", "Size 2", "Size 3", "Size 4", "Size 5"};
        for(int i = 0; i < numButtons; i++) {
            Color btnColor = CheckCollisionPointRec(GetMousePosition(), buttons[i]) ? RED : BLACK;
            DrawRectangleRec(buttons[i], btnColor);
            DrawText(buttonTexts[i],
                     buttons[i].x + (buttons[i].width - MeasureText(buttonTexts[i], 20))/2,
                     buttons[i].y + 15,
                     20, WHITE);
        }
        for(const auto& zone : orbitZones) {
            DrawCircleLines(
                    (int)zone.center.x,
                    (int)zone.center.y,
                    zone.radius,
                    zone.occupied ? NONE : NONE
            );
        }
        for (auto& plane : airplanes) {
            if (plane.isLanding) {
                plane.UpdateMovement();
            }
        }

        for (auto& plane : airplanes) {
            if (plane.isFlying) {
                plane.UpdateOrbit(); // Должен вызываться каждый кадр
            }
        }

        for(const auto& plane : airplanes) {
            plane.Draw();
        }
            for (const auto& strip : landingStripsData) { // Используем landingStripsData
                DrawCircleV(strip.startPoint, 10, strip.occupied ? NONE : NONE);
                DrawCircleV(strip.endPoint, 10, NONE);
            }
        DrawCircleV(radarPos, radarSize/2, Fade(DARKGRAY, 0.3f));
        DrawCircleLines(radarPos.x, radarPos.y, radarSize/2, DARKBROWN);

// Отрисовка самолетов на радаре
        for (const auto& plane : airplanes) {
            if (plane.isFlying) {
                // Получаем центр и радиус радара
                Vector2 radarCenter = radarPos;
                float radarRadius = radarSize / 2 - 15;

                // Рассчитываем относительное положение самолета
                // Масштабируем координаты
                // TraceLog(LOG_INFO, "Точка радара: (%.1f, %.1f)", radarPoint.x, radarPoint.y);
                }
            }

        for (const auto& plane : airplanes) {
            if (plane.isFlying) {
                DrawCircleV(plane.radarPosition, 4, GREEN);
            }
        }

        DrawText(TextFormat("Parked: %d", Global::parkedCount),
                 screenWidth - 220, 20, 30, GREEN);

        for (const auto& strip : landingStripsData) {
            // Рисуем линии между точками пути
            for (size_t i = 0; i < strip.pathToParking.size() - 1; ++i) {
                DrawLineV(strip.pathToParking[i], strip.pathToParking[i+1], RED);
            }
        }

        DrawText(TextFormat("Level: %d", Global::level), screenWidth - 220, 50, 30, GREEN);
        DrawText(TextFormat("Left: %d", Global::parkedNeeded - Global::parkedCount), screenWidth - 220, 80, 30, GREEN);




        DrawFPS(1000-78,10);
        EndDrawing();
    }

    // Очистка
    UnloadTexture(background);
    for(auto& tex : planeTextures) UnloadTexture(tex);
    CloseWindow();
    return 0;
}