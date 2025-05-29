/**
 * @file gzip_tool_fll.h
 * @author Mauricio David Correa Hernandez
 * @brief Implementación de la clase GzipTools para manejar compresión y descompresión de archivos usando gzip, incluyendo cifrado y descifrado de archivos.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "gzip_tool_fll.h"

/**
 * @brief Constructor por defecto de la clase GzipTools.
 * 
 */
GzipTools::GzipTools(){}

/**
 * @brief Verifica si una ruta es un directorio.
 * @param path Ruta a verificar.
 * @return true si es un directorio, false en caso contrario.
 */
bool GzipTools::is_directory(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) return false;
    return S_ISDIR(statbuf.st_mode);
}


/**
 * @brief Comprime un archivo usando gzip.
 * @param input_filename Nombre del archivo de entrada.
 * @param output_filename Nombre del archivo de salida comprimido.
 * @return true si la compresión fue exitosa, false en caso contrario.
 */
bool GzipTools::compress_file_gzip(const char* input_filename, const char* output_filename) {
    char buffer[BUFFER_SIZE];

    FILE* input = fopen(input_filename, "rb");
    if (!input) {
        std::cerr << "Error al abrir archivo de entrada: " << strerror(errno) << "\n";
        return false;
    }

    gzFile output = gzopen(output_filename, "wb9");
    if (!output) {
        std::cerr << "Error al abrir archivo de salida gzip.\n";
        fclose(input);
        return false;
    }

    int bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input)) > 0) {
        gzwrite(output, buffer, bytes_read);
    }

    fclose(input);
    gzclose(output);

    std::cout << GREEN << "Compresión completada: " << output_filename << RESET << "\n";
    return true;
}

/**
 * @brief Comprime archivos en un directorio agrupándolos por extensión.
 * @param dir Directorio que contiene los archivos a comprimir.
 * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
 * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
 */
void GzipTools::compress_files_by_extension(const fs::path& dir, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt) {
    std::map<std::string, std::vector<std::string>> groups;

    // Agrupa archivos por extensión
    try {
        for (auto& entry : fs::directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;
            std::string ext = entry.path().extension().string();
            if (ext.empty()) ext = ".sin_extension";
            groups[ext].push_back(entry.path().string());
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error accediendo al directorio: " << dir << "\n";
        std::cerr << "Excepción: " << e.what() << "\n";
        return;
    }

    // Convertimos map a vector para acceso con índices y paralelización
    std::vector<std::pair<std::string, std::vector<std::string>>> group_vec(groups.begin(), groups.end());

    // Paralelizamos el procesamiento por extensión usando OpenMP
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < (int)group_vec.size(); ++i) {
        auto& [ext, paths] = group_vec[i];

        std::string base = dir.filename().string();
        fs::path tarP = OUTPUT_DIR / (base + "_" + ext.substr(1) + ".tar");

        if(flagEncryptDecrypt) {
            fs::path encryptedTarP = tarP; encryptedTarP += ".enc";
            fs::path finalGzP = encryptedTarP; finalGzP += ".gz";

            std::string cmd = "tar -cf \"" + tarP.string() + "\" -C \"" + dir.string() + "\" ";
            for (auto& p : paths) {
                cmd += "\"" + fs::path(p).filename().string() + "\" ";
            }
            system(cmd.c_str());

            EncryptDecrypt encrypt_decrypt;

            bool encryption_ok = false;
            #pragma omp critical
            {
                encryption_ok = encrypt_decrypt.encrypt_file(tarP.string(), encryptedTarP.string(), keyEncryptDecrypt);
            }

            if (!encryption_ok) {
                std::cerr << "Error en la encriptación del archivo: " << tarP << "\n";
            } else {
                if (!compress_file_gzip(encryptedTarP.c_str(), finalGzP.c_str())) {
                    std::cerr << "Error en la compresión gzip del archivo encriptado: " << encryptedTarP << "\n";
                } else {
                    fs::remove(tarP);
                    fs::remove(encryptedTarP);
                    std::cout << GREEN << "Compresión con encriptación completada: " << finalGzP << RESET << "\n";
                }
            }
        } else {
            fs::path gzP = tarP; gzP += ".gz";

            std::string cmd = "tar -cf \"" + tarP.string() + "\" -C \"" + dir.string() + "\" ";
            for (auto& p : paths) {
                cmd += "\"" + fs::path(p).filename().string() + "\" ";
            }
            system(cmd.c_str());

            if (!compress_file_gzip(tarP.c_str(), gzP.c_str())) {
                std::cerr << "Error en la compresión gzip del archivo: " << tarP << "\n";
            } else {
                fs::remove(tarP);
                std::cout << GREEN << "Compresión completada: " << gzP << RESET << "\n";
            }
        }
    }
}

