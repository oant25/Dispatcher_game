#include "raylib.h"
#include <cmath>
#include "aircraft.h"
#include "runway.h"

int main() {
    const int screenWidth = 1450;
    const int screenHeight = 750;

    InitWindow(screenWidth, screenHeight, "Color Changer");

    //тест кнопок
    Color colors[5] = {RED, BLUE, BLACK, PINK, VIOLET};
    Color currentColor = BLUE; // Начальный цвет
    Color DGRAY = {30,30,30,255};
    Color BBLACK = {0,0,0,105};
    // консты кнопок
    const int numButtons = 5;
    Rectangle buttons[numButtons];
    const int buttonHeight = 50;
    const int margin = 10;

    // позиционирование кнопок
    for (int i = 0; i < numButtons; i++) {
        buttons[i].width = 200; // Фиксированная ширина кнопок
        buttons[i].x = screenWidth - buttons[i].width - margin - 320; // Смещение от правого края
        buttons[i].y = 440 + i * (buttonHeight + margin);
        buttons[i].height = buttonHeight;
    }

    SetTargetFPS(60);

    // Загрузка фоновой текстуры
    Texture2D background = LoadTexture("res/1234.png"); // Укажите путь к вашему PNG

    Texture2D planeTextures[5];
    for (int i = 0; i < 5; i++) {
        planeTextures[i] = LoadTexture(TextFormat("res/plane%d.png", i)); // Загрузите свои текстуры
    }
    for (int i = 0; i < 5; i++) {
        if (planeTextures[i].id == 0) {
            CloseWindow();
            return -1;
        }
    }

    // Проверка загрузки текстуры
    if (background.id == 0) {
        CloseWindow();
        return -1;
    }


    Rectangle sourceRec = { 0.0f, 0.0f, (float)background.width, (float)background.height };
    Rectangle destRec = { 0.0f, 0.0f, screenWidth/1.6f, (float)screenHeight };
    Vector2 origin = { 0.0f, 0.0f };




    // Размеры радара
    const int radarSize = screenWidth / 7+60;
    Vector2 radarPos = {
            screenWidth - radarSize - 10+100+10,  // Позиция X
            screenHeight - radarSize - 10+100  // Позиция Y
    };

    // Параметры анимации
    float rotationAngle = 0.0f;
    const float rotationSpeed = 1.5f;





    const int numPlanes = 5;
    Aircraft planes[numPlanes] = {
            Aircraft(planeTextures[0], {500, 600}, 0.5f, 1.0f, 0),
            Aircraft(planeTextures[1], {500, 600}, 0.7f, 0.8f, 1),
            Aircraft(planeTextures[2], {500, 600}, 0.6f, 1.2f, 2),
            Aircraft(planeTextures[3], {500, 600}, 0.4f, 1.5f, 3),
            Aircraft(planeTextures[4], {500, 600}, 0.3f, 2.0f, 4)
    };
    Runway runways[] = {
            { Rectangle{145, 115, 580, 60}, false, 4 },
            { Rectangle{145, 215, 580, 60}, false, 4 },
            { Rectangle{175, 285, 510, 40}, false, 2 },
            { Rectangle{175, 330, 510, 40}, false, 2 }
    };

    ParkingSpot parkingSpots[] = {
            { Rectangle{275-15, 408, 40, 65}, false },
            { Rectangle{323-15, 408, 40, 65}, false },
            { Rectangle{371-15, 408, 40, 65}, false },
            { Rectangle{419-15, 408, 40, 65}, false },
            { Rectangle{467-15, 408, 40, 65}, false },
            { Rectangle{515-15, 408, 40, 65}, false },
            { Rectangle{563-15, 408, 40, 65}, false }
    };


    while (!WindowShouldClose()) {

// Обработка клика по парковке
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            for (auto& runway : runways) {
                if (CheckCollisionPointRec(mousePos, runway.area)) {
                    planes[0].Land(runway);
                    break;
                }
            }
        }

// Обработка клика по парковке
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            for (auto& spot : parkingSpots) {
                if (CheckCollisionPointRec(mousePos, spot.area)) {
                    planes[0].MoveToParking(spot);
                    break;
                }
            }
        }


        rotationAngle += rotationSpeed;
        if (rotationAngle >= 360) rotationAngle = 0;

        //кникер чекер
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            for (int i = 0; i < numButtons; i++) {
                if (CheckCollisionPointRec(mousePos, buttons[i])) {
                    currentColor = colors[i];
                }
            }
        }



        // отрисовка
        DrawLine(10,10,60,60,BLACK);
        BeginDrawing();

        // левая панель с текущим цветом
        DrawRectangle(0, 0, screenWidth/1.6, screenHeight, currentColor);
        DrawRectangle(920,10,515,400,BLACK);


        //кнопки
        for (int i = 0; i < numButtons; i++) {
            Color btnColor = CheckCollisionPointRec(GetMousePosition(), buttons[i])
                             ? RED : BLACK;

            DrawRectangleRec(buttons[i], btnColor);
        }

        // текст кнопки
        const char* text = TextFormat("Start level");
        DrawText(text,
                 buttons[0].x + (buttons[0].width - MeasureText(text, 20))/2,
                 buttons[0].y + 15,
                 20, GREEN);

        const char* text1 = TextFormat("MODE Prev");
        DrawText(text1,
                 buttons[1].x + (buttons[1].width - MeasureText(text1, 20))/2,
                 buttons[1].y + 15,
                 20, GREEN);
        const char* text2 = TextFormat("MODE Next");
        DrawText(text2,
                 buttons[2].x + (buttons[2].width - MeasureText(text2, 20))/2,
                 buttons[2].y + 15,
                 20, GREEN);
        const char* text3 = TextFormat("Prev next");
        DrawText(text3,
                 buttons[3].x + (buttons[3].width - MeasureText(text3, 20))/2,
                 buttons[3].y + 15,
                 20, GREEN);
        const char* text4 = TextFormat("Level next");
        DrawText(text4,
                 buttons[4].x + (buttons[4].width - MeasureText(text4, 20))/2,
                 buttons[4].y + 15,
                 20, GREEN);

        // Отрисовка фона радара
        DrawCircleV(radarPos, radarSize/2, Fade(DARKGRAY, 0.3f));
        DrawCircleLines(radarPos.x, radarPos.y, radarSize/2, DARKBROWN);

        // Анимированный сектор сканирования
        DrawCircleSector(
                radarPos,
                radarSize/2 - 5,
                rotationAngle - 10,
                rotationAngle + 10,
                32,
                Fade(GREEN, 0.3f)
        );

        // Вращающаяся линия радара
        DrawLineEx(
                radarPos,
                (Vector2){
                        radarPos.x + (radarSize/2 - 10) * cosf(rotationAngle * DEG2RAD),
                        radarPos.y + (radarSize/2 - 10) * sinf(rotationAngle * DEG2RAD)
                },
                3.0f,
                GREEN
        );

        // Статичные элементы радара
        DrawCircleLinesV(radarPos, radarSize/2 - 15, GREEN);
        DrawCircleV(radarPos, 5, GREEN);


        ClearBackground(DGRAY);
        // Отрисовка фона на весь экран
        DrawTexturePro(
                background,
                sourceRec,
                destRec,
                origin,
                0.0f,
                WHITE
        );
        DrawFPS(1000-78,10);





        //==================================================== Non disign disain plase for airplanes
