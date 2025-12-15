/**
 * @file crypto.cpp
 * @author Мезин А.А.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация криптографических функций
 * @warning Учебный пример
 */

#include "crypto.h"

/**
 * @brief Вычисление хеша для аутентификации
 * @param[in] salt Соль для усиления хеша
 * @param[in] pass Пароль пользователя
 * @return Хеш SHA-224(salt + password) в hex-формате
 * @details Использует библиотеку CryptoPP для вычисления хеша
 */
string auth(string salt, string pass){

CPP::SHA224 sha224;
string hash;
CPP::StringSource(
    string(salt).append(pass), 
    true,
        new CPP::HashFilter(
            sha224,
            new CPP::HexEncoder(
                new CPP::StringSink(hash)))
);

return hash;
}
