/**
 * @file main.cpp
 * @author Мезин А.А.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Главный модуль серверного приложения
 * @warning Учебный пример
 * @details Основная функция приложения, обрабатывает аргументы командной строки
 *          и запускает сервер для обработки подключений
 */

#include "connection.h"
#include "interface.h"

/**
 * @brief Точка входа в приложение
 * @param[in] argc Количество аргументов
 * @param[in] argv Массив аргументов
 * @return 0 при успешном выполнении, 1 при ошибке
 */
int main(int argc, const char** argv)
{
    UserInterface userinterface;
        if(!userinterface.Parser(argc, argv)){
            cout << userinterface.getDescription() << endl;
            return 1;
        }
    Params params = userinterface.getParams();
    Connection::establishConnection(&params);//&params
    return 0;
}