//
//
//
////Взлетно посадочные полосы:
////верхняя полоса
//        DrawRectangle(145,115, 580, 30, BLACK);
//        DrawRectangle(145,145, 580, 30, GRAY);
//
////вторая сверху
//        DrawRectangle(145,115+100, 580, 30, BLACK);
//        DrawRectangle(145,145+100, 580, 30, GRAY);
//
////вторая снизу
//        DrawRectangle(175,115+100+70, 510, 20, BLACK);
//        DrawRectangle(175,135+100+70, 510, 20, GRAY);
//
////нижняя полоса
//        DrawRectangle(175,115+100+70+50, 510, 20, BLACK);
//        DrawRectangle(175,135+100+70+50, 510, 20, GRAY);
//
//
////дорожки между полосами
//
//        DrawRectangle(435,170,20,55,BLACK);
//
//        DrawRectangle(435,250,20,55,BLACK);
//
//        DrawRectangle(555,250,20,55,BLACK);
//
//        DrawRectangle(555,307,20,40,BLACK);
//
//        DrawRectangle(300,307,20,40,BLACK);
//
//        DrawRectangle(435,307,20,40,BLACK);
//
////парковка для самолетов
//
//        DrawRectangle(235,390,370,110,BBLACK);
//
////парковочные полосы на парковке
//
//        DrawRectangle(275,408,7,65,WHITE);
//
//        DrawRectangle(275+48,408,7,65,WHITE);
//
//        DrawRectangle(275+48*2,408,7,65,WHITE);
//
//        DrawRectangle(275+48*3,408,7,65,WHITE);
//
//        DrawRectangle(275+48*4,408,7,65,WHITE);
//
//        DrawRectangle(275+48*5,408,7,65,WHITE);
//
//        DrawRectangle(275+48*6,408,7,65,WHITE);


        Aircraft planes[numPlanes] = {
                Aircraft(planeTextures[0], {700, 300}, 0.07f, 1.0f, 0),
                Aircraft(planeTextures[1], {500, 600}, 0.07f, 0.8f, 1),
                Aircraft(planeTextures[2], {500, 600}, 0.06f, 1.2f, 2),
                Aircraft(planeTextures[3], {500, 600}, 0.0f, 1.5f, 3),
                Aircraft(planeTextures[4], {500, 600}, 0.03f, 2.0f, 4)
        };
        for (int i = 0; i < numPlanes; i++) {
            planes[i].Update();
            planes[i].Draw();
        }
        // В цикле отрисовки
        for (const auto& runway : runways) {
            DrawRectangleLinesEx(runway.area, 2, runway.isOccupied ? RED : GREEN);
        }

        for (const auto& spot : parkingSpots) {
            DrawRectangleLinesEx(spot.area, 2, spot.isOccupied ? RED : BLUE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}