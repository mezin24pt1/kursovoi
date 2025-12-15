/**
 * @file connection.cpp
 * @author Мезин А.А.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация сетевого соединения и аутентификации
 * @warning Учебный пример
 */

#include "connection.h"
#include "log.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <system_error>

/**
 * @brief Поиск пользователя в файле учетных записей
 * @param[in] filename Имя файла с пользователями
 * @param[in] username Логин для поиска
 * @param[out] password Найденный пароль
 * @return true если пользователь найден, false если нет или ошибка
 */
bool findUserInCredentialsFile(const std::string& filename, const std::string& username, std::string& password) {
    std::ifstream cred_file(filename);
    if (!cred_file) {
        std::cerr << "Не удалось открыть файл учетных записей: " << filename << std::endl;
        return false;
    }
    
    std::string credentials_line;
    while (std::getline(cred_file, credentials_line)) {
        // Игнорируем пустые строки и комментарии
        if (credentials_line.empty() || credentials_line[0] == '#' || credentials_line[0] == ';') {
            continue;
        }
        
        size_t separator_pos = credentials_line.find(':');
        if (separator_pos == std::string::npos) {
            continue; // Пропускаем некорректные строки
        }
        
        std::string stored_username = credentials_line.substr(0, separator_pos);
        std::string stored_password = credentials_line.substr(separator_pos + 1);
        
        // Убираем лишние пробелы
        stored_username.erase(0, stored_username.find_first_not_of(" \t"));
        stored_username.erase(stored_username.find_last_not_of(" \t") + 1);
        stored_password.erase(0, stored_password.find_first_not_of(" \t"));
        stored_password.erase(stored_password.find_last_not_of(" \t") + 1);
        
        // Сравниваем логины
        if (stored_username == username) {
            password = stored_password;
            cred_file.close();
            return true;
        }
    }
    
    cred_file.close();
    return false;
}

/**
 * @brief Установка соединения и обработка клиентских запросов
 * @param[in] connection_params Параметры соединения
 * @return 0 при успехе, 1 при ошибке аутентификации
 * @throw std::system_error при сетевых ошибках
 * @throw std::runtime_error при некорректных данных
 * @details Выполняет полный цикл работы сервера: создание сокета, привязку,
 *          прослушивание, аутентификацию клиента и обработку данных
 */

