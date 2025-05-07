#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QByteArray>
#include <QObject>
#include <QString>

// Предварительное объявление класса Client
class Client;

/**
 * @brief Класс-разрушитель для управления временем жизни синглтона
 *
 * Обеспечивает корректное освобождение ресурсов клиентского соединения
 */
class SingletonDestroyer {
private:
    static Client* client_connection; ///< Указатель на экземпляр клиента
    static QTcpSocket* socket;        ///< Указатель на сокет соединения

public:
    /**
     * @brief Инициализация разрушителя
     * @param element Указатель на экземпляр клиента
     * @param socket Указатель на сокет соединения
     */
    static void initialize(Client* element, QTcpSocket* socket);

    /**
     * @brief Деструктор разрушителя
     */
    ~SingletonDestroyer();

    /**
     * @brief Конструктор разрушителя
     */
    SingletonDestroyer();
};

/**
 * @brief Класс клиентского соединения (реализация Singleton)
 *
 * Обеспечивает взаимодействие с сервером через TCP-соединение
 */
class Client: public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Отправляет сообщение серверу
     * @param text Текст сообщения
     * @return true если сообщение отправлено успешно, false в случае ошибки
     */
    bool write(QString text);

    /**
     * @brief Возвращает единственный экземпляр клиента
     * @return Указатель на экземпляр Client
     */
    static Client* get_instance();

    /**
     * @brief Деструктор клиента
     */
    ~Client();

private:
    static QTcpSocket* socket;    ///< Сокет для соединения с сервером
    static Client* p_instance;    ///< Единственный экземпляр клиента
    static int port;             ///< Порт для подключения

    /**
     * @brief Приватный конструктор
     */
    Client();

    static SingletonDestroyer el; ///< Объект-разрушитель для управления временем жизни

private slots:
    /**
     * @brief Устанавливает соединение с сервером
     */
    void connect_to_server();

    /**
     * @brief Разрывает соединение с сервером
     */
    void disconnect_from_server();

    /**
     * @brief Читает данные от сервера
     */
    void read();

signals:
    /// @name Сигналы регистрации
    /// @{
    /**
     * @brief Успешная регистрация
     */
    void register_ok();

    /**
     * @brief Ошибка регистрации (пользователь уже существует)
     */
    void register_error();
    /// @}

    /// @name Сигналы авторизации
    /// @{
    /**
     * @brief Успешная авторизация
     */
    void auth_ok();

    /**
     * @brief Ошибка авторизации
     */
    void auth_error();
    /// @}

    /// @name Сигналы окна сброса пароля
    /// @{
    /**
     * @brief Ошибка сброса пароля
     */
    void reset_error();
    /// @}

    /// @name Сигналы главного окна
    /// @{
    /**
     * @brief Успешное решение уравнения
     * @param Ответ сервера с решением
     */
    void equation_ok(QString&);

    /**
     * @brief Ошибка решения уравнения
     * @param Ответ сервера с описанием ошибки
     */
    void equation_fail(QString&);
    /// @}
};

#endif // CLIENT_H
