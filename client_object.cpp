#include "../include/client_object.h"
#include "../include/functions_for_server.h"
#include <QList>
#include <QByteArray>
#include "../include/dbsingleton.h"

extern QList<client*> clients;              ///< Глобальный список подключенных клиентов
extern functions_for_server* servers_functions; ///< Глобальный экземпляр функций сервера

/**
 * @brief Конструктор класса client
 * @param client_description Дескриптор сокета клиента
 * @param parent Родительский QObject
 */
client::client(qintptr client_description, QObject* parent): QObject(parent), client_description(client_description)
{
    initialization();
}

/**
 * @brief Деструктор класса client
 *
 * Очищает ресурсы клиента, удаляет из глобального списка,
 * закрывает сокет и отправляет сигнал удаления потока
 */
client::~client() {
    qDebug() << "Деструктор клиента успешно вызван!";
    clients.removeAll(this);
    this->client_socket->close();
    this->bye_message();
    emit this->del_thread();
}

/**
 * @brief Инициализация подключения клиента
 *
 * Настраивает сокет, добавляет клиента в глобальный список,
 * устанавливает signal-slot соединения для:
 * - Чтения данных
 * - Регистрации
 * - Авторизации
 * - Сброса пароля
 * - Решения уравнений
 */
void client::initialization() {
    client_socket = new QTcpSocket(this);
    client_socket->setSocketDescriptor(client_description);
    clients.push_back(this);
    hello_message();

    // Базовые соединения сокета
    connect(client_socket, &QTcpSocket::readyRead, this, &client::slot_read_from_client);
    connect(client_socket, &QTcpSocket::disconnected, this, &client::slot_close_connection);

    // Соединения для регистрации
    connect(this, &client::signal_register_new_account,
            DBSingleton::getInstance(), &DBSingleton::slot_register_new_account);
    connect(DBSingleton::getInstance(), &DBSingleton::register_ok,
            this, &client::slot_register_ok);
    connect(DBSingleton::getInstance(), &DBSingleton::register_error,
            this, &client::slot_register_error);

    // Соединения для авторизации
    connect(this, &client::signal_auth,
            DBSingleton::getInstance(), &DBSingleton::slot_auth);
    connect(DBSingleton::getInstance(), &DBSingleton::auth_ok,
            this, &client::slot_auth_ok);
    connect(DBSingleton::getInstance(), &DBSingleton::auth_error,
            this, &client::slot_auth_error);

    // Соединения для сброса пароля
    connect(this, &client::signal_send_code_to_email,
            DBSingleton::getInstance(), &DBSingleton::slot_send_code);
    connect(DBSingleton::getInstance(), &DBSingleton::reset_error,
            this, &client::slot_reset_error);
    connect(this, &client::signal_set_new_password,
            DBSingleton::getInstance(), &DBSingleton::slot_new_password);

    // Соединения для решения уравнений
    connect(this, &client::signal_linear_equation,
            servers_functions, &functions_for_server::slot_linear_equation);
    connect(this, &client::signal_quadratic_equation,
            servers_functions, &functions_for_server::slot_quadratic_equation);
    connect(servers_functions, &functions_for_server::signal_equation_solution,
            this, &client::slot_equation_solution);
}

/**
 * @brief Обработка входящих данных от клиента
 *
 * Обрабатывает различные действия клиента:
 * - Регистрация ("reg")
 * - Авторизация ("login")
 * - Сброс пароля ("reset", "new_password")
 * - Решение уравнений ("equation")
 */
