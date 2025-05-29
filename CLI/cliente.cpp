/**
 * @file cliente.cpp
 * @author Mauricio David Correa Hernandez
 * @brief Implementación de la clase CLIApp para manejar un sistema de backup por línea de comandos.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "cliente.h"

/**
 * @brief Constructor por defecto de la clase CLIApp.
 * 
 */
CLIApp::CLIApp() {}

/**
 * @brief Verifica si el sistema de backup ya ha sido inicializado.
 * 
 * @return true 
 * @return false 
 */
bool CLIApp::isInitialized() {
    return fs::exists(init_flag);
}

/**
 * @brief Inicializa el sistema creando un archivo de bandera y directorio para respaldos en JSON.
 * 
 */
void CLIApp::initializeSystem() {
    std::ofstream flag(init_flag);
    flag << "Sistema de backup inicializado correctamente.\n";
    flag << "Puedes ahora usar comandos de backup o recuperación.\n\n";
    flag << "Ejemplo de uso:\n";
    flag << "./backupApp --modo backup \\\n";
    flag << "   --backupType incremental \\\n";
    flag << "   --flagResources 1 \\\n";
    flag << "   --numberResources 3 \\\n";
    flag << "   --nameResources ../../sistema_backup/ fotos/ videos/ \\\n";
    flag << "   --flagEncryptDecrypt 1 \\\n";
    flag << "   --keyEncryptDecrypt MiClave123 \\\n";
    flag << "   --storage local \\\n";
    flag << "   --encryptedCompressedFile backup_files/comprimido_backup_20250522_030829_completo_encriptado.tar.enc.gz \\\n";
    flag << "   --pathStorage /home/usuario/backups \\\n";
    flag.close();

    fs::create_directory(json_dir);
}

/**
 * @brief Reinicia el sistema eliminando el archivo de inicialización.
 * 
 */
void CLIApp::resetSystem() {
    if (fs::exists(init_flag)) {
        fs::remove(init_flag);
    }
}

/**
 * @brief Ejecuta los modos de operación iniciales como inicializar, reiniciar o verificar estado.
 * 
 * @param cliApp Referencia a una instancia de CLIApp.
 * @param init_mode Indica si se quiere inicializar el sistema.
 * @param reset_mode Indica si se quiere reiniciar el sistema.
 * @param status_mode Indica si se quiere verificar el estado del sistema.
 * @return 0 si la operación fue exitosa, 1 si se requiere inicializar el sistema, -1 si no se ejecutó ningún modo.
 */
int CLIApp::modos_iniciales(CLIApp& cliApp, bool init_mode, bool reset_mode, bool status_mode) {
    if (init_mode && cliApp.isInitialized()) {
        std::cout << GREEN << "✅  ✅  ✅  El sistema de backup ya está inicializado, no requiere volverse a inicializar. ✅  ✅  ✅" << RESET << std::endl;
        return 0;
    }
    else if (init_mode && !cliApp.isInitialized()) {
        cliApp.initializeSystem();
        std::cout << BLUE << "🟦 ℹ️  🟦 Sistema de backup inicializado correctamente. 🟦 ℹ️  🟦" << RESET << std::endl;
        return 0;
    }

    if (reset_mode) {
        cliApp.resetSystem();
        std::cout << YELLOW << "⚠️  ⚠️  ⚠️  Sistema reiniciado. Por favor, ejecuta './backupApp --init' nuevamente. ⚠️  ⚠️  ⚠️" << RESET << std::endl;
        return 0;
    }

    if (status_mode) {
        if (cliApp.isInitialized()) {
            std::cout << GREEN << "✅  ✅  ✅  Estado: el sistema de backup ya está inicializado. ✅  ✅  ✅" << RESET << std::endl;
        } else {
            std::cout << YELLOW << "⚠️  ⚠️  ⚠️  Estado: el sistema NO está inicializado. ⚠️  ⚠️  ⚠️" << RESET << std::endl;
        }
        return 0;
    }

    if (!cliApp.isInitialized()) {
        std::cerr << YELLOW << "⚠️  ⚠️  ⚠️  Debes ejecutar primero './backupApp --init'. ⚠️  ⚠️  ⚠️" << RESET << std::endl;
        return 1;
    }

    return -1;
}

/**
 * @brief Realiza validaciones previas antes de ejecutar comandos de backup o recuperación.
 * 
 * @param cliApp Referencia a una instancia de CLIApp.
 * @param mode Modo de operación del sistema (por ejemplo, "backup" o "recover").
 * @param backupType Tipo de backup a realizar (completo o incremental).
 * @param flagEncryptDecrypt Indicador si se desea cifrado (1 para sí, 0 para no).
 * @param keyEncryptDecrypt Clave usada para cifrado o descifrado.
 * @param numberResources Número de recursos esperados.
 * @param nameResources Lista de nombres de recursos proporcionados por el usuario.
 * @return 0 si las validaciones fueron exitosas, 1 si se detectó algún error, -1 si todo está correcto sin errores.
 */
int CLIApp::realizar_validaciones_previas(CLIApp& cliApp, const std::string& mode, const std::string& backupType, 
                                            int flagEncryptDecrypt, const std::string& keyEncryptDecrypt, 
                                            int numberResources, const std::vector<std::string>& nameResources) {
    if (nameResources.size() != static_cast<size_t>(numberResources)) {
        std::cerr << "Error: Se esperaban " << numberResources 
                    << " nombre(s), pero se recibieron " << nameResources.size() << ".\n";
        return 1;
    }

    if (flagEncryptDecrypt == 1) {
        if (keyEncryptDecrypt.empty()) {
            std::cerr << "Error: Debe ingresar una clave (--keyEncryptDecrypt).\n";
            return 1;
        }

        if (keyEncryptDecrypt.length() != 32) {
            std::cerr << "Error: La clave debe tener exactamente 32 caracteres, pero tiene " 
                        << keyEncryptDecrypt.length() << " caracteres.\n";
            return 1;
        }
    }

    return -1;
}
