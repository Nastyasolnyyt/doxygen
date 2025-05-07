#include "reg_form.h"
#include <QApplication>
#include <client.h>
#include "notification.h"
#include "QValidator"

// Определяем алиас для класса Widget, чтобы избежать конфликта имен
#define window Widget
// Запрещаем использование имени Widget напрямую
#pragma GCC poison Widget

/**
 * @brief Точка входа в приложение
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код возврата приложения
 *
 * Основные действия:
 * 1. Создает QApplication - ядро Qt-приложения
 * 2. Инициализирует единственный экземпляр клиента (Singleton)
 * 3. Создает и отображает окно регистрации
 * 4. Запускает главный цикл обработки событий
 */
int main(int argc, char *argv[])
{
    // Инициализация Qt-приложения
    QApplication a(argc, argv);

    // Создание клиентского соединения (Singleton)
    Client* make_client = Client::get_instance();

    // Создание и отображение окна регистрации
    window* window_reg = new window(make_client);

    // Запуск главного цикла обработки событий
    return a.exec();
}
