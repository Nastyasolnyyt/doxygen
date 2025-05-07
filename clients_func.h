#ifndef CLIENTS_FUNC_H
#define CLIENTS_FUNC_H

#include <QString>
#include <QDate>
#include <QMessageBox>
#include <QHBoxLayout>

/**
 * @brief Перечисление действий с элементами интерфейса
 */
enum class action {
    HIDE,  ///< Скрыть элемент
    SHOW,  ///< Показать элемент
};

/**
 * @brief Класс вспомогательных функций для клиентской части
 *
 * Содержит статические методы для работы с данными пользователя,
 * генерации случайных значений и управления интерфейсом
 */
class clients_func
{
private:
    clients_func() = delete;                 ///< Запрет создания экземпляров
    clients_func(const clients_func&) = delete; ///< Запрет копирования
    ~clients_func() = delete;                ///< Запрет удаления

public:
    /**
     * @brief Проверяет корректность логина
     * @param login Логин для проверки
     * @return true если логин корректен, иначе false
     */
    static bool current_login(QString login);

    /**
     * @brief Проверяет корректность пароля
     * @param password Пароль для проверки
     * @return true если пароль корректен, иначе false
     */
    static bool current_password(QString password);

    /**
     * @brief Проверяет корректность email
     * @param email Email для проверки
     * @return true если email корректен, иначе false
     */
    static bool current_email(QString email);

    /**
     * @brief Возвращает текущее время в строковом формате
     * @return Строка с текущей датой и временем
     */
    static QString get_client_time();

    /**
     * @brief Генерирует случайный пароль
     * @return Случайно сгенерированный пароль
     */
    static QString random_password();

    /**
     * @brief Генерирует случайный код подтверждения
     * @return Случайный 6-значный код
     */
    static int random_code();

    /**
     * @brief Управляет видимостью элемента уравнения
     * @param uravnenie Указатель на layout с уравнением
     * @param effect Действие (показать/скрыть)
     */
    static void equation(QHBoxLayout* uravnenie, action effect);

    /**
     * @brief Создает и отображает информационное окно
     * @param title Заголовок окна
     * @param message Текст сообщения
     */
    static void create_messagebox(QString title, QString message);

    /**
     * @brief Создает хеш строки
     * @param text Исходная строка
     * @return Хеш-строка
     */
    static QString create_hash(QString text);

private:
    /**
     * @brief Проверяет наличие только английских символов
     * @param text Строка для проверки
     * @return true если только английские символы, иначе false
     */
    static bool english_symbols(QString text);
};

#endif // CLIENTS_FUNC_H
