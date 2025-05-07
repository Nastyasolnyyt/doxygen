#include "../include/mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include <QString>
#include <QThread>
#include "../include/client_object.h"

/// Статические члены класса
MyTcpServer* MyTcpServer::p_instance = nullptr;
MyTcpServerDestroyer MyTcpServerDestroyer::destroyer = MyTcpServerDestroyer();

/// Глобальные переменные
QList<client*> clients; ///< Список подключенных клиентов
functions_for_server* servers_functions = functions_for_server::get_instance(); ///< Функционал сервера

/**
 * @brief Инициализация разрушителя
 * @param server Указатель на экземпляр сервера
 * @param functions Указатель на функционал сервера
 */
void MyTcpServerDestroyer::initialize(MyTcpServer* server, functions_for_server* functions) {
    this->server = server;
    this->functions = functions;
}

/**
 * @brief Деструктор разрушителя
 *
 * Освобождает ресурсы сервера и его функционала
 */
MyTcpServerDestroyer::~MyTcpServerDestroyer() {
    delete this->server;
    delete this->functions;
}

/**
 * @brief Деструктор сервера
 *
 * Закрывает все соединения и освобождает ресурсы
 */
MyTcpServer::~MyTcpServer()
{
    // Удаление всех клиентов
    for (int i = 0; i < clients.size(); i++) {
        delete clients[i];
    }
    mTcpServer->close(); // Закрываем серверный сокет
    delete mTcpServer;
}

/**
 * @brief Конструктор сервера
 * @param parent Родительский объект
 *
 * Инициализирует TCP сервер и начинает прослушивание порта
 */
MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent) {
    mTcpServer = new QTcpServer(this); // Создаем экземпляр сервера

    // Настраиваем обработку новых подключений
    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    // Пытаемся запустить сервер на порту 8080
    if(!mTcpServer->listen(QHostAddress::Any, 8080)) {
        qDebug() << QString("%1 Сервер не запущен!").arg(servers_functions->get_server_time());
    } else {
        qDebug() << QString("%1 Сервер успешно запущен").arg(servers_functions->get_server_time());
    }
}

/**
 * @brief Создает или возвращает экземпляр сервера (реализация Singleton)
 * @return Указатель на экземпляр сервера
 */
MyTcpServer* MyTcpServer::create_instance() {
    if (MyTcpServer::p_instance == nullptr) {
        MyTcpServer::p_instance = new MyTcpServer;
        MyTcpServerDestroyer::destroyer.initialize(MyTcpServer::p_instance, functions_for_server::get_instance());
    }
    return MyTcpServer::p_instance;
}

/**
 * @brief Обработчик новых подключений
 *
 * Создает новый поток для каждого подключившегося клиента
 */
void MyTcpServer::slotNewConnection() {
    // Создаем новый поток для клиента
    QThread* new_thread_for_client = new QThread(this);

    // Получаем сокет нового клиента
    QTcpSocket* temp = this->mTcpServer->nextPendingConnection();

    // Создаем объект клиента
    client* client_object = new client(temp->socketDescriptor());

    // Переносим клиента в новый поток
    client_object->moveToThread(new_thread_for_client);

    // Настраиваем сигналы для управления жизненным циклом потока
    connect(client_object, &client::del_thread, new_thread_for_client, &QThread::quit);
    connect(new_thread_for_client, &QThread::finished, client_object, &QThread::deleteLater);

    // Запускаем поток
    new_thread_for_client->start();
}
