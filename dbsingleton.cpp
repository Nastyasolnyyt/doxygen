#include "../include/dbsingleton.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

/// Статические члены класса
DBSingleton* DBSingleton::instance = nullptr;
DBSingletonDestroyer DBSingleton::destroyer = DBSingletonDestroyer();

/**
 * @brief Конструктор класса DBSingleton
 *
 * Инициализирует подключение к SQLite базе данных и функционал сервера
 */
DBSingleton::DBSingleton() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName("./tmp.db");
    this->servers_functions = functions_for_server::get_instance();

    if (!db.open()) {
        qDebug() << "Ошибка: Не удалось подключиться к базе данных.";
    } else {
        qDebug() << "База данных успешно подключена.";
        qDebug() << "Таблицы в базе: " << db.tables();
    }
}

/**
 * @brief Выполняет SQL-запрос
 * @param queryStr Строка с SQL-запросом
 * @return true если запрос выполнен успешно, false в случае ошибки
 */
bool DBSingleton::executeQuery(const QString& queryStr) {
    if (!db.open()) {
        qDebug() << "База данных не открыта.";
        return false;
    }
    QSqlQuery query;
    if (!query.exec(queryStr)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return false;
    }
    return true;
}

/// @name Регистрация пользователей
/// @{
/**
 * @brief Регистрирует нового пользователя
 * @param login Логин пользователя
 * @param password Пароль пользователя
 * @param email Email пользователя
 * @param last_name Фамилия пользователя
 * @param first_name Имя пользователя
 * @param middle_name Отчество пользователя
 *
 * Создает таблицу users если она не существует и добавляет нового пользователя
 */
void DBSingleton::slot_register_new_account(QString login, QString password, QString email,
                                            QString last_name, QString first_name, QString middle_name)
{
    QSqlQuery query;
    // Создаем таблицу если она не существует
    if (!query.exec("CREATE TABLE IF NOT EXISTS students ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "login TEXT UNIQUE, "
                    "hash TEXT, "
                    "email TEXT UNIQUE, "
                    "name TEXT, "
                    "surname TEXT, "
                    "middle_name TEXT)")) {
        qDebug() << "Ошибка создания таблицы:" << query.lastError().text();
        emit this->register_error();
        return;
    }

    // Проверяем существование пользователя
    QString checkQuery = QString("SELECT COUNT(*) FROM students WHERE login = '%1' OR email = '%2'")
                             .arg(login).arg(email);
    QVariantList result = fetchData(checkQuery);

    if (!result.isEmpty() && result.first().toInt() > 0) {
        emit this->register_error();
        return;
    }

    // Добавляем нового пользователя
    query.prepare("INSERT INTO students (login, hash, email, name, surname, middle_name) "
                  "VALUES (:login, :password, :email, :name, :surname, :middle_name)");
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    query.bindValue(":email", email);
    query.bindValue(":name", first_name);
    query.bindValue(":surname", last_name);
    query.bindValue(":middle_name", middle_name);

    if (query.exec()) {
        emit this->register_ok();
    } else {
        qDebug() << "Ошибка добавления пользователя:" << query.lastError().text();
        emit this->register_error();
    }
}
/// @}

/// @name Аутентификация пользователей
/// @{
/**
 * @brief Аутентифицирует пользователя
 * @param login Логин пользователя
 * @param password Пароль пользователя
 *
 * Проверяет соответствие логина и пароля в базе данных
 */
void DBSingleton::slot_auth(QString login, QString password) {
    QString selectQuery = QString("SELECT COUNT(*) FROM students WHERE login = '%1' AND hash = '%2'")
    .arg(login).arg(password);
    QVariantList result = fetchData(selectQuery);

    if (!result.isEmpty() && result.first().toInt() > 0) {
        emit this->auth_ok();
    } else {
        emit this->auth_error();
    }
}
/// @}

/// @name Сброс пароля
/// @{
/**
 * @brief Отправляет код подтверждения на email
 * @param login Логин пользователя
 * @param code Код подтверждения
 */
void DBSingleton::slot_send_code(QString login, QString code) {
    QString checkQuery = QString("SELECT email FROM students WHERE login = '%1'").arg(login);
    QVariantList result = fetchData(checkQuery);

    if (!result.isEmpty()) {
        QString email = result[0].toString();
        this->servers_functions->send_email_to_client(email, code);
    } else {
        emit this->reset_error();
    }
}

/**
 * @brief Устанавливает новый пароль пользователя
 * @param login Логин пользователя
 * @param password Новый пароль
 */
void DBSingleton::slot_new_password(QString login, QString password) {
    QString checkQuery = QString("SELECT COUNT(*) FROM students WHERE login = '%1'").arg(login);
    QVariantList result = fetchData(checkQuery);

    if (!result.isEmpty() && result.first().toInt() > 0) {
        QString updateQuery = QString("UPDATE students SET hash = '%1' WHERE login = '%2'")
        .arg(password).arg(login);

        if (executeQuery(updateQuery)) {
            emit this->reset_ok();
        } else {
            emit this->reset_error();
        }
    } else {
        emit this->reset_error();
    }
}
/// @}

/**
 * @brief Получает данные из базы данных
 * @param queryStr SQL-запрос
 * @return Список результатов запроса
 */
QVariantList DBSingleton::fetchData(const QString& queryStr) {
    if (!db.isOpen()) {
        qDebug() << "База данных не открыта.";
        return QVariantList();
    }

    QVariantList results;
    QSqlQuery query;

    if (query.exec(queryStr)) {
        while (query.next()) {
            QVariantList row;
            for (int i = 0; i <= 6; ++i) {
                row.append(query.value(i));
            }
            results.append(row);
        }
    } else {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
    }
    return results;
}

/**
 * @brief Деструктор класса DBSingleton
 *
 * Закрывает соединение с базой данных
 */
DBSingleton::~DBSingleton() {
    db.close();
    qDebug() << "Соединение с базой данных закрыто.";
}

/**
 * @brief Возвращает экземпляр синглтона
 * @return Указатель на экземпляр DBSingleton
 */
DBSingleton* DBSingleton::getInstance() {
    if (!instance) {
        instance = new DBSingleton();
        destroyer.initialize(instance);
    }
    return instance;
}

/**
 * @brief Возвращает объект базы данных
 * @return Объект QSqlDatabase
 */
QSqlDatabase DBSingleton::getDatabase() {
    return db;
}

/**
 * @brief Деструктор класса DBSingletonDestroyer
 *
 * Удаляет экземпляр синглтона
 */
DBSingletonDestroyer::~DBSingletonDestroyer() {
    delete instance;
}

/**
 * @brief Инициализирует ссылку на экземпляр
 * @param link Указатель на экземпляр DBSingleton
 */
void DBSingletonDestroyer::initialize(DBSingleton* link) {
    this->instance = link;
}
