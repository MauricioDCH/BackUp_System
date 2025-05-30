# Documentación Técnica Extendida - BackUp System
**Por: Mauricio David Correa Hernández.**

---
---

## 1. Arquitectura del Sistema

**BackUp\_System** es una aplicación desarrollada en **C++** con el objetivo de realizar respaldos y recuperación de archivos mediante procesos de compresión y/o encriptación. La arquitectura del sistema se diseñó bajo principios de modularidad y eficiencia, aprovechando bibliotecas especializadas para tareas críticas como compresión (zlib) y encriptación (OpenMP).

### Componentes Principales:

| Módulo                     | Función Principal                                                            |
| -------------------------- | ---------------------------------------------------------------------------- |
| **CLI**                    | Gestión de argumentos, menú interactivo y ejecución de comandos del usuario. |
| **Gestor de Archivos**     | Escaneo de archivos, organización, lectura y escritura en estructuras JSON.  |
| **Módulo de Compresión**   | Compresión y descompresión de archivos con Gzip mediante zlib.               |
| **Módulo de Encriptación** | Encriptación AES-256 de archivos y desencriptación para recuperación.        |

### Flujo General del Sistema

```
                                 +-----------------+
                                 |    Usuario CLI  |
                                 +--------+--------+
                                          |  
                                          v
                            +-------------+--------------+
                            | Inicialización del Sistema |
                            +-------------+--------------+
                                         |
                                         v
                            +------------+------------+
                            | Selección: Respaldo o   |
                            | Recuperación            |
                            +------------+------------+
                                |                 |
                                v                 v
                        +----------------+   +------------------+
                        | Módulo Backup  |   | Módulo Recuperar |
                        +----------------+   +------------------+
                                |					   |
                                v					   v
                       +------------------+  +---------------------+  
                       | Generación .json |	 | Archivos y carpetas |
                       +------------------+  +---------------------+
                                |
                                v
                    +------------------------+
                    | Tipo de Backup         |
                    | (Completo/Dif/Inc)     |
                    +------------------------+
                    |                        |
                    v                        v
            +-------+--------+     +---------+--------+
            |  Compresión    |     |   Encriptación   |
            +-------+--------+     +---------+--------+
                    |                        |
                    +------------+-----------+
                                 |
                                 v
                    +------------+-------------+
                    | Archivo Final .tar.enc.gz|
                    |             o            |
                    |   Archivo Final .tar.gz  |
                    +--------------------------+

```

### Dependencias y librerías.