/**
 * @brief Comprime un único archivo en un archivo .tar.gz, con opción de encriptación.
 * @param filePath Ruta del archivo a comprimir.
 * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
 * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
 */
void GzipTools::compress_single_file(const fs::path& filePath, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt) {
    fs::path outputDir = OUTPUT_DIR;

    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    std::string baseName = filePath.filename().string();
    fs::path tarP = outputDir / (baseName + ".tar");

    if (flagEncryptDecrypt) {
        fs::path encryptedTarP = tarP; encryptedTarP += ".enc";
        fs::path finalGzP = encryptedTarP; finalGzP += ".gz";

        std::string cmd = "tar -cf \"" + tarP.string() + "\" -C \"" + filePath.parent_path().string() + "\" \"" + baseName + "\"";
        std::cout << "Ejecutando comando: " << cmd << "\n";
        int result = system(cmd.c_str());

        if (result != 0) {
            std::cerr << "Error al crear el archivo tar: " << tarP << "\n";
            return;
        }

        EncryptDecrypt encrypt_decrypt;
        if (!encrypt_decrypt.encrypt_file(tarP.string(), encryptedTarP.string(), keyEncryptDecrypt)) {
            std::cerr << "Error en la encriptación del archivo: " << tarP << "\n";
            return;
        }

        if (!compress_file_gzip(encryptedTarP.c_str(), finalGzP.c_str())) {
            std::cerr << "Error en la compresión gzip del archivo encriptado: " << encryptedTarP << "\n";
            return;
        }

        fs::remove(tarP);
        fs::remove(encryptedTarP);
        std::cout << GREEN << "Compresión con encriptación completada: " << finalGzP << RESET << "\n";
    } else {
        fs::path gzP = tarP; gzP += ".gz";

        std::string cmd = "tar -cf \"" + tarP.string() + "\" -C \"" + filePath.parent_path().string() + "\" \"" + baseName + "\"";
        std::cout << "Ejecutando comando: " << cmd << "\n";
        int result = system(cmd.c_str());

        if (result != 0) {
            std::cerr << "Error al crear el archivo tar: " << tarP << "\n";
            return;
        }

        if (!compress_file_gzip(tarP.c_str(), gzP.c_str())) {
            std::cerr << "Error en la compresión gzip del archivo: " << tarP << "\n";
            return;
        }

        fs::remove(tarP);
        std::cout << GREEN << "Compresión completada: " << gzP << RESET << "\n";
    }
}

/**
 * @brief Procesa un directorio y sus archivos, comprimiéndolos por extensión.
 * @brief Si encuentra subdirectorios, los procesa recursivamente.
 * @param node Nodo JSON que representa el directorio a procesar.
 * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
 * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
 */
void GzipTools::process_directory(const json& node, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt) {
    fs::path path(node["path"].get<std::string>());

    if (!fs::exists(path)) {
        std::cout << "Ruta no existe: " << path << "\n";
        return;
    }

    if (fs::is_regular_file(path)) {
        std::cout << "Es un archivo: " << path << "\n";
        compress_single_file(path, flagEncryptDecrypt, keyEncryptDecrypt);
        return;
    }

    if (!fs::is_directory(path)) {
        std::cout << "No es ni archivo ni directorio válido: " << path << "\n";
        return;
    }

    std::cout << "Procesando directorio: " << path << "\n";
    compress_files_by_extension(path, flagEncryptDecrypt, keyEncryptDecrypt);

    for (auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_directory()) continue;

        for (const auto& child : node["children"]) {
            if (child["type"] == "directory" && child["path"] == entry.path().string()) {
                process_directory(child, flagEncryptDecrypt, keyEncryptDecrypt);
                break;
            }
        }
    }
}

