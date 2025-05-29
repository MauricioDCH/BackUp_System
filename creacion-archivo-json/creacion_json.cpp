/**
 * @file creacion_json.cpp
 * @author Mauricio David Correa Hernandez
 * @brief Implementación de la clase GzipTools para manejar compresión y descompresión de archivos usando gzip, incluyendo cifrado y descifrado de archivos.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "creacion_json.h"
#include "../includes/utils.h"

/**
 * @brief Constructor por defecto de la clase CreacionArchivoJson.
 * 
 */
CreacionArchivoJson::CreacionArchivoJson(){}

/**
 * @brief Convierte el tiempo de archivo a una cadena con formato legible.
 * @param ft Tiempo de archivo a convertir.
 * @return Cadena con la fecha y hora formateada.
 */
std::string CreacionArchivoJson::time_to_string_files(fs::file_time_type ft) {
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ft - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    std::time_t tt = std::chrono::system_clock::to_time_t(sctp);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}


/**
 * @brief Procesa un solo path (archivo o carpeta) y genera un nodo JSON.
 * @param path Ruta del archivo o carpeta a procesar.
 * @return Nodo JSON con la información del archivo o carpeta.
 */
json CreacionArchivoJson::process_path(const fs::path& path) {
    json node;
    node["path"] = path.string();
    node["last_modified"] = time_to_string_files(fs::last_write_time(path));

    if (fs::is_directory(path)) {
        node["type"] = "directory";
        node["children"] = json::array();

        std::vector<fs::directory_entry> entries;
        for (const auto& entry : fs::directory_iterator(path)) {
            entries.push_back(entry);
        }

        std::sort(entries.begin(), entries.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
            return a.path().filename() < b.path().filename();
        });

        std::vector<json> temp_results(entries.size());

        std::transform(std::execution::par, entries.begin(), entries.end(), temp_results.begin(),
            [this](const fs::directory_entry& entry) {
                return process_path(entry.path());
            });

        for (const auto& result : temp_results) {
            node["children"].push_back(result);
        }

    } else if (fs::is_regular_file(path)) {
        node["type"] = "file";
        node["size"] = fs::file_size(path);
    } else {
        node["type"] = "other";
    }

    return node;
}


/**
 * @brief Procesa múltiples paths y los agrega al JSON de salida.
 * @param paths Vector de rutas a procesar.
 * @return Nodo JSON con la lista de archivos y carpetas procesados.
 */
json CreacionArchivoJson::process_multiple_paths(const std::vector<fs::path>& paths) {
    json result;
    result["backup"] = json::array();

    std::vector<std::future<json>> futures;

    for (const auto& path : paths) {
        if (fs::exists(path)) {
            // Lanza cada procesamiento en un hilo separado
            futures.push_back(std::async(std::launch::async, &CreacionArchivoJson::process_path, this, path));
        } else {
            std::cerr << "[ERROR] La ruta no existe: " << path << std::endl;
        }
    }

    for (auto& future : futures) {
        result["backup"].push_back(future.get());
    }

    return result;
}


/**
 * @brief Guarda el JSON generado en un archivo con un nombre basado en la fecha y hora actual.
 * @param resultado JSON a guardar.
 * @param backupType Tipo de backup (completo, diferencial, incremental).
 * @param estaEncriptado Indica si el backup está encriptado.
 * @return Ruta del archivo guardado.
 */
fs::path CreacionArchivoJson::guardar_respaldo_json(const json& resultado,const std::string& backupType,bool estaEncriptado) {
    // Paso 1: Crear carpeta si no existe
    Utils utils;
    fs::path carpeta_destino = JSONS_DIR.string();
    if (!fs::exists(carpeta_destino)) {
        fs::create_directories(carpeta_destino);
    }

    // Paso 2: Obtener fecha y hora actual
    std::string fecha_hora = utils.time_to_string_hora_actual();

    // Paso 3: Construir nombre del archivo
    std::string estado_encriptado = estaEncriptado ? "encriptado" : "noEncriptado";

    std::ostringstream nombre_archivo;
    nombre_archivo << "backup_" << fecha_hora << "_"
                    << backupType << "_" << estado_encriptado << ".json";

    fs::path ruta_archivo = carpeta_destino / nombre_archivo.str();

    // Paso 4: Guardar JSON con formato bonito
    std::ofstream archivo_salida(ruta_archivo);
    archivo_salida << std::setw(4) << resultado << std::endl;
    archivo_salida.close();

    return ruta_archivo;
}


