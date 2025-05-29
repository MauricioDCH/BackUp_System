/**
 * @file utils.h
 * @author Mauricio David Correa Hernandez
 * @brief Declaración de la clase Utils para manejar utilidades generales como manejo de tiempo, directorios y ejecución de scripts Python.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstdio>
#include <memory>
#include <array>

// Define BUFFER_SIZE para el tamaño del búfer de lectura/escritura
#define BUFFER_SIZE 4096

// Define colores para la salida en consola
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"

namespace fs = std::filesystem;
using json = nlohmann::json;

// Define rutas y nombres de archivos utilizados en el sistema de backup
static const fs::path JSONS_DIR = ".json_backups_files";
static const fs::path OUTPUT_DIR = ".temp_backup";
static const fs::path TEMP_GZ_DIR = ".temp_gzip";
static const fs::path OUTPUT_DIR_ENCRYPT = ".temp_backup_en";
static const fs::path TARGET_PATH = "backup_files";
static const fs::path RECOVER_FILES = "recovered_files";

/**
 * @class Utils
 * @brief Clase que proporciona utilidades generales como manejo de tiempo, directorios y ejecución de scripts Python.
 */
class Utils {
    public:
        /**
         * @brief Constructor por defecto de la clase Utils.
         */
        Utils();

        /**
         * @brief Convierte el tiempo actual del sistema a una cadena formateada.
         * @return Cadena con la fecha y hora actual en formato YYYYMMDD_HHMMSS.
         */
        std::string time_to_string_hora_actual();

        /**
         * @brief Prepara el directorio de salida eliminando el existente y creando uno nuevo.
         */
        void prepare_output_directory();

        /**
         * @brief Ejecuta un script de Python y captura su salida.
         * @param cmd Comando del script de Python a ejecutar.
         * @return Cadena con la salida del script.
         * @throws std::runtime_error Si hay un error al ejecutar el script.
         */
        std::string exec_python_script(const char* cmd);
};
