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

**Terminal**
```
cmake -S . -B build
cmake --build build
cd build
./LaPulpe
```

Los datos se guardan en `productos.txt` y `ventas.txt`, y los tickets como `ticket-N.txt` o `ticket-N.csv`, en la carpeta donde se ejecuta el programa.

## Cómo trabajamos

- `main`: versiones entregadas. Solo recibe pull requests desde `development`.
- `development`: integración. Recibe pull requests desde las ramas `feature/…`.
- `feature/<tarea>`: una rama por tarea.

Ningún cambio llega a `main` ni a `development` sin un pull request aprobado por otro integrante.

Cada integrante agrega sus archivos al `CMakeLists.txt` **debajo de su comentario**, sin tocar las secciones de los demás.