/**
 * @brief Carga la estructura JSON desde un archivo y valida que contenga el array "backup".
 * @param filePath Ruta del archivo JSON a cargar.
 * @param outJson Referencia al objeto JSON donde se guardará la estructura cargada.
 * @return true si se cargó correctamente, false en caso contrario.
 */
bool CreacionArchivoJson::load_json_structure(const std::string& filePath, json& outJson) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        std::cerr << RED << "No se pudo abrir " << filePath << RESET << std::endl;
        return false;
    }
    in >> outJson;

    if (!outJson.contains("backup") || !outJson["backup"].is_array()) {
        std::cerr << RED << "JSON inválido: falta 'backup' array." << RESET << std::endl;
        return false;
    }
    return true;
}


/**
 * @brief Obtiene información de un archivo y la devuelve como un objeto JSON.
 * @param path Ruta del archivo a procesar.
 * @return Nodo JSON con la información del archivo, o nullptr si no es un archivo regular.
 */
json CreacionArchivoJson::get_file_info(const fs::path& path) {
    fs::file_status status = fs::status(path);
    if (fs::exists(status) && fs::is_regular_file(status)) {
        json file_json;
        file_json["path"] = path.string();                                              // Ruta completa
        file_json["type"] = "file";                                                     // Tipo de objeto
        file_json["last_modified"] = time_to_string_files(fs::last_write_time(path));   // Fecha modificada legible
        file_json["size"] = fs::file_size(path);                                        // Tamaño del archivo
        return file_json;
    }
    return nullptr;
}

/**
 * @brief Obtiene información de un directorio y sus archivos, comparando con un JSON previo para detectar cambios.
 * @param dir_path Ruta del directorio a procesar.
 * @param prev_dir_json JSON previo del directorio para comparar.
 * @param archivos_modificados Vector donde se guardarán las rutas de archivos modificados.
 * @return Nodo JSON con la información del directorio y sus archivos modificados, o nullptr si no hay cambios.
 */
json CreacionArchivoJson::get_directory_info(const fs::path& dir_path, const json& prev_dir_json, 
                                            std::vector<std::string>& archivos_modificados) {
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path))
        return nullptr;

    json result;
    result["path"] = dir_path.string();
    result["type"] = "directory";
    result["last_modified"] = time_to_string_files(fs::last_write_time(dir_path));

    json updated_children = json::array();

    // Mapa de hijos previos del JSON anterior, indexado por ruta
    std::map<std::string, json> prev_children_map;
    if (prev_dir_json.contains("children")) {
        for (const auto& item : prev_dir_json["children"]) {
            std::string item_path = item["path"].get<std::string>();
            prev_children_map[item_path] = item;
        }
    }

    std::vector<fs::directory_entry> entries;
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        entries.push_back(entry);
    }

    std::vector<json> children(entries.size());

    // Mutex para proteger la lista de archivos modificados en entorno paralelo
    std::mutex mutex_modificados;

    // Procesamiento paralelo de las entradas del directorio
    std::transform(std::execution::par, entries.begin(), entries.end(), children.begin(),
        [&](const fs::directory_entry& entry) -> json {
            fs::path current_path = entry.path();
            std::string current_path_str = current_path.string();

            if (entry.is_regular_file()) {
                json current_info = get_file_info(current_path);
                auto it = prev_children_map.find(current_path_str);

                if (it == prev_children_map.end() ||
                    it->second["last_modified"] != current_info["last_modified"]) {
                    std::lock_guard<std::mutex> lock(mutex_modificados);
                    archivos_modificados.push_back(current_path_str);
                }
                return current_info;
            }

            else if (entry.is_directory()) {
                json prev;

                auto prev_it = prev_children_map.find(current_path_str);
                if (prev_it != prev_children_map.end()) {
                    prev = prev_it->second;
                }

                std::vector<std::string> hijos_modificados;

                json dir_info = get_directory_info(current_path, prev, hijos_modificados);

                if (!hijos_modificados.empty()) {
                    std::lock_guard<std::mutex> lock(mutex_modificados);  // Protección del vector compartido
                    archivos_modificados.insert(
                        archivos_modificados.end(),
                        hijos_modificados.begin(),
                        hijos_modificados.end()
                    );
                    return dir_info;
                }
                return nullptr;
            }
            return nullptr;
        }
    );

    for (const auto& child : children) {
        if (!child.is_null()) {
            updated_children.push_back(child);
        }
    }

    result["children"] = updated_children;
    return updated_children.empty() ? nullptr : result;
}