/**
 * @brief Procesa el directorio raíz y sus subdirectorios/archivos, comprimiéndolos por extensión.
 * @param root Nodo JSON que representa el directorio raíz a procesar.
 * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
 * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
 */
void GzipTools::process_root_directory(const json& root, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt) {
    std::cout << "Procesando directorio raíz: " << root["path"].get<std::string>() << "\n";
    process_directory(root, flagEncryptDecrypt, keyEncryptDecrypt);
    fs::path rootDir = root["path"].get<std::string>();
    compress_files_by_extension(rootDir, flagEncryptDecrypt, keyEncryptDecrypt);
}

/**
 * @brief Agrupa archivos intermedios por directorio y los empaqueta en archivos tar.gz. 
 * @brief Si se especifica la encriptación, los archivos tar se encriptan antes de comprimirlos.
 * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
 * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
 */
void GzipTools::group_and_package_intermediate_tars(const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt) {
    std::map<std::string, std::vector<fs::path>> byDir;

    for (auto& e : fs::directory_iterator(OUTPUT_DIR)) {
        if (!e.is_regular_file()) continue;

        std::string name = e.path().filename().string();
        auto pos = name.find('_');
        if (pos <= 0) continue;

        std::string groupKey = name.substr(0, pos);
        byDir[groupKey].push_back(e.path());
    }

    std::vector<std::pair<std::string, std::vector<fs::path>>> groups_vec(byDir.begin(), byDir.end());

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < (int)groups_vec.size(); ++i) {
        auto& [group, files] = groups_vec[i];
        fs::path tarPath = OUTPUT_DIR / (group + ".tar");

        // Construir comando tar
        std::string cmd = "tar -cf \"" + tarPath.string() + "\" -C \"" + OUTPUT_DIR.string() + "\" ";
        for (auto& f : files) {
            cmd += "\"" + f.filename().string() + "\" ";
        }
        system(cmd.c_str());

        if (flagEncryptDecrypt) {
            fs::path encryptedPath = tarPath; encryptedPath += ".enc";
            fs::path gzPath = encryptedPath; gzPath += ".gz";

            EncryptDecrypt encrypt_decrypt;
            bool success_encrypt = false;
            #pragma omp critical
            {
                success_encrypt = encrypt_decrypt.encrypt_file(tarPath.string(), encryptedPath.string(), keyEncryptDecrypt);
            }
            if (!success_encrypt) {
                #pragma omp critical
                std::cerr << "Error al encriptar el archivo: " << tarPath << "\n";
                continue;
            }

            if (!compress_file_gzip(encryptedPath.c_str(), gzPath.c_str())) {
                #pragma omp critical
                std::cerr << "Error al comprimir el archivo encriptado: " << encryptedPath << "\n";
                continue;
            }

            fs::remove(tarPath);
            fs::remove(encryptedPath);

            #pragma omp critical
            std::cout << GREEN << "Empaquetado con encriptación: " << gzPath << RESET << "\n";
        } else {
            fs::path gzPath = tarPath; gzPath += ".gz";

            if (!compress_file_gzip(tarPath.c_str(), gzPath.c_str())) {
                #pragma omp critical
                std::cerr << "Error al comprimir el archivo: " << tarPath << "\n";
                continue;
            }

            fs::remove(tarPath);

            #pragma omp critical
            std::cout << GREEN << "Empaquetado: " << gzPath << RESET << "\n";
        }

        for (auto& f : files) {
            fs::remove(f);
        }
    }
}



