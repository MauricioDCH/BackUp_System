# Compiler
CXX = g++

# Compilation flags
CXXFLAGS = -Wall -std=c++20 -Iinclude -fopenmp

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

run-init:
	./$(EXEC) --init

run-reset:
	./$(EXEC) --reset

run-status:
	./$(EXEC) --status


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


run-backup-no-encrypt-completo:
	./$(EXEC) 	--modo backup \
				--backupType completo \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 0 \
				--storage local \
				--pathStorage backup_files

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

run-backup-no-encrypt-incremental:
	./$(EXEC) 	--modo backup \
				--backupType incremental \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 0 \
				--storage local \
				--pathStorage backup_files

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

run-backup-no-encrypt-diferencial:
	./$(EXEC) 	--modo backup \
				--backupType diferencial \
				--flagResources 1 \
				--numberResources 1 \
				--nameResources file_system_example/SistemasOperativos/LlamadaSistema/Memoria/buddySystem/ \
				--flagEncryptDecrypt 0 \
				--storage local \
				--pathStorage backup_files



run-recuperacion-encrypt:
	./$(EXEC) 	--modo recuperacion \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage local \
				--encryptedCompressedFile backup_files/comprimido_backup_20250529_152919_completo_encriptado.tar.enc.gz \
				--pathStorage recuperacion_files

run-recuperacion-encrypt-a-nube:
	./$(EXEC) 	--modo recuperacion \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage nube \
				--encryptedCompressedFile backup_files/comprimido_backup_20250529_152919_completo_encriptado.tar.enc.gz \
				--pathStorage 1xzKZwAjNphvftf5TtsrU5HhEfziYs316

run-recuperacion-encrypt-a-usb:
	./$(EXEC) 	--modo recuperacion \
				--flagEncryptDecrypt 1 \
				--keyEncryptDecrypt 12345678901234567890123456789012 \
				--storage usb \
				--encryptedCompressedFile backup_files/comprimido_backup_20250529_152919_completo_encriptado.tar.enc.gz \
				--pathStorage /run/media/madacohe/USB_MAURO/BackUp

run-recuperacion-no-encrypt:
	./$(EXEC) 	--modo recuperacion \
				--flagEncryptDecrypt 0 \
				--storage local \
				--encryptedCompressedFile backup_files/comprimido_backup_20250529_153014_completo_noEncriptado.tar.gz \
				--pathStorage recuperacion_files
