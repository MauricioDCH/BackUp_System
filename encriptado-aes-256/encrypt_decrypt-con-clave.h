/**
 * @file encrypt_decrypt-con-clave.h
 * @author Mauricio David Correa Hernandez
 * @brief Declaración de la clase EncryptDecrypt para manejar el cifrado y descifrado de datos usando AES-256.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>


/**
 * @class EncryptDecrypt
 * @brief Clase encargada de manejar el cifrado y descifrado de datos usando AES-256.
 * 
 */
class EncryptDecrypt {
    public:
        /**
         * @brief Cifra un bloque de datos usando AES-256 en modo CBC.
         * 
         * @param plaintext 
         * @param ciphertext 
         * @param key 
         * @param iv 
         * @return true 
         * @return false 
         */
        bool aes_encrypt(const std::vector<unsigned char>& plaintext, std::vector<unsigned char>& ciphertext, const unsigned char* key, const unsigned char* iv);
        
        /**
         * @brief Cifra un archivo usando AES-256 y una clave maestra.
         * 
         * @param input_filename Nombre del archivo de entrada.
         * @param output_filename Nombre del archivo de salida.
         * @param master_key_str Clave maestra para cifrado.
         * @return true Si el cifrado fue exitoso.
         * @return false Si hubo un error durante el cifrado.
         */
        bool encrypt_file(const std::string& input_filename, const std::string& output_filename, const std::string& master_key_str);
        
        /**
         * @brief Descifra un bloque de datos usando AES-256 en modo CBC.
         * 
         * @param ciphertext 
         * @param plaintext 
         * @param key 
         * @param iv 
         * @return true 
         * @return false 
         */
        bool aes_decrypt(const std::vector<unsigned char>& ciphertext, std::vector<unsigned char>& plaintext, const unsigned char* key, const unsigned char* iv);
        
        /**
         * @brief Descifra un archivo usando AES-256 y una clave maestra.
         * 
         * @param input_filename Nombre del archivo de entrada.
         * @param output_filename Nombre del archivo de salida.
         * @param master_key_str Clave maestra para descifrado.
         * @return true Si el descifrado fue exitoso.
         * @return false Si hubo un error durante el descifrado.
         */
        bool decrypt_file(const std::string& input_filename, const std::string& output_filename, const std::string& master_key_str);

};