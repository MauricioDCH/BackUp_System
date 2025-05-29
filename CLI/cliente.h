/**
 * @file cliente.h
 * @author Mauricio David Correa Hernandez
 * @brief Declaración de la clase CLIApp para manejar un sistema de backup por línea de comandos.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "CLI/CLI.hpp"
#include "../includes/utils.h"

namespace fs = std::filesystem;

const std::string init_flag = ".init_backup_flag";  /// Archivo de bandera que indica si el sistema de backup ha sido inicializado.
const std::string json_dir = ".json_backups_files"; /// Directorio donde se almacenarán los archivos JSON de respaldo.

/**
 * @class CLIApp
 * @brief Clase que gestiona la inicialización, reinicio, estado y validaciones previas de un sistema de backup a través de CLI.
 */
class CLIApp {
    public:
        /**
         * @brief Constructor por defecto de la clase CLIApp.
         */
        CLIApp();

        /**
         * @brief Verifica si el sistema de backup ya ha sido inicializado.
         * @return true si el sistema está inicializado, false en caso contrario.
         */
        bool isInitialized();

        /**
         * @brief Inicializa el sistema creando un archivo de bandera y directorio para respaldos en JSON.
         */
        void initializeSystem();

        /**
         * @brief Reinicia el sistema eliminando el archivo de inicialización.
         */
        void resetSystem();

        /**
         * @brief Ejecuta los modos de operación iniciales como inicializar, reiniciar o verificar estado.
         * @param cliApp Referencia a una instancia de CLIApp.
         * @param init_mode Indica si se quiere inicializar el sistema.
         * @param reset_mode Indica si se quiere reiniciar el sistema.
         * @param status_mode Indica si se quiere verificar el estado del sistema.
         * @return 0 si la operación fue exitosa, 1 si se requiere inicializar el sistema, -1 si no se ejecutó ningún modo.
         */
        int modos_iniciales(CLIApp& cliApp, bool init_mode, bool reset_mode, bool status_mode);

        /**
         * @brief Realiza validaciones previas antes de ejecutar comandos de backup o recuperación.
         * @param cliApp Referencia a una instancia de CLIApp.
         * @param mode Modo de operación del sistema (por ejemplo, "backup" o "recover").
         * @param backupType Tipo de backup a realizar (completo o incremental).
         * @param flagEncryptDecrypt Indicador si se desea cifrado (1 para sí, 0 para no).
         * @param keyEncryptDecrypt Clave usada para cifrado o descifrado.
         * @param numberResources Número de recursos esperados.
         * @param nameResources Lista de nombres de recursos proporcionados por el usuario.
         * @return 0 si las validaciones fueron exitosas, 1 si se detectó algún error, -1 si todo está correcto sin errores.
         */
        int realizar_validaciones_previas(CLIApp& cliApp, const std::string& mode, const std::string& backupType, 
                                            int flagEncryptDecrypt, const std::string& keyEncryptDecrypt,
                                            int numberResources, const std::vector<std::string>& nameResources);
};
