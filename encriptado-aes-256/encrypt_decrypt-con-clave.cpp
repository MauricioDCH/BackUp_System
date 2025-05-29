/**
 * @file encrypt_decrypt-con-clave.cpp
 * @author Mauricio David Correa Hernandez
 * @brief Implementación de la clase EncryptDecrypt para manejar el cifrado y descifrado de datos usando AES-256.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "encrypt_decrypt-con-clave.h"

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
bool EncryptDecrypt::aes_encrypt(const std::vector<unsigned char>& plaintext, std::vector<unsigned char>& ciphertext,
                                    const unsigned char* key, const unsigned char* iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int len = 0, ciphertext_len = 0;

    if (!ctx) return false;

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) return false;

    ciphertext.resize(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    if (!EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size())) return false;
    ciphertext_len = len;

    if (!EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) return false;
    ciphertext_len += len;

    ciphertext.resize(ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

/**
 * @brief Cifra un archivo usando AES-256 y una clave maestra.
 * 
 * @param input_filename Nombre del archivo de entrada.
 * @param output_filename Nombre del archivo de salida.
 * @param master_key_str Clave maestra para cifrado.
 * @return true Si el cifrado fue exitoso.
 * @return false Si hubo un error durante el cifrado.
 */
bool EncryptDecrypt::encrypt_file(const std::string& input_filename, const std::string& output_filename, const std::string& master_key_str) {
    std::ifstream input(input_filename, std::ios::binary);
    std::vector<unsigned char> file_data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    unsigned char real_key[32];
    RAND_bytes(real_key, sizeof(real_key));

    unsigned char iv[16];
    RAND_bytes(iv, sizeof(iv));

    std::vector<unsigned char> encrypted_data;
    if (!aes_encrypt(file_data, encrypted_data, real_key, iv)) {
        std::cerr << "Error al encriptar archivo.\n";
        return false;
    }

    std::vector<unsigned char> enc_key_vec;

    unsigned char key_iv[16];
    RAND_bytes(key_iv, sizeof(key_iv));
    if (!aes_encrypt(std::vector<unsigned char>(real_key, real_key + 32), enc_key_vec, reinterpret_cast<const unsigned char*>(master_key_str.c_str()), key_iv)) {
        std::cerr << "Error al encriptar la clave real.\n";
        return false;
    }

    std::ofstream output(output_filename, std::ios::binary);
    output.write(reinterpret_cast<char*>(key_iv), sizeof(key_iv));                          // Escribir IV usado para encriptar la clave real
    uint32_t key_size = enc_key_vec.size();                                                 // Tamaño de la clave encriptada
    output.write(reinterpret_cast<char*>(&key_size), sizeof(key_size));                     // Tamaño clave encriptada
    output.write(reinterpret_cast<char*>(enc_key_vec.data()), enc_key_vec.size());          // Clave encriptada
    output.write(reinterpret_cast<char*>(iv), sizeof(iv));                                  // IV para datos
    output.write(reinterpret_cast<char*>(encrypted_data.data()), encrypted_data.size());    // Datos encriptados

    return true;
}

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
bool EncryptDecrypt::aes_decrypt(const std::vector<unsigned char>& ciphertext, std::vector<unsigned char>& plaintext,
                                const unsigned char* key, const unsigned char* iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int len = 0, plaintext_len = 0;

    if (!ctx) return false;

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) return false;

    plaintext.resize(ciphertext.size());
    if (!EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size())) return false;
    plaintext_len = len;

    if (!EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) return false;
    plaintext_len += len;

    plaintext.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}


/**
 * @brief Descifra un archivo usando AES-256 y una clave maestra.
 * 
 * @param input_filename Nombre del archivo de entrada.
 * @param output_filename Nombre del archivo de salida.
 * @param master_key_str Clave maestra para descifrado.
 * @return true Si el descifrado fue exitoso.
 * @return false Si hubo un error durante el descifrado.
 */
bool EncryptDecrypt::decrypt_file(const std::string& input_filename, const std::string& output_filename, const std::string& master_key_str) {
    std::ifstream input(input_filename, std::ios::binary);

    unsigned char key_iv[16];
    input.read(reinterpret_cast<char*>(key_iv), sizeof(key_iv));

    uint32_t key_size;
    input.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));

    std::vector<unsigned char> encrypted_key(key_size);
    input.read(reinterpret_cast<char*>(encrypted_key.data()), key_size);

    unsigned char iv[16];
    input.read(reinterpret_cast<char*>(iv), sizeof(iv));

    std::vector<unsigned char> encrypted_data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    std::vector<unsigned char> real_key;
    if (!aes_decrypt(encrypted_key, real_key, reinterpret_cast<const unsigned char*>(master_key_str.c_str()), key_iv)) {
        std::cerr << "Error al desencriptar la clave.\n";
        return false;
    }

    std::vector<unsigned char> decrypted_data;
    if (!aes_decrypt(encrypted_data, decrypted_data, real_key.data(), iv)) {
        std::cerr << "Error al desencriptar archivo.\n";
        return false;
    }

    std::ofstream output(output_filename, std::ios::binary);
    output.write(reinterpret_cast<char*>(decrypted_data.data()), decrypted_data.size());
    return true;
}
