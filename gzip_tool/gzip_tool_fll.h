/**
 * @file gzip_tool_fll.h
 * @author Mauricio David Correa Hernandez
 * @brief Declaración de la clase GzipTools para manejar compresión y descompresión de archivos usando gzip, incluyendo cifrado y descifrado de archivos.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <map>
#include <regex>
#include <zlib.h>
#include <cerrno>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <filesystem>
#include "../includes/utils.h"
#include "../encriptado-aes-256/encrypt_decrypt-con-clave.h"

#include <future>
#include <mutex>
#include <omp.h>

namespace fs = std::filesystem;
using namespace std;

/**
 * @brief Estructura para almacenar información sobre el proceso de descompresión y desencriptación de archivos.
 * 
 */
struct Decrypt_Decompress {
    fs::path path_directorio_completo;      /// Ruta completa del directorio donde se encuentra el archivo comprimido.
    fs::path path_directorio_nombre_base;   /// Ruta del directorio base donde se encuentra el archivo comprimido.
    bool exito;                             /// Indica si el proceso de descompresión y desencriptación fue exitoso.
};

/**
 * @class GzipTools
 * @brief Clase que proporciona herramientas para comprimir y descomprimir archivos usando gzip, incluyendo cifrado y descifrado de archivos.
 * 
 * Esta clase permite manejar la compresión y descompresión de archivos, así como la organización de directorios y archivos según extensiones.
 */
class GzipTools {
    public:
        /**
         * @brief Constructor por defecto de la clase GzipTools.
         * 
         */
        GzipTools();

        /**
         * @brief Verifica si una ruta es un directorio.
         * @param path Ruta a verificar.
         * @return true si es un directorio, false en caso contrario.
         */
        bool is_directory(const char* path);

        /**
         * @brief Comprime un archivo usando gzip.
         * @param input_filename Nombre del archivo de entrada.
         * @param output_filename Nombre del archivo de salida comprimido.
         * @return true si la compresión fue exitosa, false en caso contrario.
         */
        bool compress_file_gzip(const char* input_filename, const char* output_filename);

        /**
         * @brief Comprime archivos en un directorio agrupándolos por extensión.
         * @param dir Directorio que contiene los archivos a comprimir.
         * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
         * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
         */
        void compress_files_by_extension(const fs::path& dir, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt);

        /**
         * @brief Comprime un único archivo en un archivo tar y opcionalmente lo encripta.
         * @param filePath Ruta del archivo a comprimir.
         * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
         * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
         */
        void compress_single_file(const fs::path& filePath, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt);

        /**
         * @brief Procesa un directorio y sus archivos, comprimiéndolos por extensión.
         * Si encuentra subdirectorios, los procesa recursivamente.
         * @param node Nodo JSON que representa el directorio a procesar.
         * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
         * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
         */
        void process_directory(const json& node, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt);

        /**
         * @brief Procesa el directorio raíz y sus subdirectorios/archivos, comprimiéndolos por extensión.
         * @param root Nodo JSON que representa el directorio raíz a procesar.
         * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
         * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
         */
        void process_root_directory(const json& root, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt);

        /**
         * @brief Agrupa archivos intermedios por directorio y los empaqueta en archivos tar.gz.
         * Si se especifica la encriptación, los archivos tar se encriptan antes de comprimirlos.
         * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
         * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
         */
        void group_and_package_intermediate_tars(const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt);

        /**
         * @brief Crea un paquete final de backup, comprimiendo y encriptando si es necesario.
         * @param backupType Tipo de backup (por ejemplo, "completo", "incremental").
         * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
         * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
         * @param storage Tipo de almacenamiento (por ejemplo, "usb", "local", "nube").
         * @param pathStorage Ruta personalizada para almacenar el backup, si aplica.
         */
        void create_final_backup_package(const std::string& backupType, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt, 
                                        const std::string& storage, const std::string& pathStorage);

        /**
         * @brief Descomprime un archivo .gz a un archivo normal.
         * @param input_filename Nombre del archivo de entrada comprimido.
         * @param output_filename Nombre del archivo de salida descomprimido.
         * @return true si la descompresión fue exitosa, false en caso contrario.
         */
        bool decompress_file_gzip(const char* input_filename, const char* output_filename);

        /**
         * @brief Descomprime un archivo .gz a un archivo normal y lo desencripta si es necesario.
         * @param input_filename Nombre del archivo de entrada comprimido.
         * @param output_filename Nombre del archivo de salida descomprimido.
         * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
         * @return true si la descompresión y desencriptación fueron exitosas, false en caso contrario.
         */
        bool decompress_directory(const char* input_file, const char* output_dir);

        /**
         * @brief Descomprime un archivo .tar.gz a un directorio y lo desencripta si es necesario.
         * @param input_file Nombre del archivo de entrada comprimido.
         * @param output_dir Nombre del directorio de salida descomprimido.
         * @param master_key Clave maestra para la encriptación, si aplica.
         * @return Estructura Decrypt_Decompress con información sobre el proceso.
         */
        Decrypt_Decompress decompress_desencript_backup_file(const std::string& input_file, const std::string& output_dir, const std::string& master_key);
    
    private:
        /**
         * @brief Verifica si una cadena termina con un sufijo específico.
         * @param str Cadena a verificar.
         * @param suffix Sufijo a buscar.
         * @return true si la cadena termina con el sufijo, false en caso contrario.
         */
        bool ends_with(const std::string& str, const std::string& suffix);

        /**
         * @brief Mueve el contenido de un directorio a otro y elimina el directorio original.
         * @param src Ruta del directorio fuente.
         * @param dst Ruta del directorio destino.
         * @return true si la operación fue exitosa, false en caso contrario.
         */
        bool move_contents_and_remove_dir(const fs::path& src, const fs::path& dst);

        /**
         * @brief Reorganiza las carpetas de extensión en el directorio raíz.
         * @param root_dir Ruta del directorio raíz donde se encuentran las carpetas de extensión.
         */        
        void reorganize_extension_folders(const fs::path& root_dir);
};
