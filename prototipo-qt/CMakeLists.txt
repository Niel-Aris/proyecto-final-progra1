cmake_minimum_required(VERSION 3.16)
project(PrototipoLaPulpe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(PrototipoLaPulpe
    main.cpp

    # Integrante 1
    VentanaPrototipo.cpp VentanaPrototipo.h
    PantallaProductos.cpp PantallaProductos.h

    # Integrante 2
    PantallaVender.cpp PantallaVender.h

    # Integrante 3
    PantallaHistorial.cpp PantallaHistorial.h
)

target_link_libraries(PrototipoLaPulpe PRIVATE Qt6::Widgets)

# Para que el símbolo de colones se vea bien en Visual Studio
if(MSVC)
    target_compile_options(PrototipoLaPulpe PRIVATE /utf-8)
endif()
