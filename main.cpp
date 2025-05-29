
#include <iostream>
#include "./CLI/cliente.h"
#include "./includes/utils.h"
#include "./gzip_tool/gzip_tool_fll.h"
#include "./creacion-archivo-json/creacion_json.h"

/**
 * @brief Punto de entrada principal del programa.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char** argv) {
    CLI::App app{"🗃️  Sistema de Backup con CLI11"};

    //-------------------------------------------------------------------------------------------------------------------------
    // DEFINICIÓN DE OBJETOS DE CLASES NECESARIAS.
    //-------------------------------------------------------------------------------------------------------------------------
    CLIApp cliApp;
    CreacionArchivoJson jsonfile;
    GzipTools gzip_tools;
    Utils utils;

    //-------------------------------------------------------------------------------------------------------------------------
    // CREACION DE CLI DEL PROYECTO.
    //-------------------------------------------------------------------------------------------------------------------------
    
    // FLAGS de control del sistema
    bool init_mode = false;
    bool reset_mode = false;
    bool status_mode = false;

    app.add_flag("--init", init_mode, "Inicializa el sistema de backup");
    app.add_flag("--reset", reset_mode, "Reinicia el sistema (borra inicialización)");
    app.add_flag("--status", status_mode, "Muestra si el sistema está inicializado");

    // Parámetros funcionales del sistema de backup
    std::string mode;
    std::string backupType;
    bool flagResources = false;
    int numberResources = 0;
    std::vector<std::string> nameResources;
    bool flagEncryptDecrypt = false;
    std::string keyEncryptDecrypt;
    std::string storage;
    std::string encrypted_compressed_file;
    std::string pathStorage;

    // Definición de opciones del CLI
    app.add_option("--m,--modo", mode, "Modo de operación: backup o recuperacion")->check(CLI::IsMember({"backup", "recuperacion"}));
    app.add_option("--bT,--backupType", backupType, "Tipo de backup: completo, incremental, diferencial")->check(CLI::IsMember({"completo", "incremental", "diferencial"}));
    app.add_option("--fS,--flagResources", flagResources, "Bandera para múltiples recursos (1 = sí, 0 = no)")->check(CLI::IsMember({0, 1}));
    app.add_option("--nuS,--numberResources", numberResources, "Número de archivos o carpetas (de 0 a 100)")->check(CLI::Range(0, 100));
    app.add_option("--naS,--nameResources", nameResources, "Nombre(s) de archivo(s) o carpeta(s)");
    app.add_option("--fED,--flagEncryptDecrypt", flagEncryptDecrypt, "¿Encriptar o desencriptar? (1 = sí, 0 = no)")->check(CLI::IsMember({0, 1}));
    app.add_option("--kED,--keyEncryptDecrypt", keyEncryptDecrypt, "Clave de cifrado de 32 caracteres");
    app.add_option("--s,--storage", storage, "Destino de almacenamiento: usb, nube, local, etc.")->check(CLI::IsMember({"usb", "nube", "local"}));
    app.add_option("--eCF,--encryptedCompressedFile", encrypted_compressed_file, "Archivo comprimido encriptado (para recuperación)");
    app.add_option("--ps,--pathStorage", pathStorage, "Ruta de almacenamiento");

    CLI11_PARSE(app, argc, argv);

    //-------------------------------------------------------------------------------------------------------------------------
    // VALIDACIONES MODOS INICIALES.
    //-------------------------------------------------------------------------------------------------------------------------
    int resultado_de_modos_iniciales = cliApp.modos_iniciales(cliApp, init_mode, reset_mode, status_mode);
    if (resultado_de_modos_iniciales != -1) {
        return resultado_de_modos_iniciales;
    }
    std::cout << "\n" << std::string(120, '-') << std::endl;

    //-------------------------------------------------------------------------------------------------------------------------
    // VALIDACIONES PREVIAS.
    //-------------------------------------------------------------------------------------------------------------------------
    int resultado_de_validaciones = cliApp.realizar_validaciones_previas(cliApp, mode, backupType, flagEncryptDecrypt, 
                                                                        keyEncryptDecrypt, numberResources, nameResources);
    if (resultado_de_validaciones != -1) {
        return resultado_de_validaciones;  // Detiene el programa si ya se ejecutó algo o si falló alguna validación
    }
    std::cout << "\n" << std::string(120, '-') << std::endl;
    std::cout << GREEN << "✅ Todas las validaciones previas se han completado con éxito." << RESET << std::endl;

    
    if (mode == "backup") {
        //-------------------------------------------------------------------------------------------------------------------------
        // SECCIÓN DE CREACIÓN DE BACKUPS.
        //-------------------------------------------------------------------------------------------------------------------------
        fs::path ruta_al_json;
        json archivo_json;
        if(backupType == "completo")
        {
            //--------------------------
            // PROCESAR BACKUP COMPLETO
            // -------------------------
            std::vector<fs::path> rutas;
            for (const auto& name : nameResources) {
                fs::path ruta(name);
                if (fs::exists(ruta)) {
                    rutas.push_back(ruta);
                } else {
                    std::cerr << "❌ Error: La ruta no existe: " << ruta << "\n";
                    return 1;
                }
            }
        
            archivo_json = jsonfile.process_multiple_paths(rutas);
            ruta_al_json =  jsonfile.guardar_respaldo_json(archivo_json, backupType, flagEncryptDecrypt);
        }
        else if (backupType == "diferencial")
        {
            //-----------------------------
            // PROCESAR BACKUP DIFERENCIAL
            // ----------------------------
            std::string tipo_backup = "completo";
            std::string tipo_backup_mayuscula = tipo_backup;
            fs::path path_input;
            std::vector<BackupInfo> backup_por_tipo = jsonfile.obtener_backups_ordenados_por_tipo(JSONS_DIR, tipo_backup);

            if (!backup_por_tipo.empty()) {
                const auto& ultimo_completo = backup_por_tipo.back();
                std::transform(tipo_backup_mayuscula.begin(),tipo_backup_mayuscula.end(),tipo_backup_mayuscula.begin(),::toupper);

                std::cout << "\n✅ Último backup de tipo " << tipo_backup_mayuscula << ".\n";
                std::cout << "  ➤ Ruta archivo: " << ultimo_completo.nombre_archivo.string() << "\n";
                path_input = ultimo_completo.nombre_archivo;
                std::cout << "  ➤ Fecha:   " << ultimo_completo.fecha_hora << "\n";
                std::cout << "  ➤ Tipo:    " << ultimo_completo.tipo << "\n";
                std::cout << "  ➤ Cifrado: " << ultimo_completo.encriptado << "\n";
                
            } else {
                std::cout << "\n❌ No se encontraron backups de tipo " << tipo_backup_mayuscula << ".\n";
            }
            std::cout << "\nProcesando backup diferencial...\n";
            if (!fs::exists(path_input)) {
                std::cerr << RED << "❌ Error: No se encontró el archivo de entrada: " << path_input << RESET << std::endl;
                return 1;
            }
            std::cout << "Procesando archivo de entrada: " << path_input << std::endl;
            archivo_json = jsonfile.procesar_backup_diferencial(path_input);
            ruta_al_json =  jsonfile.guardar_respaldo_json(archivo_json, backupType, flagEncryptDecrypt);
        }else if (backupType == "incremental")
        {
            //-----------------------------
            // PROCESAR BACKUP DIFERENCIAL
            // ----------------------------
            fs::path path_input;
            std::cout << "\nProcesando backup incremental...\n";

            auto ultimo_general = jsonfile.obtener_ultimo_backup_general(JSONS_DIR);
            if (ultimo_general.has_value()) {
                const auto& b = ultimo_general.value();
                std::cout << "Último backup encontrado:\n";
                std::cout << "  ➤ Archivo: " << b.nombre_archivo.filename() << "\n";
                std::cout << "  ➤ Fecha:   " << b.fecha_hora << "\n";
                std::cout << "  ➤ Tipo:    " << b.tipo << "\n";
                std::cout << "  ➤ Cifrado: " << b.encriptado << "\n";
            } else {
                std::cout << "⚠️  No se encontró ningún backup en la ruta.\n";
            }

            auto base_incremental = jsonfile.obtener_ultimo_valido_para_incremental(JSONS_DIR);

            if (base_incremental.has_value()) {
                const auto& b = base_incremental.value();
                std::cout << "\n✅ Se puede realizar un backup incremental.\n";
                std::cout << "   ➤ Último válido: " << b.nombre_archivo.filename() << "\n";
                path_input = b.nombre_archivo;
                std::cout << "   ➤ Tipo: " << b.tipo << "\n";
            } else {
                std::cout << "\n⛔ No se puede realizar un backup incremental porque el último backup fue diferencial.\n";
                std::cout << "   ➤ Sugerencia: Realiza un backup completo.\n";
                return 0;
            }
            std::cout << "Procesando archivo de entrada: " << path_input << std::endl;
            archivo_json = jsonfile.procesar_backup_incremental(path_input);
            ruta_al_json =  jsonfile.guardar_respaldo_json(archivo_json, backupType, flagEncryptDecrypt);
        }
        else {
            std::cerr << RED << "❌ Error: Tipo de backup no reconocido: " << backupType << RESET << std::endl;
            return 0;
        }
        //-------------------------------------------------------------------------------------------------------------------------
        // SECCIÓN DE ENCRIPTADO Y COMPRESIÓN DEL BACKUP.
        //-------------------------------------------------------------------------------------------------------------------------
        std::cout << std::string(120, '-') << std::endl;
        std::cout << GREEN << "✅ Se ha creado el archivo JSON correctamente." << RESET << std::endl;
        std::cout << "El archivo JSON se ha guardado en: " << ruta_al_json << std::endl;
        std::cout << std::string(120, '-') << std::endl;
        std::cout << "🗃️  Iniciando proceso de compresión..." << std::endl;
        utils.prepare_output_directory();

        json outJson;
        if(!jsonfile.load_json_structure(ruta_al_json.string(), outJson)) {
            std::cerr << RED << "Error al cargar la estructura JSON." << RESET << std::endl;
            return 1;
        }

        std::cout << "Procesamiento del archivo JSON: " << ruta_al_json << std::endl;
        std::cout << std::string(120, '-') << std::endl;
        std::cout << "🗃️  Proceso de compresión en curso..." << std::endl;
        std::cout << std::string(120, '-') << std::endl;
        std::cout << GREEN << std::string(120, '|') << "\n" << std::string(120, 'v') << RESET << std::endl;
        std::cout << std::string(120, '-') << std::endl;

        for (const auto& root : outJson["backup"]) {
            gzip_tools.process_root_directory(root, flagEncryptDecrypt, keyEncryptDecrypt);
        }
        gzip_tools.group_and_package_intermediate_tars(flagEncryptDecrypt, keyEncryptDecrypt);
        gzip_tools.create_final_backup_package(backupType, flagEncryptDecrypt, keyEncryptDecrypt, storage, pathStorage);

        fs::remove_all(OUTPUT_DIR);
        fs::remove_all(OUTPUT_DIR_ENCRYPT);

        std::cout << std::string(120, '-') << std::endl;
        std::cout << GREEN << std::string(120, '^') << "\n" << std::string(120, '|') << RESET << std::endl;
        std::cout << std::string(120, '-') << std::endl;

        std::cout << BLUE << "\nEXCELENTE...\nProceso de compresión finalizado exitosamente.\nArchivos en: " << TARGET_PATH << "\n" << RESET;
        }
    else if (mode == "recuperacion") {
        //-------------------------------------------------------------------------------------------------------------------------
        // SECCIÓN DE RECUPERACIÓN DE BACKUPS.
        //-------------------------------------------------------------------------------------------------------------------------
        if (!fs::exists(RECOVER_FILES)) {
            fs::create_directories(RECOVER_FILES);
        }
        std::cout << "Modo de recuperación seleccionado.\n";
        std::string backup_file_path;

        if (fs::is_regular_file(encrypted_compressed_file)) {
            backup_file_path = encrypted_compressed_file;
        } 
        else if (fs::is_directory(encrypted_compressed_file)) {
            backup_file_path = (encrypted_compressed_file.back() == '/' ? encrypted_compressed_file : encrypted_compressed_file + "/") + "backup_final.tar.gz";
        } 
        else {
            std::cerr << RED << "❌ Error: El encrypted_compressed_file no es un archivo ni un directorio válido: " << encrypted_compressed_file << "\n" << RESET;
            return 1;
        }

        std::cout << "Buscando archivo en: " << backup_file_path << std::endl;

        if (!fs::exists(backup_file_path)) {
            std::cerr << RED << "❌ Error: No se encontró el archivo de respaldo en: " << backup_file_path << "\n" << RESET;
            return 1;
        }

        std::cout << YELLOW << "Iniciando recuperación del respaldo desde: " << backup_file_path << RESET << std::endl;

        Decrypt_Decompress structDecryDecom = gzip_tools.decompress_desencript_backup_file(backup_file_path, RECOVER_FILES, keyEncryptDecrypt);
        if (!structDecryDecom.exito) {
            std::cerr << RED << "❌ Error al procesar el archivo de recuperación." << RESET << std::endl;
            return 1;
        }

        fs::path destino_final;
        if (storage == "usb" && !pathStorage.empty()) {
            destino_final = pathStorage / structDecryDecom.path_directorio_nombre_base;
            
            std::cout << "Destino final esperado: " << destino_final << std::endl;
            try {
                if (!fs::exists(pathStorage)) {
                    fs::create_directories(pathStorage);
                    std::cout << "Directorio creado: " << pathStorage << "\n";
                }

                if (jsonfile.copy_directory_recursive(structDecryDecom.path_directorio_completo, destino_final)) {
                    fs::remove_all(structDecryDecom.path_directorio_completo);
                    std::cout << "Directorio original eliminado: " << structDecryDecom.path_directorio_completo << "\n";
                    std::cout << "Backup extraído movido a: " << destino_final << "\n";
                } else {
                    std::cerr << "⚠️ Error en la copia, no se elimina el original.\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "❌ Error al mover el backup extraído a destino personalizado: " << e.what() << "\n";
                return false;
            }
        }
        else if (storage == "local" && !pathStorage.empty()) {
            destino_final = pathStorage / structDecryDecom.path_directorio_nombre_base;
            
            std::cout << "Destino final esperado: " << destino_final << std::endl;
            try {
                if (!fs::exists(pathStorage)) {
                    fs::create_directories(pathStorage);
                    std::cout << "Directorio creado: " << pathStorage << "\n";
                }
                if (fs::exists(destino_final)){
                    fs::remove_all(destino_final);
                }
                fs::rename(structDecryDecom.path_directorio_completo, destino_final);
                std::cout << "Backup extraído movido a: " << destino_final << "\n";
            } catch (const std::exception& e) {
                std::cerr << "❌ Error al mover el backup extraído a destino personalizado: " << e.what() << "\n";
                return false;
            }
        }
        else if (storage == "nube" && !pathStorage.empty()) {
            fs::path local_file = structDecryDecom.path_directorio_completo;
            std::string id_carpeta_drive = pathStorage; // ID de carpeta en la nube
            std::string cmd_nube_cargar_archivo = "python3 ./includes/backup_cloud.py \"" + local_file.string() + "\" " + id_carpeta_drive;
            std::cout << "Ejecutando comando para subir a la nube: " << cmd_nube_cargar_archivo << "\n";
            int result = system(cmd_nube_cargar_archivo.c_str());
            if (result != 0) {
                std::cerr << "❌ Error al subir archivo a la nube.\n";
                return false;
            }
            std::cout << "☁️ Backup extraído movido a ubicación en nube: " << pathStorage << "\n";
        } else {
            std::cerr << "❌ Tipo de almacenamiento inválido: " << storage << "\n";
            return false;
        }

        std::cout << GREEN << "✅ Recuperación completada exitosamente." << RESET << std::endl;
    }
    return 0;
}
