cmake_minimum_required(VERSION 3.17)
project(YourProjectName)

# Указываем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Используем pkg-config для Raylib
find_package(PkgConfig REQUIRED)
pkg_search_module(RAYLIB REQUIRED raylib)

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} main.cpp
        aircraft.cpp
        runway.h
        )

# Подключаем заголовки и библиотеки
target_include_directories(${PROJECT_NAME} PRIVATE ${RAYLIB_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARIES})

# Для macOS: фреймворки
if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "-framework IOKit"
            "-framework Cocoa"
            "-framework OpenGL"
            )
endif()