/**
 * @brief Obtiene todos los backups ordenados por fecha y hora.
 * @param ruta Ruta del directorio donde se encuentran los backups.
 * @return Vector de BackupInfo con la información de los backups encontrados.
 */
std::vector<BackupInfo> CreacionArchivoJson::obtener_backups_todos_ordenados(const std::string& ruta) {
    std::vector<BackupInfo> backups;

    // Expresión regular para extraer información del nombre del archivo
    std::regex patron(R"(backup_(\d{8}_\d{6})_(completo|diferencial|incremental)_(encriptado|noEncriptado)\.json)");

    for (const auto& entry : fs::directory_iterator(ruta)) {
        std::smatch coincidencias;
        std::string nombre = entry.path().filename().string();

        if (std::regex_match(nombre, coincidencias, patron)) {
            backups.push_back({
                entry.path(),
                coincidencias[2],
                coincidencias[1],
                coincidencias[3]
            });
        }
    }

    std::sort(backups.begin(), backups.end(), [](const BackupInfo& a, const BackupInfo& b) {
        return a.fecha_hora < b.fecha_hora;
    });

    return backups;
}

/**
 * @brief Obtiene los backups ordenados por tipo específico (completo, diferencial, incremental).
 * @param ruta Ruta del directorio donde se encuentran los backups.
 * @param tipo_filtro Tipo de backup a filtrar (completo, diferencial, incremental).
 * @return Vector de BackupInfo con la información de los backups filtrados.
 */
std::vector<BackupInfo> CreacionArchivoJson::obtener_backups_ordenados_por_tipo(const std::string& ruta, const std::string& tipo_filtro) {
    auto todos = obtener_backups_todos_ordenados(ruta);
    std::vector<BackupInfo> filtrados;

    for (const auto& b : todos) {
        if (b.tipo == tipo_filtro) {
            filtrados.push_back(b);
        }
    }

    return filtrados;
}

/**
 * @brief Procesa el archivo backup.json y genera un JSON con solo los archivos modificados.
 * @param path_input Ruta del archivo backup.json a procesar.
 * @return Nodo JSON con la información de los archivos modificados.
 */
json CreacionArchivoJson::procesar_backup_diferencial(const fs::path& path_input) {
    std::ifstream input_file(path_input);
    if (!input_file.is_open()) {
        std::cerr << "❌ Error: No se pudo abrir el archivo backup.json\n";
        return json();
    }

    json original_json;
    input_file >> original_json;
    std::vector<std::string> archivos_modificados;

    json updated_json;
    updated_json["backup"] = json::array();

    for (const auto& item : original_json["backup"]) {
        if (!item.contains("path")) continue;

        fs::path item_path = item["path"].get<std::string>();

        if (item["type"] == "file") {
            if (fs::exists(item_path)) {
                json current_info = get_file_info(item_path);
                if (item.contains("last_modified") &&
                    current_info["last_modified"] != item["last_modified"]) {
                    archivos_modificados.push_back(current_info["path"]);
                    updated_json["backup"].push_back(current_info);
                }
            }
        } else if (item["type"] == "directory") {
            if (fs::exists(item_path)) {
                json dir_info = get_directory_info(item_path, item, archivos_modificados);
                if (!dir_info.is_null()) {
                    updated_json["backup"].push_back(dir_info);
                }
            }
        }
    }

    if (!archivos_modificados.empty()) {
        std::cout << "📂 Archivos nuevos o modificados encontrados:\n";
        for (const auto& ruta : archivos_modificados) {
            std::cout << "  - " << ruta << "\n";
        }
    } else {
        std::cout << "✅ No hay archivos modificados.\n";
    }

    std::cout << "✅ Actualización incremental procesada con éxito.\n";
    return updated_json;
}

