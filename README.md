[[README]]
# Nombre del Proyecto

<p align="center">
    <pre>
 ██████╗ ██╗     ██╗████████╗ ██████╗██╗  ██╗██████╗  ██████╗ ██╗
██╔════╝ ██║     ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔═══██╗██║
██║  ███╗██║     ██║   ██║   ██║     ███████║██████╔╝██║   ██║██║
██║   ██║██║     ██║   ██║   ██║     ██╔══██║██╔══██╗██║   ██║██║
╚██████╔╝███████╗██║   ██║   ╚██████╗██║  ██║██████╔╝╚██████╔╝██║
 ╚═════╝ ╚══════╝╚═╝   ╚═╝    ╚═════╝╚═╝  ╚═╝╚═════╝  ╚═════╝ ╚═╝
  </pre>
</p>
<p align="center">
  <strong>Desarrollado por Glitchboi</strong><br>
  Seguridad desde México para todos
</p>

![Estado](https://img.shields.io/badge/status-En_desarollo-gree)
![License](https://img.shields.io/badge/license-GNU_AGPLv3-blue)

---

## Descripción

**PT6315VFD** es una biblioteca para **Arduino** que automatiza el control de un display **VFD basado en el chip PT6315** usando 4 pines (EN/CS/CLK/DIN) por *bit-banging*.  
Permite escribir en **8 dígitos 7-segmentos**, controlar **brillo**, **colon/indicadores**, manejar **iconos** (izq/der) y mostrar **texto con scroll** mediante `setText()` + `update()`.  
La idea es tener una API simple para reutilizar en proyectos DIY (reproductores, relojes, paneles, etc.) sin estar peleando con comandos y direcciones de RAM.

---

## Tabla de Contenidos

- [Instalación](#-instalación)
- [Uso](#-uso)
- [Contribuir](#-contribuir)
- [Créditos](#-créditos)

---

## Instalación

### Prerrequisitos
- Arduino IDE **o** PlatformIO
- Una placa compatible (UNO/Nano/ESP32/etc.)
- Display VFD con controlador **PT6315**

> Nota: el constructor permite indicar si `EN` es activo alto o activo bajo (`enActiveHigh`).
### Uso
#### Opción A — Arduino IDE (manual)
1. Crea una carpeta en tu sketchbook: `libraries/PT6315VFD/`
2. Copia dentro:
   - `PT6315VFD.h`
   - `PT6315VFD.cpp`
1. Reinicia Arduino IDE y ya podrás incluirla.
#### Opción B — PlatformIO
Coloca los archivos en `lib/PT6315VFD/` dentro de tu proyecto.

---
## Uso

La librería expone funciones simples para escribir caracteres en 7 segmentos, mostrar números, tiempo, iconos y texto con scroll.

### Ejemplo básico

```cpp
#include <Arduino.h>
#include "PT6315VFD.h"

// Ajusta pines a tu cableado:
PT6315VFD vfd(
  /*EN*/  2,
  /*CS*/  3,
  /*CLK*/ 4,
  /*DIN*/ 5,
  /*enActiveHigh*/ true
);

void setup() {
  vfd.begin(/*brightness 0..7*/ 7, /*displayOn*/ true);
  vfd.clear();

  // Escribir 8 caracteres (si es menos, rellena con espacios)
  vfd.setDigits8("HELLO   ");

  // Encender icono MP3 (lado izquierdo)
  vfd.showMP3(true);

  // Indicadores especiales
  vfd.setColon(true);        // colon (usa el bit SEG_X del dígito correspondiente)
  vfd.setOneIndicator(false);
}

void loop() {
  // Texto con scroll: si el texto > 8 chars, activa scroll interno
  vfd.setText("GLITCHBOI PT6315 VFD SCROLL DEMO  ", 200);

  // IMPORTANTE: update() debe llamarse seguido para animar el scroll
  while (true) {
    vfd.update();
    delay(5);
  }
}
```

### Funciones útiles (resumen rápido)

- **Inicialización / control**
    - `begin(brightness0to7, displayOn)`
    - `setDisplay(on, brightness0to7)`
    - `setBrightness(brightness0to7)`
    - `clear()`
- **Dígitos**
    - `setDigitPos(pos1to8, char)`
    - `setDigits8("TEXT....")`
    - `printLeft4(value, leadingZeros)`
    - `printTimeMMSS(mm, ss, colonOn)`
- **Indicadores**
    - `setColon(on)`
    - `setOneIndicator(on)`
- **Iconos**
    - `setIconLeft(ICONL_..., on)` / `setIconRight(ICONR_..., on)`
    - `setIconsLeftMask(mask)` / `setIconsRightMask(mask)`
    - `showMP3(on)`
- **Scroll**
    - `setText(text, scrollDelayMs)`
    - `update()` (llámala en el `loop`)
---

## Contribuir

Este proyecto no solo es un repositorio: es un espacio abierto para aprender, experimentar y construir juntos. **Buscamos activamente contribuciones**, ya sea en la parte técnica o incluso en la documentación.  
- **En software:** Desde corrección de bugs, optimización de rendimiento, hasta mejoras en la legibilidad del código o documentación; todo aporte, grande o pequeño, suma muchísimo.
No necesitas ser experto para ayudar: si crees que algo puede explicarse mejor, que el código puede ser más claro, o que hay una forma más elegante de hacer algo, **cuéntanos o abre un Pull Request**.

---

## Créditos
Creado por:
- [Erik Alcantara Covarrubias](https://www.linkedin.com/in/erik-alc%C3%A1ntara-covarrubias-29a97628a)