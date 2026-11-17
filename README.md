# La Pulpe

Proyecto final de Programación I (EIF201, II ciclo 2026), Universidad Nacional, Campus Nicoya.

Aplicación de escritorio en C++ con Qt Widgets para administrar una pulpería: productos e inventario, ventas en efectivo con cálculo del cambio, historial de ventas y tickets.

## Equipo

| Integrante | Área |
|---|---|
| Integrante 1 (facilitador) | Productos e inventario |
| Integrante 2 | Ventas y cobro |
| Integrante 3 | Comprobantes e historial |

## Compilar y ejecutar

**Visual Studio 2022**
1. Tener instalado Qt 6 (MSVC 2022 64-bit) y la extensión Qt VS Tools.
2. *Archivo → Abrir → Carpeta…* y elegir esta carpeta. Visual Studio detecta el `CMakeLists.txt`.
3. Elegir `LaPulpe.exe` como elemento de inicio y ejecutar (F5).

**Terminal (Windows, Linux)**
```
cmake -S . -B build
cmake --build build
cd build
./LaPulpe
```

**Mac**
1. Instalar Qt 6 y CMake con Homebrew: `brew install qt cmake ninja`.
2. Compilar y abrir:
```
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=$(brew --prefix qt)
cmake --build build
cd build
./LaPulpe
```

## Al abrir la app

- Pide un **PIN de prueba: `1234`** (usuario Administrador o Cajero). No es una contraseña real y se puede apagar en Ajustes (el engrane arriba a la derecha).
- La primera vez trae 11 productos de ejemplo (`productos.json`), que CMake copia a la carpeta `build`.

## Dónde se guardan los datos

Todo se guarda en la carpeta donde se ejecuta el programa:

| Archivo | Qué guarda |
|---|---|
| `productos.json` | Los productos: código, nombre, precio, existencias y si está activo |
| `movimientos.json` | La actividad de cada producto (cambios de precio, mercadería que llegó, ventas) |
| `ventas.json` | Las ventas con sus líneas y el precio que se usó en cada una |
| `ticket-N.txt` / `ticket-N.csv` | El ticket de la venta N |

Si en esa carpeta todavía están los `productos.txt` y `ventas.txt` de versiones anteriores, la app los lee una vez y desde el siguiente cambio ya guarda en JSON. Los ajustes (tema, nombre de la tienda, aviso de pocas existencias) los guarda Qt con `QSettings`.

## Cómo trabajamos

- `main`: versiones entregadas. Solo recibe pull requests desde `development`.
- `development`: integración. Recibe pull requests desde las ramas `feature/…`.
- `feature/<tarea>`: una rama por tarea.

Ningún cambio llega a `main` ni a `development` sin un pull request aprobado por otro integrante.

Cada integrante agrega sus archivos al `CMakeLists.txt` **debajo de su comentario**, sin tocar las secciones de los demás.
