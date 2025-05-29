/**
 * @file utils.cpp
 * @author Mauricio David Correa Hernandez
 * @brief Implementación de la clase Utils para manejar utilidades generales como manejo de tiempo, directorios y ejecución de scripts Python.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "utils.h"

/**
 * @brief Constructor por defecto de la clase Utils.
 * 
 */
Utils::Utils(){}

/**
 * @brief Convierte el tiempo actual del sistema a una cadena formateada.
 * @return Cadena con la fecha y hora actual en formato YYYYMMDD_HHMMSS.
 */
std::string Utils::time_to_string_hora_actual() {
    auto ahora = std::chrono::system_clock::now();
    std::time_t tiempo_actual = std::chrono::system_clock::to_time_t(ahora);
    std::tm tm = *std::localtime(&tiempo_actual);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

/**
 * @brief Prepara el directorio de salida eliminando el existente y creando uno nuevo.
 * 
 */
void Utils::prepare_output_directory() {
    if (fs::exists(OUTPUT_DIR)) {
        fs::remove_all(OUTPUT_DIR);
    }
    fs::create_directories(OUTPUT_DIR);
}

/**
 * @brief Ejecuta un script de Python y captura su salida.
 * @param cmd Comando del script de Python a ejecutar.
 * @return Cadena con la salida del script.
 * @throws std::runtime_error Si hay un error al ejecutar el script.
 */
std::string Utils::exec_python_script(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);

    if (!pipe) throw std::runtime_error("Error al ejecutar Python.");
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}