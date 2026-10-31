cmake_minimum_required(VERSION 3.16)
project(LaPulpe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Cada integrante agrega sus archivos debajo de su comentario
add_executable(LaPulpe
    main.cpp
    MainWindow.cpp MainWindow.h MainWindow.ui

    # Integrante 1: Productos e inventario

    # Integrante 2: Ventas y cobro

    # Integrante 3: Comprobantes e historial

)

target_link_libraries(LaPulpe PRIVATE Qt6::Widgets)

# Copia los productos de ejemplo a la carpeta donde se ejecuta el programa
if(EXISTS ${CMAKE_SOURCE_DIR}/productos.txt)
    file(COPY productos.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
