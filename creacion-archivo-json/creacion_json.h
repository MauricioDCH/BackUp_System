/**
 * @file creacion_json.h
 * @author Mauricio David Correa Hernandez
 * @brief Declaración de la clase GzipTools para manejar compresión y descompresión de archivos usando gzip, incluyendo cifrado y descifrado de archivos.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <iostream>
#include <filesystem>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include "../includes/utils.h"
#include <optional>
#include <algorithm>

#include <future> // Para std::async
#include <execution> // C++17


using namespace std;


/**
 * @struct BackupInfo
 * @brief Estructura que almacena información sobre un archivo de backup.
 */
struct BackupInfo {
    fs::path nombre_archivo;    /// Nombre del archivo completo
    std::string tipo;           /// Tipo de backup: completo, diferencial o incremental
    std::string fecha_hora;     /// Fecha y hora en formato YYYYMMDD_HHMMSS
    std::string encriptado;     /// encriptado o noEncriptado
};

/**
 * @class CreacionArchivoJson
 * @brief Clase para crear y manejar archivos JSON de backups, incluyendo la creación de nodos JSON para archivos y directorios.
 */
class CreacionArchivoJson {
    public:
        /**
         * @brief Constructor por defecto de la clase CreacionArchivoJson.
         * 
         */
        CreacionArchivoJson();

        /**
         * @brief Convierte el tiempo de archivo a una cadena con formato legible.
         * @param ft Tiempo de archivo a convertir.
         * @return Cadena con la fecha y hora formateada.
         */
        std::string time_to_string_files(fs::file_time_type ft);

        /**
         * @brief Procesa un solo path (archivo o carpeta) y genera un nodo JSON.
         * @param path Ruta del archivo o carpeta a procesar.
         * @return Nodo JSON con la información del archivo o carpeta.
         */
        json process_path(const fs::path& path);

        /**
         * @brief Procesa múltiples paths y los agrega al JSON de salida.
         * @param paths Vector de rutas a procesar.
         * @return Nodo JSON con la lista de archivos y carpetas procesados.
         */
        json process_multiple_paths(const std::vector<fs::path>& paths);
        
        /**
         * @brief Crea un archivo JSON con la estructura de archivos y carpetas de una ruta dada.
         * @param ruta Ruta del directorio a procesar.
         * @param backupType Tipo de backup: completo, diferencial o incremental.
         * @param estaEncriptado Indica si el backup está encriptado o no.
         * @return Ruta del archivo JSON creado.
         */
        fs::path guardar_respaldo_json(const json& resultado,const std::string& backupType,bool estaEncriptado);
        
        /**
         * @brief Carga la estructura JSON desde un archivo y valida que contenga el array "backup".
         * @param filePath Ruta del archivo JSON a cargar.
         * @param outJson Referencia al objeto JSON donde se guardará la estructura cargada.
         * @return true si se cargó correctamente, false en caso contrario.
         */
        bool load_json_structure(const std::string& filePath, json& outJson);

        /**
         * @brief Obtiene información de un archivo y la devuelve como un objeto JSON.
         * @param path Ruta del archivo a procesar.
         * @return Nodo JSON con la información del archivo, o nullptr si no es un archivo regular.
         */
        json get_file_info(const fs::path& path);

        /**
         * @brief Obtiene información de un directorio y sus archivos, comparando con un JSON previo para detectar cambios.
         * @param dir_path Ruta del directorio a procesar.
         * @param prev_dir_json JSON previo del directorio para comparar.
         * @param archivos_modificados Vector donde se guardarán las rutas de archivos modificados.
         * @return Nodo JSON con la información del directorio y sus archivos modificados, o nullptr si no hay cambios.
         */
        json get_directory_info(const fs::path& dir_path, const json& prev_dir_json, std::vector<std::string>& archivos_modificados);

        /**
         * @brief Obtiene todos los backups ordenados por fecha y hora.
         * @param ruta Ruta del directorio donde se encuentran los backups.
         * @return Vector de BackupInfo con la información de los backups encontrados.
         */
        std::vector<BackupInfo> obtener_backups_todos_ordenados(const std::string& ruta);

        /**
         * @brief Obtiene los backups ordenados por tipo específico (completo, diferencial, incremental).
         * @param ruta Ruta del directorio donde se encuentran los backups.
         * @param tipo_filtro Tipo de backup a filtrar (completo, diferencial, incremental).
         * @return Vector de BackupInfo con la información de los backups filtrados.
         */
        std::vector<BackupInfo> obtener_backups_ordenados_por_tipo(const std::string& ruta, const std::string& tipo_filtro);

        /**
         * @brief Procesa el archivo backup.json y genera un JSON con solo los archivos modificados.
         * @param path_input Ruta del archivo backup.json a procesar.
         * @return Nodo JSON con la información de los archivos modificados.
         */
        json procesar_backup_diferencial(const fs::path& path_input);
        
        /**
         * @brief Obtiene el último backup general (completo, diferencial o incremental) ordenado por fecha.
         * @param ruta Ruta del directorio donde se encuentran los backups.
         * @return BackupInfo del último backup encontrado, o std::nullopt si no hay backups.
         */
        std::optional<BackupInfo> obtener_ultimo_backup_general(const std::string& ruta);

        /**
         * @brief Obtiene el último backup válido para un backup incremental, buscando el más reciente completo o incremental.
         * @param ruta Ruta del directorio donde se encuentran los backups.
         * @return BackupInfo del último backup válido encontrado, o std::nullopt si no hay backups válidos.
         */
        std::optional<BackupInfo> obtener_ultimo_valido_para_incremental(const std::string& ruta);

        /**
         * @brief Procesa el archivo backup.json y genera un JSON con solo los archivos modificados.
         * Este método es específico para backups incrementales, comparando el estado actual del sistema de archivos
         * con el backup anterior para identificar cambios.
         * @param path_input Ruta del archivo backup.json a procesar.
         * @return Nodo JSON con la información de los archivos modificados.
         */
        json procesar_backup_incremental(const fs::path& path_input);

        /**
         * @brief Copia un directorio de forma recursiva a otro directorio.
         * @param from Ruta del directorio de origen.
         * @param to Ruta del directorio de destino.
         * @return true si la copia fue exitosa, false en caso contrario.
         */
        bool copy_directory_recursive(const fs::path& from, const fs::path& to);
};
