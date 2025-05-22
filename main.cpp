#include "raylib.h"
#include <cmath>

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

    while (!WindowShouldClose()) {

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

        EndDrawing();
    }

    CloseWindow();
    return 0;
}