| Biblioteca / Encabezado       | Funcionalidad / Uso Principal                                                        | Requiere instalación externa | URL/documentación                                                |
| ----------------------------- | ------------------------------------------------------------------------------------ | ---------------------------- | ---------------------------------------------------------------- |
| `<iostream>`, `<fstream>`     | Entrada/salida estándar y manejo de archivos                                         | NO                            | Estándar C++                                                     |
| `<filesystem>`                | Navegación y manipulación de archivos y directorios                                  | SI (C++17+)                     | [cppreference.com](https://en.cppreference.com/w/cpp/filesystem) |
| `<regex>`                     | Manejo de expresiones regulares                                                      | NO                            | Estándar C++                                                     |
| `<future>`, `<execution>`     | Programación concurrente y paralelismo                                               | SI (C++17+)                     | [cppreference.com](https://en.cppreference.com/)                 |
| `<unistd.h>`, `<sys/stat.h>`  | Acceso a funciones del sistema UNIX (permisos, procesos, etc.)                       | NO                            | POSIX                                                            |
| `<omp.h>`                     | Soporte para ejecución paralela usando OpenMP                                        | SI                            | [openmp.org](https://www.openmp.org/)                            |
| `<openssl/evp.h>`, `<rand.h>` | Encriptación AES-256 y generación de números aleatorios seguros                      | SI                            | [OpenSSL](https://www.openssl.org/docs/)                         |
| `<zlib.h>`                    | Compresión y descompresión de archivos con Gzip                                      | SI                            | [zlib.net](https://zlib.net/)                                    |
| `nlohmann/json.hpp`           | Serialización y deserialización de datos en formato JSON                             | SI                            | [nlohmann/json](https://github.com/nlohmann/json)                |
| `CLI/CLI.hpp`                 | Librería para parsing de argumentos desde línea de comandos (CLI11)                  | SI                            | [CLI11](https://github.com/CLIUtils/CLI11)                       |
| `<mutex>`, `<thread>`         | Sincronización y concurrencia de hilos                                               | NO                            | Estándar C++                                                     |
| `<chrono>`, `<ctime>`, etc.   | Medición de tiempo y marcas temporales                                               | NO                            | Estándar C++                                                     |
| `<iomanip>`, `<sstream>`      | Manipulación y formateo de cadenas, conversión de tipos                              | NO                            | Estándar C++                                                     |
| `<memory>`, `<array>`, etc.   | Estructuras modernas de datos y manejo de memoria inteligente (smart pointers, etc.) | NO                            | Estándar C++                                                     |

### Instalaciones requeridas.
Ejecutando las instalaciones en Ubuntu:

```bash
# Actualiza los paquetes
sudo apt update

# Para OpenSSL (encriptación AES-256)
sudo apt install libssl-dev

# Para zlib (compresión Gzip)
sudo apt install zlib1g-dev

# Para OpenMP (paralelización con #pragma omp)
sudo apt install libomp-dev

# Para nlohmann/json (manejo de JSON en C++)
sudo apt install nlohmann-json3-dev

# Para CLI11 (parser de argumentos tipo CLI)
git clone https://github.com/CLIUtils/CLI11.git
cd CLI11
mkdir build && cd build
cmake ..
sudo make install
cd ../..
rm -rf CLI11  # Opcional: limpiar después de instalar
```

### Especificaciones del Entorno.

| Recurso                        | Requisito mínimo                                                                             |
| ------------------------------ | -------------------------------------------------------------------------------------------- |
| **Sistema Operativo**          | Linux (Ubuntu/Debian), macOS, o Windows con WSL                                              |
| **Compilador**                 | `g++` versión 11.2 o superior (compatible con C++17 o superior)                              |
| **Estándar C++**               | C++17 mínimo (`-std=c++17`)                                                                  |
| **Herramienta de Compilación** | `CMake` ≥ 3.16, o alternativamente `make`                                                    |
| **OpenSSL**                    | Instalado y enlazado (ej. `libssl-dev` en Ubuntu)                                            |
| **zlib**                       | Instalado y enlazado (`zlib1g-dev` en Ubuntu)                                                |
| **OpenMP**                     | Activado en el compilador con `-fopenmp`                                                     |
| **CLI11**                      | Incluir headers en el proyecto o instalar desde [CLI11](https://github.com/CLIUtils/CLI11)   |
| **nlohmann/json**              | Incluir el header único (`json.hpp`) desde [nlohmann/json](https://github.com/nlohmann/json) |

### Compilación y ejecución.
Como tenemos un Makefile, entonces se ejecuta el make para compilar y enlazar.

```bash
make
```

Si se desea, se puede ejecutar la compilación y el enlazamiento de la siguiente manera.

```bash
g++ -Wall -std=c++17 -Iinclude -fopenmp -c main.cpp -o main.o
g++ -Wall -std=c++17 -Iinclude -fopenmp -c CLI/cliente.cpp -o CLI/cliente.o
g++ -Wall -std=c++17 -Iinclude -fopenmp -c creacion-archivo-json/creacion_json.cpp -o creacion-archivo-json/creacion_json.o
g++ -Wall -std=c++17 -Iinclude -fopenmp -c gzip_tool/gzip_tool_fll.cpp -o gzip_tool/gzip_tool_fll.o
g++ -Wall -std=c++17 -Iinclude -fopenmp -c includes/utils.cpp -o includes/utils.o
g++ -Wall -std=c++17 -Iinclude -fopenmp -c encriptado-aes-256/encrypt_decrypt-con-clave.cpp -o encriptado-aes-256/encrypt_decrypt-con-clave.o
g++ -Wall -std=c++17 -Iinclude -fopenmp -o backupApp main.o CLI/cliente.o creacion-archivo-json/creacion_json.o gzip_tool/gzip_tool_fll.o includes/utils.o encriptado-aes-256/encrypt_decrypt-con-clave.o -lz -lssl -lcrypto -lpthread -ltbb
```

Para que el sistema pueda ejecutarce sin problema, debemos inicializar el sistema mediante el comando.

Si usamos el Makefile.

```bash
run-init
```

Si usamos el archivo ejecutable después de compilar.

```bash
./backupApp --init
```

Archivo Makefile internamente.
```makefile
# Compiler
CXX = g++

# Compilation flags
CXXFLAGS = -Wall -std=c++17 -Iinclude -fopenmp

# Libraries
LDFLAGS = -lz -lssl -lcrypto -lpthread -ltbb
# sudo dnf install tbb-devel
# Name of the executable
EXEC = backupApp

# Source files
SRC = main.cpp \
      CLI/cliente.cpp \
      creacion-archivo-json/creacion_json.cpp \
      gzip_tool/gzip_tool_fll.cpp \
      includes/utils.cpp \
      encriptado-aes-256/encrypt_decrypt-con-clave.cpp

# Object files
OBJ = main.o \
      CLI/cliente.o \
      creacion-archivo-json/creacion_json.o \
      gzip_tool/gzip_tool_fll.o \
      includes/utils.o \
      encriptado-aes-256/encrypt_decrypt-con-clave.o

# Default target: compile the executable
all: $(EXEC)

# Link object files to create executable
$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJ) $(LDFLAGS)

# Compile each source file into object files
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

CLI/cliente.o: CLI/cliente.cpp
	$(CXX) $(CXXFLAGS) -c CLI/cliente.cpp -o CLI/cliente.o

creacion-archivo-json/creacion_json.o: creacion-archivo-json/creacion_json.cpp
	$(CXX) $(CXXFLAGS) -c creacion-archivo-json/creacion_json.cpp -o creacion-archivo-json/creacion_json.o

gzip_tool/gzip_tool_fll.o: gzip_tool/gzip_tool_fll.cpp
	$(CXX) $(CXXFLAGS) -c gzip_tool/gzip_tool_fll.cpp -o gzip_tool/gzip_tool_fll.o

includes/utils.o: includes/utils.cpp
	$(CXX) $(CXXFLAGS) -c includes/utils.cpp -o includes/utils.o

encriptado-aes-256/encrypt_decrypt-con-clave.o: encriptado-aes-256/encrypt_decrypt-con-clave.cpp
	$(CXX) $(CXXFLAGS) -c encriptado-aes-256/encrypt_decrypt-con-clave.cpp -o encriptado-aes-256/encrypt_decrypt-con-clave.o

# Clean build artifacts
clean:
	rm -f $(OBJ) $(EXEC)

clean-jsons-and-backup-files:
	rm backup_files/* && rm .json_backups_files/* && rm recovered_files/* && rm recuperacion_files/*

# Run initialization, reset, and status commands
run-init:
	./$(EXEC) --init

run-reset:
	./$(EXEC) --reset

run-status:
	./$(EXEC) --status

# Run backup and recovery commands with different configurations

## Backup commands
### Run backup with encryption and compression complete store: local.
run-backup-encrypt-completo:
	./$(EXEC) 	--modo backup \
				--backupType completo \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage local \
				--pathStorage backup_files

### Run backup with encryption and compression complete store: cloud.
run-backup-encrypt-completo-a-nube:
	./$(EXEC) 	--modo backup \
				--backupType completo \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage nube \
				--pathStorage 1xzKZwAjNphvftf5TtsrU5HhEfziYs316

### Run backup with encryption and compression complete store: USB.
run-backup-encrypt-completo-a-usb:
	./$(EXEC) 	--modo backup \
				--backupType completo \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage usb \
				--pathStorage /run/media/madacohe/USB_MAURO/BackUp

### Run backup without encryption but compression complete store: local.
run-backup-no-encrypt-completo:
	./$(EXEC) 	--modo backup \
				--backupType completo \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 0 \
				--storage local \
				--pathStorage backup_files

### Run backup with encryption and compression type: incremental store: local.
run-backup-encrypt-incremental:
	./$(EXEC) 	--modo backup \
				--backupType incremental \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage local \
				--pathStorage backup_files

### Run backup without encryption but compression type: incremental store: local.
run-backup-no-encrypt-incremental:
	./$(EXEC) 	--modo backup \
				--backupType incremental \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 0 \
				--storage local \
				--pathStorage backup_files

### Run backup with encryption and compression type: differential store: local.
run-backup-encrypt-diferencial:
	./$(EXEC) 	--modo backup \
				--backupType diferencial \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage local \
				--pathStorage backup_files

### Run backup without encryption but compression type: differential store: local.
run-backup-no-encrypt-diferencial:
	./$(EXEC) 	--modo backup \
				--backupType diferencial \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 0 \
				--storage local \
				--pathStorage backup_files


## Recover commands
run-recuperacion-encrypt:
	./$(EXEC) 	--modo recuperacion \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage local \
				--encryptedCompressedFile backup_files/comprimido_backup_20250529_152919_completo_encriptado.tar.enc.gz \
				--pathStorage recuperacion_files

### Run recovery with encryption and compression store: cloud.
run-recuperacion-encrypt-a-nube:
	./$(EXEC) 	--modo recuperacion \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage nube \
				--encryptedCompressedFile backup_files/comprimido_backup_20250529_152919_completo_encriptado.tar.enc.gz \
				--pathStorage 1xzKZwAjNphvftf5TtsrU5HhEfziYs316

### Run recovery with encryption and compression store: USB.
run-recuperacion-encrypt-a-usb:
	./$(EXEC) 	--modo recuperacion \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage usb \
				--encryptedCompressedFile backup_files/comprimido_backup_20250529_152919_completo_encriptado.tar.enc.gz \
				--pathStorage /run/media/madacohe/USB_MAURO/BackUp

### Run recovery without encryption but compression store: local.
run-recuperacion-no-encrypt:
	./$(EXEC) 	--modo recuperacion \
				--flagEncryptDecrypt 0 \
				--storage local \
				--encryptedCompressedFile backup_files/comprimido_backup_20250529_153014_completo_noEncriptado.tar.gz \
				--pathStorage recuperacion_files
```

Los comandos que hay disponibles para la ejecución son:
```bash
# Comando para hacer un backup completo encriptado y comprimido, guardado localmente.
run-backup-encrypt-completo

# Comando para hacer un backup completo encriptado y comprimido, guardado en nube.
run-backup-encrypt-completo-a-nube

# Comando para hacer un backup completo encriptado y comprimido, guardado en una usb.
run-backup-encrypt-completo-a-usb

# Comando para hacer un backup completo sin encriptado pero comprimido, guardado localmente.
run-backup-no-encrypt-completo

# Comando para hacer un backup incremental encriptado y comprimido, guardado localmente.
run-backup-encrypt-incremental

# Comando para hacer un backup incremental sin encriptado y comprimido, guardado localmente.
run-backup-no-encrypt-incremental

# Comando para hacer un backup diferencial encriptado y comprimido, guardado localmente.
run-backup-encrypt-diferencial

# Comando para hacer un backup diferencial sin encriptado y comprimido, guardado localmente.
run-backup-no-encrypt-diferencial

# Comando para hacer un recuperación de archivo encriptado y comprimido, guardado localmente.
run-recuperacion-encrypt

# Comando para hacer un recuperación de archivo encriptado y comprimido, guardado en nube.
run-recuperacion-encrypt-a-nube

# Comando para hacer un recuperación de archivo encriptado y comprimido, guardado en usb.
run-recuperacion-encrypt-a-usb

# Comando para hacer un recuperación de archivo sin encriptado y comprimido, guardado localmente.
run-recuperacion-no-encrypt
```

Igualmente, se puede ejecutar el comando directamente en la termina.

Ejemplo de ejecución por comando.

```bash
./backupApp --modo recuperacion --flagEncryptDecrypt 1 --keyEncryptDecrypt 12345678901234567890123456789012 --storage usb --encryptedCompressedFile backup_files/comprimido_backup_20250529_152919_completo_encriptado.tar.enc.gz --pathStorage /run/media/madacohe/USB_MAURO/BackUp
```

---
---

## 2. Implementación de Paralelismo con OpenMP y la STL de C++

En esta implementación se emplean herramientas modernas de C++ como `OpenMP`, `std::mutex`, `std::execution` y `std::future` para introducir paralelismo y concurrencia en distintas partes del sistema.

### 2.1. Bibliotecas Utilizadas

| Cabecera      | Uso principal                               | Tipo de Paralelismo                         |
| ------------- | ------------------------------------------- | ------------------------------------------- |
| `<future>`    | `std::async`, `std::promise`, `std::future` | Multihilo asincrónico                       |
| `<execution>` | Políticas como `std::execution::par`        | Paralelismo algorítmico (C++17 en adelante) |
| `<mutex>`     | Control de acceso a recursos compartidos    | Sincronización entre hilos                  |
| `<omp.h>`     | Directivas `#pragma omp`                    | Paralelismo con múltiples hilos (OpenMP)    |

Aunque OpenMP es ampliamente utilizado en C/C++ para lograr paralelismo a nivel de CPU mediante directivas del preprocesador (`#pragma omp`), en este proyecto se optó por combinar esta herramienta con una alternativa moderna y más flexible: **la biblioteca estándar de C++ (STL)**. Esto incluye el uso de `std::async` para tareas asincrónicas, `std::mutex` para sincronización entre hilos, y `std::transform` junto con `std::execution::par` para recorrer colecciones en paralelo.

La inclusión de la cabecera `<omp.h>` permite acceder a las funcionalidades propias de OpenMP, como la paralelización de bucles `for`, control de secciones críticas con `#pragma omp critical`, y el balanceo dinámico de carga mediante `schedule(dynamic)`. Esta estrategia híbrida facilita un uso eficiente de los recursos del sistema y se alinea mejor con las arquitecturas contemporáneas, permitiendo optimizar tareas intensivas de procesamiento y I/O.

### 2.2. Paralelismo en Procesamiento de Archivos JSON

#### `process_multiple_paths`

Procesa múltiples rutas (archivos o carpetas) de forma concurrente utilizando `std::async` y ejecutando un hilo por ruta:

```cpp
std::vector<std::future<json>> futures;

for (const auto& path : paths) {
    futures.push_back(std::async(std::launch::async, &CreacionArchivoJson::process_path, this, path));
}
```

Los resultados se recogen mediante `future.get()` y se integran al JSON final.

#### `process_path`

Este método recorre de manera recursiva una ruta de archivo o directorio y genera nodos JSON. Si se trata de un directorio, se usa `std::transform` con ejecución paralela:

```cpp
std::transform(std::execution::par, entries.begin(), entries.end(), temp_results.begin(),
    [this](const fs::directory_entry& entry) {
        return process_path(entry.path());
    });
```

Este enfoque optimiza el recorrido de estructuras grandes de carpetas, reduciendo el tiempo de operaciones I/O y cómputo.

#### `get_directory_info`

Compara el estado actual de un directorio con una versión previa en JSON. Se usa `std::transform` con `std::execution::par` para recorrer los elementos:

```cpp
std::transform(std::execution::par, entries.begin(), entries.end(), children.begin(),
    [&](const fs::directory_entry& entry) -> json {
        ...
    });
```

Para evitar condiciones de carrera en `archivos_modificados`, se emplea `std::mutex`:

```cpp
std::lock_guard<std::mutex> lock(mutex_modificados);
archivos_modificados.push_back(current_path_str);
```

### 2.3. Paralelismo en Compresión y Descompresión

#### `compress_files_by_extension`

Agrupa archivos por extensión y los comprime de forma paralela.

* **OpenMP (`#pragma omp parallel for`)**: Se paraleliza el bucle de procesamiento de grupos con `schedule(dynamic)`:

  ```cpp
  #pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < (int)group_vec.size(); ++i) {
      // Procesamiento de cada grupo
  }
  ```

  El uso de `schedule(dynamic)` mejora el rendimiento en tareas con tiempos de ejecución variables.

* **Sección crítica**: Se emplea `#pragma omp critical` para proteger la operación de encriptación:

  ```cpp
  #pragma omp critical
  {
      // Encriptación del archivo
  }
  ```

#### `group_and_package_intermediate_tars`

Agrupa archivos intermedios por directorio y los empaqueta en archivos `.tar.gz`.

* **Paralelización con OpenMP**:

  ```cpp
  #pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < (int)groups_vec.size(); ++i) {
      // Empaquetado de cada grupo
  }
  ```

* **Secciones críticas**: Se protege la encriptación y la salida por consola:

  ```cpp
  #pragma omp critical
  {
      // Mensajes o encriptación
  }
  ```

#### `decompress_desencript_backup_file`

Descomprime y desencripta archivos comprimidos, y procesa recursivamente los contenidos internos.

* **Paralelización con OpenMP**:

  ```cpp
  #pragma omp parallel for
  for (int i = 0; i < static_cast<int>(compressed_files.size()); ++i) {
      decompress_desencript_backup_file(compressed_files[i], ...);
  }
  ```

Este enfoque permite tratar múltiples archivos internos de forma concurrente, acelerando significativamente el proceso de restauración.

---
---

## 3. Algoritmo de Compresión Clásico Elegido

### Algoritmo: **Gzip (basado en DEFLATE)**

El algoritmo elegido para la compresión y descompresión de archivos en este proyecto es **Gzip**, el cual utiliza internamente el algoritmo **DEFLATE**, una técnica ampliamente adoptada en sistemas UNIX, navegadores web, archivos `.tar.gz`, y protocolos como HTTP y SSH.

### 3.1. Características Generales

* **Tipo:** Compresión sin pérdida (lossless).
* **Estandarización:** Definido en la RFC 1951 (DEFLATE) y RFC 1952 (Gzip).
* **Compatibilidad:** Multiplataforma. Usado por defecto en muchas herramientas como `gzip`, `gunzip`, `tar`, `HTTP content-encoding`.
* **Tamaño típico:** Ofrece tasas de compresión que van del 30% al 70%, dependiendo del contenido.
* **Velocidad:** Muy rápido en descompresión, con velocidad moderada en compresión.

### 3.2. Funcionamiento Interno de DEFLATE

El algoritmo DEFLATE combina **dos técnicas clásicas de compresión**:

1. **LZ77 (Sliding Window Compression)**

   * Busca y reemplaza repeticiones de cadenas de bytes con referencias a ocurrencias anteriores en una ventana de tamaño fijo (generalmente 32 KB).
   * Reduce la redundancia de datos.

2. **Codificación de Huffman**

   * Aplica una codificación de longitud variable para representar los símbolos con mayor frecuencia usando menos bits.
   * Optimiza aún más la representación binaria de los datos.

Este doble enfoque permite que DEFLATE tenga un **balance muy eficiente entre velocidad y compresión**, lo que lo hace ideal para sistemas que necesitan tratar archivos en tiempo real o semiautónomos como los sistemas distribuidos.

### 3.3. Biblioteca Utilizada: **zlib**

Para implementar Gzip/DEFLATE en C++, se utiliza la biblioteca **zlib**, una implementación ampliamente adoptada que proporciona:

* Funciones para compresión y descompresión: `compress()`, `uncompress()`, `deflate()`, `inflate()`.
* Opciones para ajustar el **nivel de compresión** (0 a 9).
* Funciones avanzadas como `gzopen()`, `gzread()`, `gzwrite()` para manipular archivos `.gz`.


**Documentación oficial:**
[https://zlib.net/manual.html](https://zlib.net/manual.html)

### 3.4. Integración.

Ver código en.

**[gzip_tool_fll.h](./gzip_tool/gzip_tool_fll.h)**

**[gzip_tool_fll.cpp](./gzip_tool/gzip_tool_fll.cpp)**

#### Los métodos principales son: 

##### En la sección de compresión.

```cpp
void process_root_directory(const json& root, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt);

void group_and_package_intermediate_tars(const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt);

void create_final_backup_package(const std::string& backupType, const bool flagEncryptDecrypt, const std::string keyEncryptDecrypt, const std::string& storage, const std::string& pathStorage);
```

##### En la sección de descompresión.
```cpp
Decrypt_Decompress decompress_desencript_backup_file(const std::string& input_file, const std::string& output_dir, const std::string& master_key);
```

### 3.5. Consideraciones de Rendimiento

* **Compresión:** Más costosa computacionalmente; depende de la repetitividad del contenido.
* **Descompresión:** Extremadamente rápida y poco costosa.
* **Consumo de memoria:** Bajo en comparación con otros algoritmos como Bzip2 o LZMA.
* **Paralelización:** zlib en sí no paraleliza, pero puede ejecutarse de forma concurrente sobre múltiples archivos en hilos separados, como se hace en este sistema usando OpenMP o `std::async`.

### 3.6. Seguridad y Robustez

* **Sin cifrado:** Gzip no cifra los datos, por lo tanto, debe combinarse con algoritmos de encriptación como AES si se requiere confidencialidad.
* **Integridad de datos:** Incluye un checksum CRC32 al final del archivo `.gz` para detectar corrupción.
* **Resiliencia a errores:** Un error de un solo bit en un archivo `.gz` puede invalidar bloques completos; por ello, se recomienda empaquetar datos en partes o backups incrementales.

---
---

## 4. Algoritmo de Encriptación Clásico Elegido

### Algoritmo: **AES-256 en modo CBC (Cipher Block Chaining)**

El algoritmo elegido para proteger la confidencialidad de los archivos comprimidos en este sistema es **AES-256**, uno de los estándares de cifrado más robustos y utilizados en la industria. Para este proyecto se optó por el modo de operación **CBC (Cipher Block Chaining)**, el cual garantiza que patrones repetidos no produzcan bloques cifrados idénticos, aumentando así la seguridad frente a ataques por análisis de patrones.

### 4.1. Características Generales

* **Tipo:** Cifrado simétrico por bloques.
* **Tamaño de clave:** 256 bits (32 bytes).
* **Tamaño del bloque:** 128 bits (16 bytes).
* **Modo CBC:** Añade aleatoriedad mediante un vector de inicialización (IV) distinto por cada cifrado.
* **Estándar:** AES está definido por el NIST (FIPS PUB 197).
* **Seguridad:** Considerado seguro siempre y cuando se use una clave fuerte y el IV no se reutilice.

### 4.2. Funcionamiento Interno

El cifrado AES-256 opera sobre bloques de 128 bits utilizando una clave de 256 bits. El modo CBC añade una capa adicional de seguridad al realizar una operación XOR entre el bloque actual y el resultado cifrado del bloque anterior (o con el IV si es el primer bloque), de modo que la misma entrada genera salidas diferentes.

**Etapas del proceso de cifrado en CBC:**

1. El mensaje se divide en bloques de 128 bits.
2. Cada bloque es **XOR** con el resultado del bloque cifrado anterior.
3. El bloque resultante se cifra usando **AES-256**.
4. Se utiliza un **vector de inicialización (IV)** aleatorio en el primer bloque.
5. Para descifrar, se invierte el proceso con la misma clave y IV.

Este modo **evita patrones repetitivos** en los datos cifrados y es ideal para archivos binarios y respaldos comprimidos.

### 4.3. Biblioteca Utilizada: **OpenSSL (EVP)**

Para implementar AES-256-CBC en C++, se utiliza la biblioteca **OpenSSL**, una de las bibliotecas criptográficas más utilizadas y auditadas en el mundo. A través de su interfaz **EVP**, proporciona una abstracción de alto nivel para múltiples algoritmos criptográficos.

**Funciones clave utilizadas:**

* `EVP_EncryptInit_ex()` y `EVP_DecryptInit_ex()`: Inicializan el contexto de cifrado y descifrado.
* `EVP_EncryptUpdate()` y `EVP_DecryptUpdate()`: Procesan los datos por bloques.
* `EVP_EncryptFinal_ex()` y `EVP_DecryptFinal_ex()`: Finalizan la operación de cifrado/descifrado.

**Documentación oficial:**
[https://www.openssl.org/docs/man3.0/man3/EVP\_EncryptInit\_ex.html](https://www.openssl.org/docs/man3.0/man3/EVP_EncryptInit_ex.html)

### 4.4. Integración

Ver código en.

**[encrypt_decrypt-con-clave.h](./encriptado-aes-256/encrypt_decrypt-con-clave.h)**

**[encrypt_decrypt-con-clave.cpp](./encriptado-aes-256/encrypt_decrypt-con-clave.cpp)**

#### Los métodos principales son:

##### Cifrado de datos:

```cpp
bool encrypt_file(const std::string& input_filename, const std::string& output_filename, const std::string& master_key_str);
```

##### Descifrado de datos:

```cpp
bool decrypt_file(const std::string& input_filename, const std::string& output_filename, const std::string& master_key_str);
```

Estos métodos son utilizados dentro del flujo de compresión para proteger los archivos tar antes de ser almacenados o transferidos.

### 4.5. Consideraciones de Seguridad

* **Confidencialidad fuerte:** AES-256 con CBC provee alta seguridad si se maneja correctamente el IV y la clave.
* **IV aleatorio por archivo:** Cada archivo cifrado usa un IV diferente, almacenado junto al archivo de forma segura.
* **Gestión de claves:** La clave se pasa como parámetro al método, y debe estar protegida mediante políticas seguras (por ejemplo, derivación mediante PBKDF2 con sal).
* **Padding:** Se utiliza **PKCS#7** para rellenar el último bloque si no tiene longitud completa.
* **Integridad:** Este sistema solo cifra, por lo que se recomienda combinar con autenticación como HMAC o AEAD si se desea protección contra modificación.

### 4.6. Consideraciones de Rendimiento

* **Velocidad:** Muy rápido, especialmente si se usa aceleración por hardware (AES-NI).
* **Consumo de memoria:** Bajo; opera sobre bloques pequeños.
* **Multihilo:** Compatible con paralelización si los archivos se cifran por separado.

---
---

## 5. Decisiones de Diseño

### Lenguaje: **C++**

**Justificación:**

* Alto rendimiento (útil en compresión/encriptación).
* Bajo consumo de recursos.
* Fácil integración con bibliotecas en C (zlib/OpenSSL).
* Permite manejo fino de memoria.

### JSON como formato de configuración y metadatos

* Legible por humanos.
* Compatible con múltiples lenguajes.
* Facilita guardar información estructurada sobre los archivos respaldados (nombre, tamaño, fecha, hash, etc.).

### Estructura Modular

Separar funciones en diferentes archivos permite:

* Mayor mantenibilidad.
* Mejora la legibilidad del sistema.
* Facilita futuras expansiones (ej. añadir paralelismo o una GUI).

---

## 6. Recomendaciones Futuras

* Incluir verificación de integridad de archivos con **hash SHA256**.
* Permitir restauración parcial de respaldos desde el JSON.
* Añadir una interfaz gráfica en Qt o portar CLI a web.
* Incluir manejo de errores robusto y bitácora (`logger`).

---

## 7. Bibliografía

* **zlib Manual**: [https://zlib.net/manual.html](https://zlib.net/manual.html)
* **OpenSSL Crypto Docs**: [https://docs.openssl.org/master/](https://docs.openssl.org/master/)
* **OpenMP Specification**: [https://www.openmp.org/specifications/](https://www.openmp.org/specifications/)
* **AES (NIST)**: [https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf)
* **JSON Format**: [http://json.org/json-en.html](http://json.org/json-en.html)
* **Tipos de backups**: [Explicación de los tipos de backups: Completa, Incremental y Diferencial](https://www.nakivo.com/es/blog/backup-types-explained/)

## 8. Sustentación.

[Video sustentación](https://eafit-my.sharepoint.com/:v:/g/personal/mdcorreah_eafit_edu_co/Eb_kyZCE7Q9Aq_327jjnzhEBP7XxsZMwfRyHH5qTAwT8Xg?e=pv3zaY&nav=eyJyZWZlcnJhbEluZm8iOnsicmVmZXJyYWxBcHAiOiJTdHJlYW1XZWJBcHAiLCJyZWZlcnJhbFZpZXciOiJTaGFyZURpYWxvZy1MaW5rIiwicmVmZXJyYWxBcHBQbGF0Zm9ybSI6IldlYiIsInJlZmVycmFsTW9kZSI6InZpZXcifX0%3D)