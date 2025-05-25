#include <cstdio>
#include <queue>
#include <string>

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
std::vector<Airplane> inactive_airplanes;
std::vector<OrbitZone> orbitZones(NUM_ORBIT_ZONES);
std::vector<ParkingSpot> parkingSpots(NUM_PARKING_SPOTS);
int level = 0;
std::queue<std::pair<Airplane, int>> queue_plane;
long long coef;
bool isComplete = false;
int lim = 0;
namespace Global {
    int level = 1;
    int parkedNeeded = 4;
    int parkedCount = 0;
    int count_planes[5] = {1, 1, 1, 1, 0};
}
struct Level {
    int number;
    int task_parked;
    int count_planes[5];
};
bool IsSelectionState = true;
int current_level = 1;
int fine = 0;
Level levels[4] = {
    {1, 4, {1, 1, 1, 1, 0}},
    {2, 6, {1, 1, 1, 1, 2}},
{3, 10, {4, 2, 1, 1, 2}},
{1, 14, {3, 3, 2, 4, 2}}
};
std::string names[5] = {"SKYHUWK", "KING AIR 350", "BOEING 777", "BOEING 737", "AIREUS A230"};
void CompleteLevel()
{
    isComplete = true;
}
void ResetGame()
{
    lim = 0;
    inactive_airplanes.clear();
    Global::parkedNeeded = levels[current_level - 1].task_parked;
    while (!queue_plane.empty()) { queue_plane.pop(); }
    int* counts = levels[current_level - 1].count_planes;
    for (int i = 0; i < 5; ++i)
    {
        Global::count_planes[i] = counts[i];
    }
    isComplete = false;
    airplanes.clear();
    for (auto& spot : parkingSpots) spot.occupied = false;
    for (auto& zone : orbitZones) zone.occupied = false;
    Global::parkedCount = 0;
}



// Глобальные переменные

