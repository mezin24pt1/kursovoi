/**
 * @file interface.cpp
 * @author Мезин А.А.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация пользовательского интерфейса
 * @warning Учебный пример
 */

#include "interface.h"

/**
 * @brief Конструктор инициализирует параметры командной строки
 */
UserInterface::UserInterface() : desc("Allowed options")
{
    // добавление параметров в парсер командной строки
    desc.add_options()
    ("help,h", "Show help")
    ("log,l", po::value<string>(&params.logFile)->default_value("journal.txt"), "Set log file")
    ("base,b", po::value<std::string>(&params.inFileName)->required(),"Set input data base name")
    ("journal,j", po::value<std::string>(&params.inFileJournal)->required(),"Set journal file name")
    ("port,p", po::value<int>(&params.Port)->required(), "Set port")
    ("address,a", po::value<string>(&params.Address)->default_value("127.0.0.1"), "Set address");

}

/**
 * @brief Парсинг аргументов командной строки
 * @param[in] argc Количество аргументов
 * @param[in] argv Массив аргументов
 * @return true при успешном парсинге, false при запросе помощи
 * @throw boost::program_options::error при некорректных аргументах
 */
bool UserInterface::Parser(int argc, const char** argv)
{
    // Если нет аргументов или только --help, показываем справку
    if (argc == 1) {
        return false;
    }
    // парсинг
    po::store(po::parse_command_line(argc, argv, desc), vm);
    // обрабатываем --help до вызова notify
    if (vm.count("help"))
    return false;
    // присвоение значений по умолчанию и возбуждение исключений
    po::notify(vm);
    return true;
}

/**
 * @brief Генерация строки с описанием параметров
 * @return Форматированная строка с описанием всех опций
 */
std::string UserInterface::getDescription()
{
    std::ostringstream ss;
    ss << desc;
    return ss.str();
}
