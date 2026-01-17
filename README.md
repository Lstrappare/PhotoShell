# PhotoShell
Una herramienta CLI (Línea de Comandos) escrita en C para organizar fotos y videos automáticamente basándose en sus metadatos reales (EXIF).

<img width="461" height="198" alt="Screenshot 2026-01-17 at 1 12 55 a m" src="https://github.com/user-attachments/assets/798005b1-5bbf-4d7f-8dae-1d33992075f6" />

![C](https://img.shields.io/badge/Language-C-blue) ![License](https://img.shields.io/badge/License-Apache2.0-red)

## ✨ Características

* ✅ **Detecta fechas reales:** Lee metadatos EXIF de JPG, PNG, HEIC y QuickTime (MOV/MP4).
* ✅ **Shell Interactiva:** Incluye su propia mini-terminal para navegar carpetas (`ls`, `cd`).
* ✅ **Fallback Inteligente:** Si un archivo no tiene metadatos (descargado de internet), usa la fecha de modificación del archivo para no perderlo.
* ✅ **Renombrado Automático:** Formato `YYYY-MM-DD_HH-MM-SS.ext`.
* ✅ **Anti-Colisiones:** Evita sobrescribir archivos con el mismo nombre (agrega `_1`, `_2`, etc.).

## 🚀 Requisitos Previos

El programa utiliza **ExifTool** como motor de lectura de metadatos.

### macOS
1. Instala Homebrew si no lo tienes.
2. Ejecuta en tu terminal:
   ```bash
   brew install exiftool
   ```
### Windows

1. Descarga el ejecutable de ExifTool.

2. Descomprime el archivo y renombra exiftool(-k).exe a exiftool.exe.

3. Mueve exiftool.exe a la carpeta C:\Windows (para que sea accesible globalmente) O ponlo en la misma carpeta donde compiles este programa.

🛠️ Instalación y Compilación
1. Clonar el repositorio
git clone [https://github.com/Lstrappare/PhotoShell.git](https://github.com/Lstrappare/PhotoShell.git)

2. Compilar

Usando GCC (Mac/Linux/Windows con MinGW):
gcc main.c -o photoshell

### 📖 Cómo Usar
1. Ejecuta el programa:

```
./photoshell
```
2. Verás la terminal interactiva:
```
👤 photo-shell:[/Users/TuUsuario] $
```
3. Navega hasta tu carpeta de fotos desordenadas usando comandos:

- ls: Ver archivos.

- cd Carpeta: Entrar a una carpeta.

- cd ..: Regresar atrás.

4. Una vez dentro de la carpeta a ordenar, escribe:

```
organizar
```
   
5. El programa te pedirá la ruta de destino. Escríbela y pulsa Enter.

- Ejemplo destino: /Users/TuUsuario/Pictures/Ordenadas


### ⚠️ Notas
El programa crea copias o mueve archivos (según tu SO y configuración). Se recomienda probar primero con una carpeta de copia de seguridad.

Soporta: .jpg, .jpeg, .png, .heic, .mp4, .mov, .gif.








