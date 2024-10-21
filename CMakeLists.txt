cmake_minimum_required(VERSION 3.0)
project(Snake C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Snake
    consoleLib.c
    consoleLib.h
    consoleMenu.c
    consoleMenu.h
    gameField.c
    gameField.h
    helpMenu.c
    helpMenu.h
    main.c)

if(UNIX)
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})

    target_link_libraries(Snake ${CURSES_LIBRARIES})

    target_link_libraries(Snake m)
elseif(WIN32)
    # TODO windows
endif()
