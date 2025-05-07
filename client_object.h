#ifndef CLIENT_OBJECT_H
#define CLIENT_OBJECT_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>

/**
 * @brief Класс клиента для обработки соединения и взаимодействия с сервером
 */
class client: public QObject
{
    Q_OBJECT
public:
    /**
    * @brief Конструктор клиента
    * @param client_description Дескриптор клиента
    * @param parent Родительский объект
    */
    client(qintptr client_description, QObject* parent = nullptr);

    /**
    * @brief Деструктор клиента
    */
    ~client();

public slots:

private slots:
    /**
    * @brief Инициализация каждого клиента
    */
    void initialization();

    /**
    * @brief Закрытие соединения с клиентом
    */
    void slot_close_connection();

    /**
    * @brief Чтение данных от клиента
    */
    void slot_read_from_client();

    /// @name Регистрация
    /// @{
    /**
    * @brief Отправка клиенту сообщения об успешной регистрации
    */
    void slot_register_ok();

    /**
    * @brief Отправка клиенту сообщения об ошибке при регистрации
    */
    void slot_register_error();
    /// @}

    /// @name Авторизация
    /// @{
    /**
    * @brief Отправка клиенту сообщения об успешной авторизации
    */
    void slot_auth_ok();

    /**
    * @brief Отправка клиенту сообщения об ошибке при авторизации
    */
    void slot_auth_error();
    /// @}

    /// @name Сброс пароля
    /// @{
    /**
    * @brief Отправка сообщения об ошибке при сбросе пароля (логин не найден в БД)
    */
    void slot_reset_error();

    /**
    * @brief Отправка сообщения об успешном сбросе пароля
    */
    void slot_reset_ok();
    /// @}

    /// @name Главное клиентское окно
    /// @{
    /**
    * @brief Отправка клиенту решения уравнения
    * @param answer Ответ с решением уравнения
    */
    void slot_equation_solution(QString answer);
    /// @}

signals:
    /**
    * @brief Сигнал завершения работы потока клиента
    */
    void finished();

    /// @name Сигналы для регистрации
    /// @{
    /**
    * @brief Сигнал регистрации нового аккаунта
    * @param login Логин
    * @param password Пароль
    * @param email Email
    * @param last_name Фамилия
    * @param first_name Имя
    * @param middle_name Отчество
    */
    void signal_register_new_account(QString login, QString password, QString email, QString last_name, QString first_name, QString middle_name);
    /// @}

    /// @name Сигналы для авторизации
    /// @{
    /**
    * @brief Сигнал авторизации
    * @param login Логин
    * @param password Пароль
    */
    void signal_auth(QString login, QString password);
    /// @}

    /// @name Сброс пароля
    /// @{
    /**
    * @brief Сигнал отправки кода на email клиента
    * @param email Email клиента
    * @param code Код подтверждения
    */
    void signal_send_code_to_email(QString email, QString code);

    /**
    * @brief Сигнал установки нового пароля
    * @param email Email клиента
    * @param password Новый пароль
    */
    void signal_set_new_password(QString email, QString password);
    /// @}

    /// @name Главное клиентское окно
    /// @{
    /**
    * @brief Сигнал решения линейного уравнения
    * @param a Коэффициент a
    * @param b Коэффициент b
    */
    void signal_linear_equation(QString a, QString b);

    /**
    * @brief Сигнал решения квадратного уравнения
    * @param a Коэффициент a
    * @param b Коэффициент b
    * @param c Коэффициент c
    */
    void signal_quadratic_equation(QString a, QString b, QString c);
    /// @}

    /**
    * @brief Сигнал удаления потока
    */
    void del_thread();

private:
    QTcpSocket* client_socket;       ///< Сокет клиента
    qintptr client_description;      ///< Дескриптор клиента
    QThread* thread_for_client;      ///< Поток для клиента

    /**
    * @brief Отправка приветственного сообщения в консоль при новом подключении
    */
    static void hello_message();

    /**
    * @brief Отправка сообщения в консоль при отключении клиента
    */
    static void bye_message();
};

#endif // CLIENT_OBJECT_H