Color NONE = {0,0,0,0};
Strip landingStrips[4];

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
Texture2D planeTextures[5];
const int radarSize = screenWidth / 7 + 60;
Vector2 radarPos = { screenWidth - radarSize - 10 + 100 + 10, screenHeight - radarSize - 10 + 100 };
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
void CreatePlane()
{
    if (((long long)(GetTime()) - coef) == lim)
    {
        lim += 11;
        std::vector<int> full;
        for (int i = 0; i < 5; i++) {
            if (Global::count_planes[i] != 0)
                full.push_back(i);
        }
        if (full.empty()) return;
        int num = full[rand() % full.size()];
        for (auto& zone : orbitZones) {
            if (!zone.occupied) {
                airplanes.emplace_back
                (
                    planeTextures[num],
                    num+1,      // Размер (1-5)
                    WHITE,
                    &zone,
                    radarPos,
                    radarSize/2-15
                    );
                break;
            }
        }
        queue_plane.push({(airplanes.back()), (long long)(GetTime()) - coef});
        Global::count_planes[num]--;

    }
}
void StartLevel() {
    ResetGame();
    fine = 0;
    IsSelectionState = false;
    coef = (long long)(GetTime());
    int* counts = levels[current_level - 1].count_planes;
    for (int i = 0; i < 5; ++i)
    {
        Global::count_planes[i] = counts[i];
    }
    /*for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < counts[i]; ++j)
        {
            for(auto& zone : orbitZones)
            {
                if (!zone.occupied) {
                    airplanes.emplace_back(
                        planeTextures[i],
                        i+1,
                        WHITE,
                        &zone,
                        radarPos,
                        radarSize/2-15
                        );
                    break;
                }
            }
        }
    }*/
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
int main()
{
    InitWindow(screenWidth, screenHeight, "Air Traffic Control");
    if (Global::level >= 1){
        level= Global::level;
    }
    float alpha = 0;
    // Загрузка текстур
    Texture2D fogTexture; // Это переменная, где будет храниться туман
    fogTexture = LoadTexture("res/4321.jpg"); // Загружаем картинку
    Texture2D background = LoadTexture("res/1234.png");
    if (fogTexture.id == 0) {
        printf("Ошибка: не удалось загрузить туман!\n");
    }
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
    float rotationAngle = 0.0f;
    const float rotationSpeed = 1.5f;

    while (!WindowShouldClose()) {
        if (Global::parkedNeeded - Global::parkedCount == 0)
        {
            IsSelectionState = true;
            ResetGame();
        }
        if (IsSelectionState == false) {
            CreatePlane();
        }
        rotationAngle += rotationSpeed;
        if(rotationAngle >= 360) rotationAngle = 0;
        int key = GetKeyPressed();
        if (key >= KEY_ONE && key <= KEY_FIVE)
        {
            for (auto& plane : airplanes) {
                if (plane.size == key - KEY_ONE + 1 && plane.isParked == false && plane.isParking == false && plane.isLanding == false) {
                    selectedPlane = &plane;
                    break;
                }
            }
        }
        // Обработка ввода
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();

            //Выбор самолета
            for (auto& plane : airplanes) {
                if (CheckCollisionPointCircle(mousePos, plane.position, 80)&& plane.isParked == false && plane.isParking == false && plane.isLanding == false) {
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
            if (CheckCollisionPointRec(mousePos, buttons[0]))
            {
                StartLevel();
            }
            if (CheckCollisionPointRec(mousePos, buttons[1]))
            {
                StartLevel();
            }
            if (CheckCollisionPointRec(mousePos, buttons[2]))
            {
                IsSelectionState = true;
                if (current_level < 4) {
                    current_level++;
                }
                ResetGame();
                Global::level = current_level;
                Global::parkedCount = 0;
                Global::parkedNeeded = levels[current_level - 1].task_parked;
            }
            if (CheckCollisionPointRec(mousePos, buttons[3]))
            {
                IsSelectionState = true;
                if (current_level > 1) {
                    current_level--;
                }
                ResetGame();
                Global::level = current_level;
                Global::parkedCount = 0;
                Global::parkedNeeded = levels[current_level - 1].task_parked;
            }
            if (CheckCollisionPointRec(mousePos, buttons[4]))
            {
                break;
            }
            /*for(int i = 0; i < 5; i++) {
                if(CheckCollisionPointRec(mousePos, buttons[i]))
                {
                    Global::count_planes[i]++;
                    // Ищем свободную зону
                    for(auto& zone : orbitZones) {
                        if(!zone.occupied)
                            {
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
            }*/
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
        const char* buttonTexts[5] = {"Start level", "Restart level", "Next level", "Previous level", "Exit"};
        for(int i = 0; i < numButtons; i++) {
            Color btnColor = CheckCollisionPointRec(GetMousePosition(), buttons[i]) ? RED : BLACK;
            DrawRectangleRec(buttons[i], btnColor);
            DrawText(buttonTexts[i],
                     buttons[i].x + (buttons[i].width - MeasureText(buttonTexts[i], 20))/2,
                     buttons[i].y + 15,
                     20, GREEN);
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
        for (const auto& plane : inactive_airplanes) {
            plane.Draw();
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
        if (!queue_plane.empty() && (45 - ((long long)(GetTime()) - coef - queue_plane.front().second) <= 0))
        {
            fine++;
            queue_plane.pop();
            airplanes.erase(airplanes.begin());
            Global::parkedCount++;
        }
        int len = queue_plane.size();
        for (int i = 0; i < len; ++i) {
            std::pair<Airplane, int> a = queue_plane.front();
            if (airplanes[i].isParked == true)
            {
                inactive_airplanes.emplace_back(airplanes[i]);
                airplanes.erase(airplanes.begin() + i);
                queue_plane.pop();
                continue;
            }
            queue_plane.push(a);
            queue_plane.pop();
        }
// Отрисовка самолетов на радаре
        for (const auto& plane : airplanes) {
            if (plane.isFlying) {
                // Получаем центр и радиус радара
                Vector2 radarCenter = radarPos;
                float radarRadius = radarSize / 2 - 15;
                }
            }

        for (const auto& plane : airplanes) {
            if (plane.isFlying) {
                DrawCircleV(plane.radarPosition, 4, GREEN);
            }
        }

        DrawText(TextFormat("Level: %d", Global::level), screenWidth - 520, 50, 30, GREEN);
        DrawText(TextFormat("Left: %d", Global::parkedNeeded - Global::parkedCount), screenWidth - 520, 80, 30, GREEN);
        int temp = 120;
        if (IsSelectionState == false)
            {
            DrawText(TextFormat("%d:%d", ((long long)(GetTime()) - coef) / 60,  ((long long)(GetTime()) - coef) % 60), screenWidth - 300, 50, 30, GREEN);
        }
        DrawText(TextFormat("Fine: %d", fine), screenWidth - 300, 80, 30, GREEN);
        int count = queue_plane.size();
        for (int i = 0; i < count; ++i)
            {
            std::pair<Airplane, int> a = queue_plane.front();
            DrawText(names[a.first.size - 1].c_str(), screenWidth - 520, temp, 20, GREEN);
            DrawText(TextFormat("%d", 45 - ((long long)(GetTime()) - coef - a.second)), screenWidth - 300, temp, 20, GREEN);
            queue_plane.push(a);
            queue_plane.pop();
            temp += 30;
        }
        if (IsSelectionState == true) {
            DrawText(TextFormat("SKYHUWK: %d", Global::count_planes[0]), screenWidth - 520, 120, 20, GREEN);
            DrawText(TextFormat("KING AIR 350: %d", Global::count_planes[1]), screenWidth - 520, 150, 20, GREEN);
            DrawText(TextFormat("BOEING 777: %d", Global::count_planes[2]), screenWidth - 520, 180, 20, GREEN);
            DrawText(TextFormat("BOEING 737: %d", Global::count_planes[3]), screenWidth - 520, 210, 20, GREEN);
            DrawText(TextFormat("AIREUS A230: %d", Global::count_planes[4]), screenWidth - 520, 240, 20, GREEN);

        }
        if (isComplete == true)
            {
            IsSelectionState = true;
            ResetGame();
        }
        float gameWidth = screenWidth / 1.6f;
        float gameHeight = screenHeight;




// Считаем, насколько густой туман
// Прозрачность зависит от уровня(хуй там)
        if (IsKeyPressed(KEY_D)) alpha = alpha+0.05;
        if (IsKeyPressed(KEY_S)) alpha = alpha-0.05;


// Создаем цвет для тумана с прозрачностью
        Color fogColor = {255, 255, 255, (unsigned char)(alpha * 255)};

// Рисуем туман поверх поля
        DrawTexturePro(
                fogTexture, // Текстура тумана (4321.png)
                {0, 0, (float)fogTexture.width, (float)fogTexture.height}, // Берем всю картинку
                {0, 0, gameWidth, gameHeight}, // Рисуем в ту же область, что и поле
                {0, 0}, // Точка поворота (не трогаем)
                0.0f,   // Угол поворота (не поворачиваем)
                fogColor // Цвет с прозрачностью
        );
        DrawText("Select signal:",1000-50-25,400, 25,GREEN);



        DrawFPS(1000-78,10);
        EndDrawing();
    }
    GetTime();
    // Очистка
    UnloadTexture(background);
    for(auto& tex : planeTextures) UnloadTexture(tex);
    CloseWindow();
    return 0;
}