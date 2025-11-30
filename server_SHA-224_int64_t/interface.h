/**
 * @file interface.h
 * @author Мезин А.А.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для пользовательского интерфейса
 * @warning Учебный пример
 */

#pragma once
#include <boost/program_options.hpp>
#include <string>
#include <sstream>
using namespace std;
namespace po = boost::program_options;

/**
 * @struct Params
 * @brief Структура для хранения параметров командной строки
 */
struct Params {
    string inFileName;      ///< Имя файла с учетными данными
    string inFileJournal;   ///< Имя файла журнала
    string inFileData;      ///< Имя файла с данными
    string logFile;         ///< Имя файла лога
    int Port;               ///< Порт для подключения
    string Address;         ///< Адрес для подключения
};

/**
 * @class UserInterface
 * @brief Класс для обработки параметров командной строки
 * @details Использует boost::program_options для парсинга аргументов
 */
class UserInterface {
private:
    po::options_description desc;   ///< Описание параметров
    po::variables_map vm;           ///< Результаты парсинга
    Params params;                  ///< Параметры приложения

public:
    /**
     * @brief Конструктор инициализирует опции командной строки
     */
    UserInterface();
    
    /**
     * @brief Парсинг аргументов командной строки
     * @param[in] argc Количество аргументов
     * @param[in] argv Массив аргументов
     * @return true при успешном парсинге, false при запросе помощи или ошибке
     */
    bool Parser(int argc, const char** argv);
    
    /**
     * @brief Получение описания параметров
     * @return Строка с описанием всех параметров
     */
    string getDescription();
    
    /**
     * @brief Получение параметров приложения
     * @return Структура с параметрами
     */
    Params getParams() {
        return params;
    };
};