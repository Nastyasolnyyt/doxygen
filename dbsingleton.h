#ifndef DBSINGLETON_H
#define DBSINGLETON_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QObject>
#include <QDebug>
#include <QVariantList>
#include "functions_for_server.h"

class DBSingletonDestroyer; ///< Предварительное объявление класса-разрушителя

/**
 * @brief Класс Singleton для работы с базой данных
 *
 * Реализует паттерн Singleton для обеспечения единственного экземпляра
 * соединения с базой данных в приложении.
 */
class DBSingleton : public QObject {
    Q_OBJECT

private:
    static DBSingleton* instance;       ///< Единственный экземпляр класса
    static DBSingletonDestroyer destroyer; ///< Объект-разрушитель
    QSqlDatabase db;                    ///< Объект базы данных

    functions_for_server* servers_functions; ///< Указатель на вспомогательные функции сервера

    /**
     * @brief Приватный конструктор
     */
    DBSingleton();

    DBSingleton(const DBSingleton&) = delete; ///< Запрет копирования
    DBSingleton& operator=(const DBSingleton&) = delete; ///< Запрет присваивания

    friend class DBSingletonDestroyer;

public:
    /**
     * @brief Получение экземпляра Singleton
     * @return Указатель на единственный экземпляр класса
     */
    static DBSingleton* getInstance();

    /**
     * @brief Получение объекта базы данных
     * @return Объект QSqlDatabase
     */
    QSqlDatabase getDatabase();

    /**
     * @brief Деструктор
     */
    ~DBSingleton();

    /**
     * @brief Выполнение SQL-запроса
     * @param queryStr Строка с SQL-запросом
     * @return true если запрос выполнен успешно, false в противном случае
     */
    bool executeQuery(const QString& queryStr);

    /**
     * @brief Получение данных из базы
     * @param queryStr Строка с SQL-запросом
     * @return Список значений QVariantList с результатами запроса
     */
    QVariantList fetchData(const QString& queryStr);

signals:
    /// @name Сигналы регистрации
    /// @{
    /**
     * @brief Сигнал успешной регистрации
     */
    void register_ok();

    /**
     * @brief Сигнал ошибки при регистрации
     */
    void register_error();
    /// @}

    /// @name Сигналы авторизации
    /// @{
    /**
     * @brief Сигнал успешной авторизации
     */
    void auth_ok();

    /**
     * @brief Сигнал ошибки при авторизации
     */
    void auth_error();
    /// @}

    /// @name Сигналы сброса пароля
    /// @{
    /**
     * @brief Сигнал ошибки (логин не найден в БД)
     */
    void reset_error();

    /**
      * @brief Сигнал успешного сброса пароля
      */
    void reset_ok();
    /// @}

public slots:
    /// @name Слоты регистрации
    /// @{
    /**
     * @brief Слот регистрации нового аккаунта
     * @param login Логин
     * @param password Пароль
     * @param email Email
     * @param last_name Фамилия
     * @param first_name Имя
     * @param middle_name Отчество
     */
    void slot_register_new_account(QString login, QString password, QString email,
                                   QString last_name, QString first_name, QString middle_name);
    /// @}

    /// @name Слоты авторизации
    /// @{
    /**
     * @brief Слот авторизации
     * @param login Логин
     * @param password Пароль
     */
    void slot_auth(QString login, QString password);
    /// @}

    /// @name Слоты сброса пароля
    /// @{
    /**
     * @brief Слот отправки кода подтверждения
     * @param login Логин пользователя
     * @param code Код подтверждения
     */
    void slot_send_code(QString login, QString code);

    /**
     * @brief Слот установки нового пароля
     * @param login Логин пользователя
     * @param password Новый пароль
     */
    void slot_new_password(QString login, QString password);
    /// @}
};

/**
 * @brief Класс-разрушитель для корректного удаления Singleton
 */
class DBSingletonDestroyer {
private:
    DBSingleton* instance; ///< Указатель на экземпляр Singleton

public:
    /**
     * @brief Деструктор
     */
    ~DBSingletonDestroyer();

    /**
     * @brief Инициализация разрушителя
     * @param link Указатель на экземпляр Singleton
     */
    void initialize(DBSingleton* link);
};

#endif // DBSINGLETON_H
