/**
 * @file crypto.h
 * @author Мезин А.А.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для криптографических функций
 * @warning Учебный пример
 */

#pragma once
#include <string>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
using namespace std;
namespace CPP = CryptoPP;

/**
 * @brief Функция аутентификации с использованием SHA-224
 * @param[in] salt Соль для хеширования
 * @param[in] pass Пароль для хеширования
 * @return Хеш-строка в шестнадцатеричном формате
 */
string auth(string salt, string pass);