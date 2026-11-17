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
    Producto.cpp Producto.h
    Inventario.cpp Inventario.h
    ArchivoProductos.cpp ArchivoProductos.h
    ProductosForm.ui

    # Integrante 2: Ventas y cobro
    LineaVenta.cpp LineaVenta.h
    Carrito.cpp Carrito.h
    VentasForm.cpp VentasForm.h VentasForm.ui

    # Integrante 3: Comprobantes e historial
    Venta.cpp Venta.h
    Historial.cpp Historial.h
    Ticket.cpp Ticket.h
    TicketTxt.cpp TicketTxt.h
    TicketCsv.cpp TicketCsv.h
    HistorialForm.ui

)

target_link_libraries(LaPulpe PRIVATE Qt6::Widgets)

# Copia los productos de ejemplo a la carpeta donde se ejecuta el programa.
# Solo la primera vez: si ya existe, tiene los datos reales y no se debe pisar.
if(EXISTS ${CMAKE_SOURCE_DIR}/productos.txt AND NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/productos.txt)
    file(COPY productos.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