int Connection::establishConnection(const Params* connection_params){
    
    // Создаем сокет
    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1){       
        std::string error_message = "Сбой создания сокета: " + std::string(strerror(errno));
        logError(connection_params->logFile, error_message);
        throw std::system_error(errno, std::generic_category()); 
    }

    // Настраиваем адрес сервера
    std::unique_ptr<sockaddr_in> server_address(new sockaddr_in); 
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(connection_params->Port);
    server_address->sin_addr.s_addr = inet_addr(connection_params->Address.c_str());

    // Привязываем сокет
    int bind_result = bind(server_socket_fd, reinterpret_cast<const sockaddr*>(server_address.get()), sizeof(sockaddr_in));
    if (bind_result == -1) {
        std::string error_message = "Ошибка привязки сокета: " + std::string(strerror(errno));
        logError(connection_params->logFile, error_message);
        close(server_socket_fd);
        throw std::system_error(errno, std::generic_category());
    }
    
    // Переводим сокет в режим прослушивания
    int listen_result = listen(server_socket_fd, 5);
    if (listen_result == -1) {
        std::string error_message = "Сбой перевода в режим прослушивания: " + std::string(strerror(errno));
        logError(connection_params->logFile, error_message);
        close(server_socket_fd);
        throw std::system_error(errno, std::generic_category());
    }

    std::cout << "Сервер ожидает подключений на " << connection_params->Address << ":" << connection_params->Port << std::endl;

    // Принимаем входящее соединение
    sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    int client_socket_fd = accept(server_socket_fd, reinterpret_cast<sockaddr*>(&client_address), &client_address_length);
    if (client_socket_fd == -1) {
        std::string error_message = "Ошибка принятия соединения: " + std::string(strerror(errno));
        logError(connection_params->logFile, error_message);
        close(server_socket_fd);
        throw std::system_error(errno, std::generic_category());
    }

    std::cout << "Клиент подключен" << std::endl;

    // Получаем данные аутентификации
    char auth_buffer[BUFFER_SIZE];
    ssize_t auth_bytes_received = recv(client_socket_fd, auth_buffer, BUFFER_SIZE - 1, 0);
    if (auth_bytes_received == -1){
        std::string error_message = "Сбой получения данных аутентификации: " + std::string(strerror(errno));
        logError(connection_params->logFile, error_message);
        close(client_socket_fd);
        close(server_socket_fd);
        throw std::system_error(errno, std::generic_category());
    }

    if (auth_bytes_received > 0) {
        auth_buffer[auth_bytes_received] = '\0';
        std::cout << "Получены данные от клиента: " << auth_buffer << std::endl;
    }

    // Анализируем полученное сообщение
    std::string auth_message(auth_buffer);
    
    // Проверяем корректность формата сообщения
    if (auth_message.length() < 20) {
        std::string error_message = "Некорректный формат аутентификационного сообщения";
        logError(connection_params->logFile, error_message);
        close(client_socket_fd);
        close(server_socket_fd);
        throw std::runtime_error(error_message);
    }
    
    std::string client_username = auth_message.substr(0, 4);   // "user"
    std::string client_salt_value = auth_message.substr(4, 16); // соль
    std::string client_hash_value = auth_message.substr(20);    // хеш пароля

    ssize_t bytes_sent;
    std::string response_message;

    // Проверяем существование пользователя
    std::string user_credentials;
    if (!findUserInCredentialsFile(connection_params->inFileName, client_username, user_credentials)) {
        std::string error_message = "Учетная запись не обнаружена: " + client_username;
        logError(connection_params->logFile, error_message);
        
        response_message = "ERR_USER_NOT_FOUND";
        bytes_sent = send(client_socket_fd, response_message.c_str(), response_message.length(), 0);
        
        close(client_socket_fd);
        close(server_socket_fd);
        return 1;
    }

    std::cout << "Обнаружена учетная запись: " << client_username << std::endl;

    // Вычисляем хеш на стороне сервера
    std::string server_hash_value = auth(client_salt_value, user_credentials);
    std::cout << "Хеш сервера: " << server_hash_value << std::endl;
    std::cout << "Хеш клиента: " << client_hash_value << std::endl;

    // Проверяем соответствие хешей
    if(client_hash_value == server_hash_value){
        response_message = "OK";
        std::cout << "Проверка подлинности успешна" << std::endl;
    } else {
        response_message = "ERR";
        std::string error_message = "Сбой аутентификации: неверный хеш для пользователя " + client_username;
        logError(connection_params->logFile, error_message);
    }
    
    bytes_sent = send(client_socket_fd, response_message.c_str(), response_message.length(), 0);
    if (bytes_sent == -1) {
        std::string error_message = "Ошибка отправки результата аутентификации: " + std::string(strerror(errno));
        logError(connection_params->logFile, error_message);
        close(client_socket_fd);
        close(server_socket_fd);
        throw std::system_error(errno, std::generic_category());
    }

    // Завершаем соединение при неудачной аутентификации
    if (response_message != "OK") {
        close(client_socket_fd);
        close(server_socket_fd);
        return 1;
    }

    // Обрабатываем передаваемые данные
    uint32_t total_vectors;
    ssize_t bytes_received_total = 0;
    char* data_buffer = reinterpret_cast<char*>(&total_vectors);

    // 1. Получаем количество векторов
    while (bytes_received_total < sizeof(total_vectors)) {
        ssize_t current_received = recv(client_socket_fd, data_buffer + bytes_received_total, 
                                      sizeof(total_vectors) - bytes_received_total, 0);
        if (current_received <= 0) {
            std::string error_message = "Сбой получения количества векторов: " + std::string(strerror(errno));
            logError(connection_params->logFile, error_message);
            close(client_socket_fd);
            close(server_socket_fd);
            throw std::system_error(errno, std::generic_category());
        }
        bytes_received_total += current_received;
    }
    std::cout << "Получено векторов для обработки: " << total_vectors << std::endl;

    // Обрабатываем каждый вектор
    for (uint32_t current_vector = 0; current_vector < total_vectors; current_vector++) {
        // 2. Получаем размер текущего вектора
        uint32_t vector_length;
        bytes_received_total = 0;
        data_buffer = reinterpret_cast<char*>(&vector_length);
        
        while (bytes_received_total < sizeof(vector_length)) {
            ssize_t current_received = recv(client_socket_fd, data_buffer + bytes_received_total, 
                                          sizeof(vector_length) - bytes_received_total, 0);
            if (current_received <= 0) {
                std::string error_message = "Ошибка получения размера вектора " + 
                                          std::to_string(current_vector) + ": " + std::string(strerror(errno));
                logError(connection_params->logFile, error_message);
                close(client_socket_fd);
                close(server_socket_fd);
                throw std::system_error(errno, std::generic_category());
            }
            bytes_received_total += current_received;
        }
        std::cout << "Вектор #" << current_vector << ", элементов: " << vector_length << std::endl;

        // 3. Получаем элементы вектора и вычисляем произведение
        int64_t multiplication_result = 1; 
        for (uint32_t element_index = 0; element_index < vector_length; element_index++) {
            int64_t vector_element;
            bytes_received_total = 0;
            data_buffer = reinterpret_cast<char*>(&vector_element);
            
            while (bytes_received_total < sizeof(vector_element)) {
                ssize_t current_received = recv(client_socket_fd, data_buffer + bytes_received_total, 
                                              sizeof(vector_element) - bytes_received_total, 0);
                if (current_received <= 0) {
                    std::string error_message = "Сбой получения элемента " + std::to_string(element_index) + 
                                              " вектора " + std::to_string(current_vector) + ": " + 
                                              std::string(strerror(errno));
                    logError(connection_params->logFile, error_message);
                    close(client_socket_fd);
                    close(server_socket_fd);
                    throw std::system_error(errno, std::generic_category());
                }
                bytes_received_total += current_received;
            }
            
            std::cout << "  Элемент [" << element_index << "]: " << vector_element << std::endl;
            multiplication_result = multiplication_result * vector_element;
        }
        
        std::cout << "Результат умножения: " << multiplication_result << std::endl;
        ssize_t send_status = send(client_socket_fd, &multiplication_result, sizeof(multiplication_result), 0);
        if (send_status == -1) {
            std::string error_message = "Ошибка отправки результата для вектора " + 
                                      std::to_string(current_vector) + ": " + std::string(strerror(errno));
            logError(connection_params->logFile, error_message);
            close(client_socket_fd);
            close(server_socket_fd);
            throw std::system_error(errno, std::generic_category());
        }
    }
    
    std::cout << "Обработка завершена, закрытие соединений" << std::endl;
    close(client_socket_fd);
    close(server_socket_fd);
    return 0;
};