void client::slot_read_from_client() {
    qDebug() << "Сработал " << Q_FUNC_INFO << " . Текущий поток - " << QThread::currentThreadId();
    QString data;
    while (client_socket->bytesAvailable()) {
        data.push_back(client_socket->readAll());
    }

    QString action = data.split("|")[0];
    QString clients_data = data.split("|")[1];

    // Обработка регистрации
    if (action == "reg") {
        QStringList clients_data_list = clients_data.split("$");
        emit signal_register_new_account(
            clients_data_list[0], // логин
            clients_data_list[1], // пароль
            clients_data_list[2], // email
            clients_data_list[3], // фамилия
            clients_data_list[4], // имя
            clients_data_list[5]  // отчество
            );
    }

    // Обработка авторизации
    if (action == "login") {
        QString login = clients_data.split("$")[0];
        QString password = clients_data.split("$")[1];
        emit this->signal_auth(login, password);
    }

    // Обработка сброса пароля
    if (action == "reset") {
        QString email = clients_data.split("$")[0];
        QString code = clients_data.split("$")[1];
        emit signal_send_code_to_email(email, code);
    }
    if (action == "new_password") {
        QString login = clients_data.split("$")[0];
        QString new_password = clients_data.split("$")[1];
        emit signal_set_new_password(login, new_password);
    }

    // Обработка решения уравнений
    if (action == "equation") {
        QString type_equation = data.split("|")[1];
        if (type_equation == QString("linear")) {
            QStringList List_with_koef = data.split("|")[2].split("$");
            emit this->signal_linear_equation(List_with_koef[0], List_with_koef[1]);
        }
        if (type_equation == "quadratic") {
            QStringList List_with_koef = data.split("|")[2].split("$");
            emit this->signal_quadratic_equation(
                List_with_koef[0], // a
                List_with_koef[1], // b
                List_with_koef[2]  // c
                );
        }
    }

    qDebug() << QString("%1 Клиент ").arg(servers_functions->get_server_time())
             << &client_socket
             << QString(" отправил сообщение: %1").arg(QString(data)).simplified();
}

/**
 * @brief Обработка отключения клиента
 */
void client::slot_close_connection() {
    this->client_socket->close();
    this->deleteLater();
}

/// @name Обработчики ответов
/// @{
/**
 * @brief Обработка успешной регистрации
 */
void client::slot_register_ok() {
    this->client_socket->write("register|ok");
}

/**
 * @brief Обработка ошибки регистрации
 */
void client::slot_register_error() {
    this->client_socket->write("register|error");
}

/**
 * @brief Обработка успешной авторизации
 */
void client::slot_auth_ok() {
    this->client_socket->write("auth|ok");
}

/**
 * @brief Обработка ошибки авторизации
 */
void client::slot_auth_error() {
    this->client_socket->write("auth|error");
}

/**
 * @brief Обработка ошибки сброса пароля
 */
void client::slot_reset_error() {
    this->client_socket->write("reset|error");
}

/**
 * @brief Обработка успешного сброса пароля
 */
void client::slot_reset_ok() {
    this->client_socket->write("reset|ok");
}

/**
 * @brief Отправка решения уравнения клиенту
 * @param answer Решение для отправки
 */
void client::slot_equation_solution(QString answer) {
    this->client_socket->write(answer.toUtf8());
}
/// @}

/// @name Служебные функции
/// @{
/**
 * @brief Логирование сообщения о новом подключении
 */
void client::hello_message() {
    if (clients.size() == 1) {
        qDebug() << QString("%1 Клиент подключился. Сейчас подключен 1 сокет")
                        .arg(servers_functions->get_server_time());
    }
    else if (clients.size() > 1) {
        qDebug() << QString("%1 Клиент подключился. Сейчас подключено %2 сокета")
                        .arg(servers_functions->get_server_time())
                        .arg(QString::number(clients.size()));
    }
}

/**
 * @brief Логирование сообщения об отключении
 */
void client::bye_message() {
    if (clients.size() == 0)
        qDebug() << QString("%1 Клиент отключился. Нет подключенных клиентов")
                        .arg(servers_functions->get_server_time());
    else if (clients.size() == 1)
        qDebug() << QString("%1 Клиент отключился. Сейчас подключен 1 сокет")
                        .arg(servers_functions->get_server_time());
    else if (clients.size() > 1)
        qDebug() << QString("%1 Клиент отключился. Сейчас подключено %2 сокета")
                        .arg(servers_functions->get_server_time())
                        .arg(QString::number(clients.size()));
}
/// @}
