#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h> // Para getcwd, chdir y usleep

// --- DEFINICIÓN DE COLORES Y ESTILOS ---
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

// Variables globales
char RUTA_DESTINO_BASE[1024];

// ==========================================
//    PARTE 0: ANIMACIONES Y EFECTOS
// ==========================================

// Función para limpiar pantalla (Compatible Linux/Mac/Windows moderno)
void limpiar_pantalla() {
    printf("\033[H\033[J");
}

// Pausa en milisegundos
void esperar(int ms) {
    usleep(ms * 1000);
}

// Efecto de escribir letra por letra
void escribir_lento(const char *texto, int velocidad_ms) {
    for (int i = 0; texto[i] != '\0'; i++) {
        printf("%c", texto[i]);
        fflush(stdout); // Fuerza a imprimir inmediatamente
        esperar(velocidad_ms);
    }
}

// Animación de barra de carga
void animacion_carga() {
    printf(YELLOW "\n   [SISTEMA] Inicializando PhotoShell...\n" RESET);
    printf("   Cargando: [");
    for (int i = 0; i <= 40; i++) {
        printf(GREEN "#");
        fflush(stdout);
        esperar(30); // Velocidad de la barra
    }
    printf(RESET "] 100%%\n");
    esperar(500);
}

void mostrar_logo() {
    limpiar_pantalla();
    printf(CYAN BOLD);
    printf("  ____  _           _        ____  _          _ _ \n");
    printf(" |  _ \\| |__   ___ | |_ ___ / ___|| |__   ___| | |\n");
    printf(" | |_) | '_ \\ / _ \\| __/ _ \\\\___ \\| '_ \\ / _ \\ | |\n");
    printf(" |  __/| | | | (_) | || (_) |___) | | | |  __/ | |\n");
    printf(" |_|   |_| |_|\\___/ \\__\\___/|____/|_| |_|\\___|_|_|\n");
    printf(RESET);
    printf(MAGENTA "      v1.0  |  Organizador Inteligente (EXIF)\n" RESET);
    printf("  --------------------------------------------------\n\n");
}

// ==========================================
//    PARTE 1: LÓGICA DE ORGANIZACIÓN (CORE)
// ==========================================

void limpiar_string(char *s) {
    char *pos;
    if ((pos = strchr(s, '\n')) != NULL) *pos = '\0';
}

const char* obtener_extension(const char* nombre) {
    const char* punto = strrchr(nombre, '.');
    if (!punto || punto == nombre) return NULL;
    return punto + 1;
}

void buscar_fecha_en_string(const char* raw_output, char* fecha_destino) {
    int len = strlen(raw_output);
    for (int i = 0; i < len - 10; i++) {
        if ((strncmp(&raw_output[i], "20", 2) == 0 || strncmp(&raw_output[i], "19", 2) == 0) &&
            isdigit(raw_output[i+2]) && isdigit(raw_output[i+3]) && 
            raw_output[i+4] == '-') {
            strncpy(fecha_destino, &raw_output[i], 19);
            fecha_destino[19] = '\0';
            return;
        }
    }
    strcpy(fecha_destino, "DESCONOCIDO");
}

void obtener_fecha_real(const char* ruta, char* fecha_destino) {
    char comando[2048];
    char resultado[512];

    snprintf(comando, sizeof(comando), 
        "exiftool -T -DateTimeOriginal -CreateDate -MediaCreateDate -d \"%%Y-%%m-%%d_%%H-%%M-%%S\" \"%s\" 2>/dev/null", 
        ruta);

    FILE *fp = popen(comando, "r");
    if (fp == NULL) {
        strcpy(fecha_destino, "ERROR");
        return;
    }

    if (fgets(resultado, sizeof(resultado), fp) != NULL) {
        buscar_fecha_en_string(resultado, fecha_destino);
    } else {
        strcpy(fecha_destino, "DESCONOCIDO");
    }
    pclose(fp);
}

void mover_y_renombrar(const char* ruta_origen, const char* ext_original, const char* fecha_str) {
    char carpeta_anio[2048];
    char carpeta_mes[2048];
    char nombre_final[512];
    char ruta_destino[2048];
    char anio[5] = {0};
    char mes[3] = {0};

    strncpy(anio, fecha_str, 4);
    strncpy(mes, fecha_str + 5, 2);

    snprintf(carpeta_anio, sizeof(carpeta_anio), "%s/%s", RUTA_DESTINO_BASE, anio);
    mkdir(carpeta_anio, 0755);

    snprintf(carpeta_mes, sizeof(carpeta_mes), "%s/%s", carpeta_anio, mes);
    mkdir(carpeta_mes, 0755);

    int contador = 0;
    while (1) {
        if (contador == 0) 
            snprintf(nombre_final, sizeof(nombre_final), "%s.%s", fecha_str, ext_original);
        else 
            snprintf(nombre_final, sizeof(nombre_final), "%s_%d.%s", fecha_str, contador, ext_original);

        snprintf(ruta_destino, sizeof(ruta_destino), "%s/%s", carpeta_mes, nombre_final);

        if (access(ruta_destino, F_OK) == 0) contador++; 
        else break; 
    }

    if (rename(ruta_origen, ruta_destino) == 0) {
        printf(GREEN "   ✅ %s\n" RESET, nombre_final);
    } else {
        printf(RED "   ❌ Error: %s\n" RESET, strerror(errno));
    }
    // Pequeña pausa para que se vea el trabajo (efecto visual)
    esperar(50); 
}

