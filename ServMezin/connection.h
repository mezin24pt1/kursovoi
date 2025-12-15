/**
 * @file connection.h
 * @author Мезин А.А.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для класса Connection - установка сетевых соединений
 * @warning Учебный пример
 */

#pragma once
#include "errno.h"
#include "crypto.h"
#include "interface.h"
#include <system_error>
#include <netinet/in.h>
#include <memory>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#define BUFFER_SIZE 1024
using namespace std;

/**
 * @class Connection
 * @brief Класс для управления сетевыми соединениями и аутентификацией
 * @details Обеспечивает установку TCP-соединения, аутентификацию клиентов 
 *          и обработку передаваемых данных
 */
class Connection{
private:

public:
    /**
     * @brief Установка соединения с клиентом
     * @param[in] p Параметры соединения (адрес, порт, файлы конфигурации)
     * @return 0 при успешном выполнении, 1 при ошибке аутентификации
     * @throw std::system_error при ошибках сетевого взаимодействия
     * @throw std::runtime_error при некорректных данных
     */
    static int establishConnection(const Params* p);
};