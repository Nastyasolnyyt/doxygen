#include "client.h"
#include "clients_func.h"
#include <QMessageBox>
#include <QCryptographicHash>

extern QApplication a;

/// Инициализация статических членов класса
Client* Client::p_instance = nullptr;
Client* SingletonDestroyer::client_connection = nullptr;
QTcpSocket* SingletonDestroyer::socket = nullptr;
QTcpSocket* Client::socket = nullptr;
SingletonDestroyer Client::el = SingletonDestroyer();
int Client::port = 8080;

/**
 * @brief Инициализирует разрушитель синглтона
 * @param element Указатель на экземпляр клиента
 * @param socket Указатель на сокет соединения
 */
void SingletonDestroyer::initialize(Client* element, QTcpSocket* socket) {
    SingletonDestroyer::client_connection = element;
    SingletonDestroyer::socket = socket;
}

/**
 * @brief Конструктор разрушителя
 */
SingletonDestroyer::SingletonDestroyer() {}

/**
 * @brief Деструктор разрушителя
 *
 * Освобождает ресурсы клиента и сокета
 */
SingletonDestroyer::~SingletonDestroyer() {
    qDebug() << "Вызвался деструктор SingletonDestroyer";
    delete SingletonDestroyer::socket;
    delete SingletonDestroyer::client_connection;
}

/**
 * @brief Конструктор клиента
 *
 * Инициализирует соединение с сервером
 */
Client::Client()
{
    qDebug() << "Вызвался конструктор клиента";
    Client::socket = new QTcpSocket();
    // Подключаем сигналы состояния соединения
    connect(Client::socket, &QTcpSocket::connected, this, &Client::connect_to_server);
    connect(Client::socket, &QTcpSocket::disconnected, this, &Client::disconnect_from_server);
    // Устанавливаем соединение с сервером
    Client::socket->connectToHost("127.0.0.1", port);
}

/**
 * @brief Деструктор клиента
 */
Client::~Client() {
    qDebug() << "Вызвался деструктор клиента";
}

/**
 * @brief Возвращает единственный экземпляр клиента (Singleton)
 * @return Указатель на экземпляр Client
 */
Client* Client::get_instance() {
    if (Client::p_instance == nullptr) {
        Client::p_instance = new Client();
        SingletonDestroyer::initialize(Client::p_instance, Client::socket);
    }
    return Client::p_instance;
}

/**
 * @brief Обработчик успешного подключения к серверу
 */
void Client::connect_to_server() {
    // Настраиваем обработку входящих данных
    connect(this->socket, &QTcpSocket::readyRead, this, &Client::read);
}

/**
 * @brief Читает данные от сервера
 *
 * Обрабатывает входящие сообщения и генерирует соответствующие сигналы
 */
void Client::read() {
    QByteArray data;
    while (this->socket->bytesAvailable() > 0) {
        data.append(this->socket->readAll());
    }
    QString data_to_qstring = QString(data);

    // Обработка сообщений о регистрации
    if (data_to_qstring == "register|ok")
        emit this->register_ok();
    if (data_to_qstring == "register|error")
        emit this->register_error();

    // Обработка сообщений о авторизации
    if (data_to_qstring == "auth|ok")
        emit this->auth_ok();
    if (data_to_qstring == "auth|error")
        emit this->auth_error();

    // Обработка сообщений о сбросе пароля
    if (data_to_qstring == "reset|error")
        emit this->reset_error();

    // Обработка ответов на уравнения
    if (data_to_qstring.split("|")[0] == "answer") {
        QString answer = data_to_qstring.split("|")[1];
        if (answer != "error" and answer != "infinity_solutions" and answer != "no_solution")
            emit this->equation_ok(answer);
        else
            emit this->equation_fail(answer);
    }

    qDebug() << QString("%1 Server send: %2").arg(clients_func::get_client_time()).arg(data_to_qstring.simplified());
}

/**
 * @brief Отправляет сообщение серверу
 * @param text Текст сообщения
 * @return true если сообщение отправлено успешно, false в случае ошибки
 */
bool Client::write(QString text) {
    QByteArray data = text.toUtf8();
    if (this->socket->state() != QAbstractSocket::ConnectedState) {
        clients_func::create_messagebox("Ошибка", "Нет подключения к серверу, попробуйте перезапустить приложение");
        return false;
    }
    else {
        this->socket->write(data);
        return true;
    }
}

/**
 * @brief Обработчик отключения от сервера
 */
void Client::disconnect_from_server() {
    this->socket->close();
    qDebug() << QString("%1 Произошло отключение от сервера!").arg(clients_func::get_client_time());
}
