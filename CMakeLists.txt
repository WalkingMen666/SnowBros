cmake_minimum_required(VERSION 3.16)
project(SnowBros)

set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    ptsd

    URL         https://github.com/ntut-open-source-club/practical-tools-for-simple-design/archive/refs/tags/v0.3-rc1.zip

    # Or alternatively if you want to use a specific git commit instead of a release:
    # GIT_REPOSITORY  https://github.com/ntut-open-source-club/practical-tools-for-simple-design.git
    # GIT_TAG         <git_tag>

    SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/PTSD
)

FetchContent_MakeAvailable(ptsd)

# 定義目錄路徑
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 收集所有源文件
set(SRC_FILES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/AppStart.cpp
    ${SRC_DIR}/AppUpdate.cpp
    ${SRC_DIR}/AppEnd.cpp
    ${SRC_DIR}/AnimatedCharacter.cpp
    ${SRC_DIR}/Bullet.cpp
    ${SRC_DIR}/Boss.cpp
    ${SRC_DIR}/Boss2.cpp
    ${SRC_DIR}/Character.cpp
    ${SRC_DIR}/Enemy.cpp
    ${SRC_DIR}/Fat.cpp
    ${SRC_DIR}/FatBullet.cpp
    ${SRC_DIR}/Frog.cpp
    ${SRC_DIR}/FrogBullet.cpp
    ${SRC_DIR}/GameWorld.cpp
    ${SRC_DIR}/Map.cpp
    ${SRC_DIR}/Monkey.cpp
    ${SRC_DIR}/Nick.cpp
    ${SRC_DIR}/PhaseManager.cpp
    ${SRC_DIR}/RedDemon.cpp
    ${SRC_DIR}/ScoreItem.cpp
    ${SRC_DIR}/SmallBoss.cpp
    ${SRC_DIR}/SmallBoss2.cpp
    ${SRC_DIR}/Snowball.cpp
    ${SRC_DIR}/NickBullet.cpp
)

# 收集所有頭文件
set(HEADER_FILES
    ${INCLUDE_DIR}/AnimatedCharacter.hpp
    ${INCLUDE_DIR}/App.hpp
    ${INCLUDE_DIR}/BackgroundImage.hpp
    ${INCLUDE_DIR}/Boss.hpp
    ${INCLUDE_DIR}/Boss2.hpp
    ${INCLUDE_DIR}/Bullet.hpp
    ${INCLUDE_DIR}/Character.hpp
    ${INCLUDE_DIR}/Enemy.hpp
    ${INCLUDE_DIR}/Fat.hpp
    ${INCLUDE_DIR}/FatBullet.hpp
    ${INCLUDE_DIR}/Frog.hpp
    ${INCLUDE_DIR}/FrogBullet.hpp
    ${INCLUDE_DIR}/GameWorld.hpp
    ${INCLUDE_DIR}/Map.hpp
    ${INCLUDE_DIR}/Monkey.hpp
    ${INCLUDE_DIR}/Nick.hpp
    ${INCLUDE_DIR}/PhaseResourceManger.hpp
    ${INCLUDE_DIR}/RedDemon.hpp
    ${INCLUDE_DIR}/ScoreItem.hpp
    ${INCLUDE_DIR}/SmallBoss.hpp
    ${INCLUDE_DIR}/SmallBoss2.hpp
    ${INCLUDE_DIR}/Snowball.hpp
    ${INCLUDE_DIR}/UpdatableDrawable.hpp
)

# 添加可執行文件
add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${HEADER_FILES}
)

# 添加編譯選項
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# 設置資源目錄
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Resources")
else()
    message(FATAL_ERROR "relative RESOURCE_DIR is WIP, Please use `-DCMAKE_BUILD_TYPE=Debug` build for now.")
    # target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_INSTALL_PREFIX}/Resources")
endif()

# 設置include目錄
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${DEPENDENCY_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/PTSD/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 鏈接庫
target_link_libraries(${PROJECT_NAME}
    SDL2::SDL2main
    PTSD
)