/**
 * @brief Crea un paquete final de backup, comprimiendo y encriptando si es necesario.
 * @param backupType Tipo de backup (por ejemplo, "completo", "incremental").
 * @param flagEncryptDecrypt Indica si se debe encriptar el archivo comprimido.
 * @param keyEncryptDecrypt Clave para la encriptación, si aplica.
 * @param storage Tipo de almacenamiento (por ejemplo, "usb", "local", "nube").
 * @param pathStorage Ruta personalizada para almacenar el backup, si aplica.
 */
void GzipTools::create_final_backup_package(const std::string& backupType, const bool flagEncryptDecrypt,
                                            const std::string keyEncryptDecrypt,
                                            const std::string& storage, const std::string& pathStorage) {
    Utils utils;

    std::string estado_encriptado = flagEncryptDecrypt ? "encriptado" : "noEncriptado";

    // Paso 1: Asegurar que el directorio destino por defecto exista
    if (!fs::exists(TARGET_PATH)) {
        fs::create_directories(TARGET_PATH);
    }

    // Paso 2: Generar nombre base
    std::string fecha_hora = utils.time_to_string_hora_actual();
    std::ostringstream nombre_base;
    nombre_base << "comprimido_backup_" << fecha_hora << "_" << backupType << "_" << estado_encriptado;

    // Paso 3: Definir paths temporales
    fs::path temp_tar = TARGET_PATH / (nombre_base.str() + ".tar");
    fs::path temp_enc = TARGET_PATH / (nombre_base.str() + ".tar.enc");
    fs::path temp_gz;

    // Nombre final del archivo comprimido
    std::string nombre_final = flagEncryptDecrypt ? (nombre_base.str() + ".tar.enc.gz")
                                                    : (nombre_base.str() + ".tar.gz");
    temp_gz = TARGET_PATH / nombre_final;

    // Paso 4: Crear el archivo .tar
    std::string cmd_tar = "tar -cf \"" + temp_tar.string() + "\" -C \"" + OUTPUT_DIR.string() + "\" .";
    if (system(cmd_tar.c_str()) != 0) {
        std::cerr << "❌ Error al crear el archivo tar temporal: " << temp_tar << "\n";
        return;
    }

    // Paso 5: Encriptar si es necesario
    if (flagEncryptDecrypt) {
        EncryptDecrypt encrypt_decrypt;

        if (!encrypt_decrypt.encrypt_file(temp_tar.string(), temp_enc.string(), keyEncryptDecrypt)) {
            std::cerr << "❌ Error al encriptar archivo tar: " << temp_tar << "\n";
            fs::remove(temp_tar);
            return;
        }

        fs::remove(temp_tar);

        if (!compress_file_gzip(temp_enc.c_str(), temp_gz.c_str())) {
            std::cerr << "❌ Error al comprimir archivo encriptado: " << temp_enc << "\n";
            fs::remove(temp_enc);
            return;
        }

        fs::remove(temp_enc);
    } else {
        if (!compress_file_gzip(temp_tar.c_str(), temp_gz.c_str())) {
            std::cerr << "❌ Error al comprimir archivo tar: " << temp_tar << "\n";
            fs::remove(temp_tar);
            return;
        }
        fs::remove(temp_tar);
    }

    // Paso 6: Determinar destino final según storage
    fs::path destino_final;

    if (storage == "usb") {
        if (!pathStorage.empty()) {
            destino_final = fs::path(pathStorage) / nombre_final;

            if (!fs::exists(fs::path(pathStorage))) {
                fs::create_directories(fs::path(pathStorage));
            }

            try {
                fs::copy(temp_gz, destino_final, fs::copy_options::overwrite_existing);
                fs::remove(temp_gz);
            } catch (const std::exception& e) {
                std::cerr << "❌ Error al mover archivo a ruta personalizada: " << e.what() << "\n";
                return;
            }

            std::cout << GREEN << "✅ Archivo movido a: " << destino_final << RESET << "\n";
        } else {
            destino_final = temp_gz;
            std::cout << GREEN << "✅ Archivo guardado en ruta por defecto: " << destino_final << RESET << "\n";
        }
    }
    else if (storage == "local") {
        if (!pathStorage.empty()) {
            destino_final = fs::path(pathStorage) / nombre_final;

            if (!fs::exists(fs::path(pathStorage))) {
                fs::create_directories(fs::path(pathStorage));
            }

            try {
                fs::rename(temp_gz, destino_final);
            } catch (const std::exception& e) {
                std::cerr << "❌ Error al mover archivo a ruta personalizada: " << e.what() << "\n";
                return;
            }

            std::cout << GREEN << "✅ Archivo movido a: " << destino_final << RESET << "\n";
        } else {
            destino_final = temp_gz;
            std::cout << GREEN << "✅ Archivo guardado en ruta por defecto: " << destino_final << RESET << "\n";
        }
    } else if (storage == "nube" && !pathStorage.empty()) {
        fs::path local_file = temp_gz;
        std::string id_carpeta_drive = pathStorage; // ID de carpeta en la nube
        std::string cmd_nube_cargar_archivo = "python3 ./includes/backup_cloud.py \"" + local_file.string() + "\" " + id_carpeta_drive;
        std::cout << "Ejecutando comando para subir a la nube: " << cmd_nube_cargar_archivo << "\n";
        int result = system(cmd_nube_cargar_archivo.c_str());
        if (result != 0) {
            std::cerr << "❌ Error al subir archivo a la nube.\n";
            return;
        }
        std::cout << "☁️ Backup extraído movido a ubicación en nube: " << pathStorage << "\n";
        std::cout << GREEN << "✅ Archivo subido a la nube en la carpeta con ID: " << pathStorage << RESET << "\n";
    } else {
        std::cerr << "❌ Tipo de almacenamiento inválido: " << storage << "\n";
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// DESCOMPRIMIR...
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** 
 * @brief Descomprime un archivo .gz a un archivo normal.
 * @param input_filename Nombre del archivo de entrada comprimido.
 * @param output_filename Nombre del archivo de salida descomprimido.
 * @return true si la descompresión fue exitosa, false en caso contrario.
 */
bool GzipTools::decompress_file_gzip(const char* input_filename, const char* output_filename) {
    char buffer[BUFFER_SIZE];

    gzFile input = gzopen(input_filename, "rb");
    if (!input) {
        std::cerr << "Error al abrir archivo GZIP para descompresión.\n";
        return false;
    }

    FILE* output = fopen(output_filename, "wb");
    if (!output) {
        std::cerr << "Error al abrir archivo de salida: " << strerror(errno) << "\n";
        gzclose(input);
        return false;
    }

    int bytes_read;
    while ((bytes_read = gzread(input, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes_read, output);
    }

    gzclose(input);
    fclose(output);

    std::cout << "Descompresión completada: " << output_filename << "\n";
    return true;
}


/**
 * @brief Descomprime un archivo .tar.gz a un directorio especificado.
 * @param input_file Nombre del archivo de entrada comprimido.
 * @param output_dir Directorio donde se descomprimirá el contenido.
 * @return true si la descompresión fue exitosa, false en caso contrario.
 */
bool GzipTools::decompress_directory(const char* input_file, const char* output_dir) {
    std::string tar_file = std::string(input_file) + ".untar";

    bool success = decompress_file_gzip(input_file, tar_file.c_str());
    if (!success) return false;

    std::string mkdir_cmd = "mkdir -p " + std::string(output_dir);
    system(mkdir_cmd.c_str());

    std::string untar_cmd = "tar -xf " + tar_file + " -C " + std::string(output_dir);
    if (system(untar_cmd.c_str()) != 0) {
        std::cerr << "Error al descomprimir archivo tar.\n";
        return false;
    }

    remove(tar_file.c_str());
    return true;
}

bool GzipTools::ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
            str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}


/**
 * @brief Mueve el contenido de un directorio a otro y elimina el directorio original.
 * @param src Ruta del directorio fuente.
 * @param dst Ruta del directorio destino.
 * @return true si la operación fue exitosa, false en caso contrario.
 */
bool GzipTools::move_contents_and_remove_dir(const fs::path& src, const fs::path& dst) {
    try {
        for (auto& entry : fs::directory_iterator(src)) {
            fs::path target = dst / entry.path().filename();
            fs::rename(entry.path(), target);
        }
        fs::remove(src);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "❌ Error moviendo contenido de " << src << " a " << dst << ": " << e.what() << "\n";
        return false;
    }
}



/**
 * @brief Reorganiza las carpetas de extensiones, moviendo su contenido a la carpeta base correspondiente.
 * @param root_dir Ruta del directorio raíz donde se encuentran las carpetas de extensiones.
 */
void GzipTools::reorganize_extension_folders(const fs::path& root_dir) {
    std::regex ext_folder_regex(R"((.*)_([a-zA-Z0-9]+)$)");

    for (auto it = fs::recursive_directory_iterator(root_dir, fs::directory_options::skip_permission_denied);
        it != fs::recursive_directory_iterator(); ) {

        auto current_path = it->path();

        if (it->is_directory()) {
            std::string folder_name = current_path.filename().string();
            std::smatch match;
            if (folder_name.size() > 1) {
                if (folder_name[0] == '_') {
                    fs::path parent_dir = current_path.parent_path();
                    std::cout << "📂 Moviendo contenido de carpeta '_extensión': " << current_path << " → " << parent_dir << "\n";
                    move_contents_and_remove_dir(current_path, parent_dir);
                    it = fs::recursive_directory_iterator(root_dir, fs::directory_options::skip_permission_denied);
                    continue;
                }
                else if (std::regex_match(folder_name, match, ext_folder_regex)) {
                    std::string base_folder_name = match[1].str();
                    fs::path parent_dir = current_path.parent_path();
                    fs::path base_folder_path = parent_dir / base_folder_name;
                    if (!fs::exists(base_folder_path) || !fs::is_directory(base_folder_path)) {
                        fs::path grandparent_dir = parent_dir.parent_path();
                        fs::path alternative_base_folder_path = grandparent_dir / base_folder_name;
                        if (fs::exists(alternative_base_folder_path) && fs::is_directory(alternative_base_folder_path)) {
                            base_folder_path = alternative_base_folder_path;
                        }
                    }
                    if (fs::exists(base_folder_path) && fs::is_directory(base_folder_path)) {
                        std::cout << "📂 Moviendo contenido de carpeta '" << folder_name << "' a carpeta base: " << base_folder_path << "\n";
                        move_contents_and_remove_dir(current_path, base_folder_path);

                        it = fs::recursive_directory_iterator(root_dir, fs::directory_options::skip_permission_denied);
                        continue;
                    } else {
                        std::cerr << "❌ Carpeta base no existe para: " << folder_name << "\n";
                    }
                }
            }
        }
        ++it;
    }
}


/**
 * @brief Descomprime y desencripta un archivo de backup comprimido.
 * @param input_file Ruta del archivo de entrada comprimido (puede ser .tar.gz o .tar.enc.gz).
 * @param output_dir Directorio donde se descomprimirá el contenido.
 * @param master_key Clave maestra para la desencriptación, si aplica.
 * @return Estructura Decrypt_Decompress con información del proceso.
 */
Decrypt_Decompress GzipTools::decompress_desencript_backup_file(const std::string& input_file, const std::string& output_dir, const std::string& master_key) {
    Decrypt_Decompress structDecryDecom;
    std::string tmp_gz_input = input_file;
    std::string tmp_dec_file;
    bool is_encrypted = false;

    // Detecta si el archivo es .tar.enc.gz (encriptado) o .tar.gz (solo comprimido)
    if (ends_with(input_file, ".tar.enc.gz")) {
        is_encrypted = true;
        tmp_dec_file = input_file.substr(0, input_file.size() - 11) + ".tar";
    } else if (ends_with(input_file, ".tar.gz")) {
        tmp_dec_file = input_file.substr(0, input_file.size() - 3);
    } else {
        std::cerr << "❌ Formato de archivo no soportado: " << input_file << "\n";
        structDecryDecom.exito = false;
        return structDecryDecom;
    }

    std::string tmp_intermediate_file = tmp_dec_file;

    // Si es encriptado, verificar que se proporcione clave y ajustar archivo temporal
    if (is_encrypted) {
        if (master_key.empty()) {
            std::cerr << RED << "❌ Error: Debes proporcionar la clave de desencriptado con --kED.\n" << RESET;
            structDecryDecom.exito = false;
            return structDecryDecom;
        }

        tmp_intermediate_file = input_file.substr(0, input_file.size() - 3); // Remueve .gz para dejar .enc
    }

    // Paso 1: Descomprimir GZIP
    if (!decompress_file_gzip(tmp_gz_input.c_str(), tmp_intermediate_file.c_str())) {
        std::cerr << "❌ Fallo al descomprimir GZIP: " << tmp_gz_input << "\n";
        structDecryDecom.exito = false;
        return structDecryDecom;
    }

    // Paso 2: Si está encriptado, desencriptar archivo .enc a .tar
    if (is_encrypted) {
        EncryptDecrypt crypto;
        if (!crypto.decrypt_file(tmp_intermediate_file, tmp_dec_file, master_key)) {
            std::cerr << "❌ Fallo al desencriptar: " << tmp_intermediate_file << "\n";
            structDecryDecom.exito = false;
            return structDecryDecom;
        }
        remove(tmp_intermediate_file.c_str()); // Elimina .enc
    }

    // Paso 3: Crear subdirectorio con el nombre base
    std::string base_name = fs::path(tmp_dec_file).stem().string();
    std::string extract_dir = (fs::path(output_dir) / base_name).string();
    fs::create_directories(extract_dir);

    // Paso 4: Extraer .tar en el subdirectorio
    std::string untar_cmd = "tar -xf \"" + tmp_dec_file + "\" -C \"" + extract_dir + "\"";
    if (system(untar_cmd.c_str()) != 0) {
        std::cerr << "❌ Fallo al extraer: " << tmp_dec_file << "\n";
        structDecryDecom.exito = false;
        return structDecryDecom;
    }

    remove(tmp_dec_file.c_str()); // Paso 5: Eliminar archivo .tar

    // Paso 6: Detectar archivos comprimidos internos a procesar
    std::vector<std::string> compressed_files;
    for (const auto& entry : fs::recursive_directory_iterator(extract_dir)) {
        if (entry.is_regular_file() && 
            (ends_with(entry.path().string(), ".tar.gz") || ends_with(entry.path().string(), ".tar.enc.gz"))) {
            compressed_files.push_back(entry.path().string());
        }
    }

    // Paso 7: Procesar archivos internos recursivamente en paralelo
    #pragma omp parallel for
    for (int i = 0; i < static_cast<int>(compressed_files.size()); ++i) {
        std::cout << "🔁 Procesando archivo interno: " << compressed_files[i] << "\n";
        decompress_desencript_backup_file(compressed_files[i], fs::path(compressed_files[i]).parent_path().string(), master_key);
    }

    // Paso 8: Reorganizar carpetas por convención de extensiones
    reorganize_extension_folders(extract_dir);

    // Paso 9: Reubicar archivos .gz en un directorio temporal
    fs::path new_dir = RECOVER_FILES / TEMP_GZ_DIR;
    fs::create_directories(new_dir);
    for (const auto& entry : fs::directory_iterator(extract_dir)) {
        if (entry.is_regular_file() && ends_with(entry.path().string(), ".gz")) {
            fs::rename(entry.path(), new_dir / entry.path().filename());
        }
    }
    fs::remove_all(new_dir); // Elimina el directorio temporal si es necesario

    // Finalizar y devolver estructura con información del proceso
    structDecryDecom.exito = true;
    structDecryDecom.path_directorio_completo = extract_dir;
    structDecryDecom.path_directorio_nombre_base = base_name;
    return structDecryDecom;
}