void procesar_archivo(const char* ruta_base, const char* nombre, struct stat info) {
    char ruta_completa[2048];
    snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", ruta_base, nombre);

    const char* ext = obtener_extension(nombre);
    if (!ext) return;

    int es_multimedia = 0;
    if (strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0 || 
        strcasecmp(ext, "png") == 0 || strcasecmp(ext, "heic") == 0 || 
        strcasecmp(ext, "mp4") == 0 || strcasecmp(ext, "mov") == 0 || 
        strcasecmp(ext, "gif") == 0) es_multimedia = 1;

    if (es_multimedia) {
        char fecha_final[100];
        obtener_fecha_real(ruta_completa, fecha_final);

        if (strcmp(fecha_final, "DESCONOCIDO") == 0) {
            struct tm *t = localtime(&info.st_mtime);
            strftime(fecha_final, sizeof(fecha_final), "%Y-%m-%d_%H-%M-%S", t);
        }
        mover_y_renombrar(ruta_completa, ext, fecha_final);
    }
}

void escanear_directorio(const char *ruta) {
    DIR *d = opendir(ruta);
    struct dirent *dir;
    struct stat info;
    char sub_ruta[2048];

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] == '.') continue;

            snprintf(sub_ruta, sizeof(sub_ruta), "%s/%s", ruta, dir->d_name);
            if (strstr(sub_ruta, RUTA_DESTINO_BASE) != NULL) continue;

            if (stat(sub_ruta, &info) == 0) {
                if (S_ISDIR(info.st_mode)) {
                    escanear_directorio(sub_ruta);
                } else if (S_ISREG(info.st_mode)) {
                    procesar_archivo(ruta, dir->d_name, info);
                }
            }
        }
        closedir(d);
    }
}

// ==========================================
//    PARTE 2: MINI TERMINAL (SHELL)
// ==========================================

void cmd_ls() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return;

    DIR *d = opendir(cwd);
    struct dirent *dir;
    if (d) {
        printf("\n   " YELLOW "CONTENIDO DE: %s" RESET "\n", cwd);
        printf("   ----------------------------------------\n");
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] == '.') continue;
            if (dir->d_type == DT_DIR) printf(BLUE "   📂 %s/\n" RESET, dir->d_name);
            else printf("   📄 %s\n", dir->d_name);
        }
        printf("   ----------------------------------------\n");
        closedir(d);
    }
}

void cmd_cd(char* path) {
    if (path == NULL || strlen(path) == 0) return;
    limpiar_string(path);
    if (chdir(path) != 0) {
        printf(RED "❌ No se pudo entrar a: %s\n" RESET, path);
    }
}

void cmd_organizar() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    printf(YELLOW BOLD "\n🚀 MODO ORGANIZADOR ACTIVADO\n" RESET);
    printf("📂 Origen: %s\n", cwd);
    printf("🎯 Destino (Arrastra carpeta o escribe ruta): ");

    if (fgets(RUTA_DESTINO_BASE, sizeof(RUTA_DESTINO_BASE), stdin)) {
        limpiar_string(RUTA_DESTINO_BASE);
    }

    if (strlen(RUTA_DESTINO_BASE) < 2) {
        printf(RED "❌ Ruta destino inválida.\n" RESET);
        return;
    }

    mkdir(RUTA_DESTINO_BASE, 0755);

    printf("\n");
    escribir_lento(CYAN "--- 🔎 Analizando y Moviendo Archivos ---" RESET, 30);
    printf("\n");

    escanear_directorio(cwd);

    printf(GREEN BOLD "\n✅ ¡Organización terminada con éxito!\n" RESET);
}

void imprimir_ayuda() {
    printf("\n   " BOLD "COMANDOS DISPONIBLES:" RESET "\n");
    printf(GREEN "   ls" RESET "          -> Listar archivos en carpeta actual\n");
    printf(GREEN "   cd <ruta>" RESET "   -> Cambiar de carpeta (Ej: cd Fotos)\n");
    printf(GREEN "   pwd" RESET "         -> Ver ruta actual\n");
    printf(YELLOW "   organizar" RESET "   -> Iniciar proceso de ordenado AQUÍ\n");
    printf(RED "   exit" RESET "        -> Salir\n");
}

int main(void) {
    char input[1024];
    char cwd[1024];
    char *comando;
    char *argumento;

    animacion_carga(); // 1. Barra de carga
    mostrar_logo();    // 2. Logo ASCII Grande

    if (system("exiftool -ver > /dev/null 2>&1") != 0) {
        printf(RED BOLD "⚠️  ADVERTENCIA: 'exiftool' no detectado. Instálalo para mejor precisión.\n" RESET);
    }

    escribir_lento("Bienvenido al sistema. Escribe 'help' para ver comandos.\n", 20);

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf(CYAN "\nphoto-shell" RESET ":" BLUE "[%s]" RESET " $ ", cwd);
        } else {
            printf("\n👤 photo-shell:[?] $ ");
        }

        if (fgets(input, sizeof(input), stdin) == NULL) break;
        limpiar_string(input);
        if (strlen(input) == 0) continue;

        comando = strtok(input, " ");
        argumento = strtok(NULL, ""); 

        if (comando == NULL) continue;

        if (strcmp(comando, "exit") == 0) {
            escribir_lento(MAGENTA "Cerrando sesión... Hasta pronto! 👋\n" RESET, 30);
            break;
        } else if (strcmp(comando, "ls") == 0) {
            cmd_ls();
        } else if (strcmp(comando, "pwd") == 0) {
            printf("%s\n", cwd);
        } else if (strcmp(comando, "cd") == 0) {
            cmd_cd(argumento);
        } else if (strcmp(comando, "organizar") == 0) {
            cmd_organizar();
        } else if (strcmp(comando, "help") == 0) {
            imprimir_ayuda();
        } else {
            printf("Comando no reconocido: %s. Escribe 'help' para ayuda.\n", comando);
        }
    }

    return 0;
}