/**
 * @brief Obtiene el último backup general (completo, diferencial o incremental) ordenado por fecha.
 * @param ruta Ruta del directorio donde se encuentran los backups.
 * @return BackupInfo del último backup encontrado, o std::nullopt si no hay backups.
 */
std::optional<BackupInfo> CreacionArchivoJson::obtener_ultimo_backup_general(const std::string& ruta) {
    auto backups = obtener_backups_todos_ordenados(ruta);

    if (!backups.empty()) {
        return backups.back();
    }

    return std::nullopt;
}

/**
 * @brief Obtiene el último backup válido para un backup incremental, buscando el más reciente completo o incremental.
 * @param ruta Ruta del directorio donde se encuentran los backups.
 * @return BackupInfo del último backup válido encontrado, o std::nullopt si no hay backups válidos.
 */
std::optional<BackupInfo> CreacionArchivoJson::obtener_ultimo_valido_para_incremental(const std::string& ruta) {
    auto backups = obtener_backups_todos_ordenados(ruta);

    for (auto it = backups.rbegin(); it != backups.rend(); ++it) {
        if (it->tipo == "completo" || it->tipo == "incremental") {
            return *it;
        } else if (it->tipo == "diferencial") {
            break;
        }
    }

    return std::nullopt;
}

/**
 * @brief Procesa el archivo backup.json y genera un JSON con solo los archivos modificados.
 * Este método es específico para backups incrementales, comparando el estado actual del sistema de archivos
 * con el backup anterior para identificar cambios.
 * @param path_input Ruta del archivo backup.json a procesar.
 * @return Nodo JSON con la información de los archivos modificados.
 */
json CreacionArchivoJson::procesar_backup_incremental(const fs::path& path_input) {
    std::ifstream input_file(path_input);
    if (!input_file.is_open()) {
        std::cerr << "❌ Error: No se pudo abrir el archivo backup.json\n";
        return json();
    }

    json original_json;
    input_file >> original_json;
    std::vector<std::string> archivos_modificados;

    json updated_json;
    updated_json["backup"] = json::array();

    for (const auto& item : original_json["backup"]) {
        if (!item.contains("path")) continue;

        fs::path item_path = item["path"].get<std::string>();

        if (item["type"] == "file") {
            if (fs::exists(item_path)) {
                json current_info = get_file_info(item_path);
                if (item.contains("last_modified") &&
                    current_info["last_modified"] != item["last_modified"]) {
                    archivos_modificados.push_back(current_info["path"]);
                    updated_json["backup"].push_back(current_info);
                }
            }
        } else if (item["type"] == "directory") {
            if (fs::exists(item_path)) {
                json dir_info = get_directory_info(item_path, item, archivos_modificados);
                if (!dir_info.is_null()) {
                    updated_json["backup"].push_back(dir_info);
                }
            }
        }
    }

    if (!archivos_modificados.empty()) {
        std::cout << "📂 Archivos nuevos o modificados encontrados:\n";
        for (const auto& ruta : archivos_modificados) {
            std::cout << "  - " << ruta << "\n";
        }
    } else {
        std::cout << "✅ No hay archivos modificados.\n";
    }

    std::cout << "✅ Actualización incremental procesada con éxito.\n";
    return updated_json;
}

/**
 * @brief Copia un directorio de forma recursiva a otro directorio.
 * @param from Ruta del directorio de origen.
 * @param to Ruta del directorio de destino.
 * @return true si la copia fue exitosa, false en caso contrario.
 */
bool CreacionArchivoJson::copy_directory_recursive(const fs::path& from, const fs::path& to) {
    try {
        if (!fs::exists(from) || !fs::is_directory(from))
            return false;

        if (fs::exists(to))
            fs::remove_all(to);  // Limpia si ya existe

        fs::create_directories(to);

        for (const auto& entry : fs::recursive_directory_iterator(from)) {
            const auto& path = entry.path();
            auto relative_path = fs::relative(path, from);
            auto target = to / relative_path;

            if (fs::is_directory(path)) {
                fs::create_directories(target);
            } else if (fs::is_regular_file(path)) {
                fs::copy_file(path, target, fs::copy_options::overwrite_existing);
            } else if (fs::is_symlink(path)) {
                auto symlink_target = fs::read_symlink(path);
                fs::create_symlink(symlink_target, target);
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "❌ Error al copiar directorio recursivamente: " << e.what() << "\n";
        return false;
    }
}