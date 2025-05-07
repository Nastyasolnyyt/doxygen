#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QtNetwork>
#include <QByteArray>
#include <QDebug>
#include <QList>
#include "functions_for_server.h"

// Предварительное объявление класса MyTcpServer
class MyTcpServer;

/**
 * @brief Вспомогательный класс для корректного удаления синглтона
 *
 * Обеспечивает правильное освобождение ресурсов экземпляра MyTcpServer
 */
class MyTcpServerDestroyer {
public:
    static MyTcpServerDestroyer destroyer;  ///< Единственный экземпляр разрушителя

    /**
     * @brief Инициализация разрушителя
     * @param server Указатель на экземпляр MyTcpServer
     * @param functions Указатель на functions_for_server
     */
    void initialize(MyTcpServer* server, functions_for_server* functions);

    /**
     * @brief Деструктор
     */
    ~MyTcpServerDestroyer();

private:
    MyTcpServer* server;                   ///< Управляемый экземпляр сервера
    functions_for_server* functions;       ///< Функционал сервера
};

/**
 * @brief TCP сервер, реализованный как синглтон
 *
 * Обрабатывает входящие TCP-подключения и управляет клиентскими сокетами
 */
class MyTcpServer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Получение или создание экземпляра сервера
     * @return Указатель на единственный экземпляр
     */
    static MyTcpServer* create_instance();

    /**
     * @brief Деструктор
     */
    ~MyTcpServer();

private slots:
    /**
     * @brief Слот для обработки новых подключений
     */
    void slotNewConnection();

private:
    static MyTcpServer* p_instance;        ///< Указатель на экземпляр синглтона
    QTcpServer* mTcpServer;               ///< Экземпляр QTcpServer
    QTcpSocket* temp;                     ///< Временное хранилище сокета

    /**
     * @brief Приватный конструктор
     * @param parent Родительский QObject
     */
    explicit MyTcpServer(QObject* parent = nullptr);

    MyTcpServer(const MyTcpServer&) = delete;  ///< Запрет копирования
};

#endif // MYTCPSERVER_H
