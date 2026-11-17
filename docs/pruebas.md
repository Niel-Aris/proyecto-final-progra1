# Tabla de pruebas

Los 10 casos de la tabla de pruebas (sección 3.2 del informe). La evidencia de cada caso es un comentario con su captura en el issue [#36](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36).

**Resultado: los 10 casos pasan.** Se probaron el 17/11/2026 sobre `development` con las fotos nuevas de los productos (#58), y se volvieron a correr después del cambio a JSON (#59).

**Antes de empezar:** borrar `productos.json`, `movimientos.json`, `ventas.json` y los `ticket-*` de la carpeta donde corre el programa (`build`) y volver a configurar con CMake, para empezar con los productos de ejemplo (Arroz 24, Leche 30, Huevos 2, Café activo) y que la venta sea la número 1.

| # | Caso | Resultado esperado | Resultado obtenido | Responsable | Evidencia |
|---|---|---|---|---|---|
| 1 | Venta completa: Arroz × 2 y Leche × 1, paga ₡5000 | Total ₡3475, cambio ₡1525, se guarda el ticket | Total ₡3 475, vuelto ₡1 525 y se crea `ticket-1.txt` | Integrante 2 | [Caso 1](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960753185) |
| 2 | Inventario después de la venta 1 | Arroz baja de 24 a 22 y Leche de 30 a 29 | Arroz 22 y Leche 29 | Integrante 1 | [Caso 2](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960753740) |
| 3 | Pago insuficiente: carrito de ₡3475, paga ₡1000 | Mensaje de pago insuficiente y no se registra la venta | Cobrar apagado, "Faltan ₡2 475" y "El pago no alcanza: faltan ₡2 475."; no se guarda | Integrante 2 | [Caso 3](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960754316) |
| 4 | Sin existencias: Huevos (15) × 3 | Mensaje de existencias insuficientes (solo hay 2) | La tarjeta queda en "Sin existencias" y el + dice "Solo hay 2 de Huevos (15)." | Integrante 2 | [Caso 4](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960754808) |
| 5 | Producto con precio 0 | "El precio debe ser mayor que cero." | Mismo mensaje y no se crea | Integrante 1 | [Caso 5](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960755360) |
| 6 | Producto con código repetido (P001) | "Ya existe un producto con ese código." | Mismo mensaje y no se crea | Integrante 1 | [Caso 6](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960755971) |
| 7 | Desactivar Café 250 g | Queda Inactivo y no aparece en Vender | Inactivo y no sale en Vender | Integrante 1 | [Caso 7](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960756485) |
| 8 | Cambiar el precio del arroz después de la venta 1 | La venta 1 sigue con el precio viejo (₡1250) | Arroz a ₡1 400; la venta 1 sigue con 2 × ₡1 250 | Integrante 3 | [Caso 8](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960756915) |
| 9 | Cerrar y volver a abrir la aplicación | Productos y ventas siguen ahí | Siguen los 11 productos, la venta y los cambios | Integrante 3 | [Caso 9](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960757430) |
| 10 | Historial → venta 1 → formato csv → Generar ticket | Se crea `ticket-1.csv` | Se crea `ticket-1.csv` con las líneas, el total, el pago y el cambio | Integrante 3 | [Caso 10](https://github.com/Niel-Aris/proyecto-final-progra1/issues/36#issuecomment-6960757946) |

Resumen en el issue [#25](https://github.com/Niel-Aris/proyecto-final-progra1/issues/25).
