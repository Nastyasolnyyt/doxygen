#ifndef AUTH_FORM_H
#define AUTH_FORM_H

#include <QWidget>
#include "client.h"
#include <QCloseEvent>

// Предварительные объявления классов
class reset_password; ///< Класс окна сброса пароля
class Widget;        ///< Класс окна регистрации
class client_main_window; ///< Класс главного окна клиента

namespace Ui {
class auth_form;
}

/**
 * @brief Класс формы авторизации
 *
 * Предоставляет интерфейс для авторизации пользователей,
 * перехода к регистрации и сбросу пароля
 */
class auth_form : public QWidget
{
    Q_OBJECT

public:
    /**
    * @brief Конструктор формы авторизации
    * @param client_socket Указатель на клиентский сокет
    * @param parent Родительский виджет
    */
    explicit auth_form(Client* client_socket, QWidget *parent = nullptr);

    /**
    * @brief Деструктор формы авторизации
    */
    ~auth_form();

private slots:
    /**
    * @brief Слот для обработки нажатия кнопки авторизации
    */
    void on_pushButton_login_clicked();

    /**
    * @brief Слот для обработки нажатия кнопки сброса пароля
    */
    void on_pushButton_reset_password_clicked();

    /**
    * @brief Слот для обработки нажатия кнопки перехода к регистрации
    */
    void on_pushButton_to_reg_clicked();

    /// @name Слоты авторизации
    /// @{
    /**
    * @brief Слот успешной авторизации
    */
    void auth_ok();

    /**
    * @brief Слот ошибки авторизации
    */
    void auth_error();
    /// @}

    /**
    * @brief Слот отображения пароля при нажатии кнопки
    */
    void on_pushButton_draw_password_pressed();

    /**
    * @brief Слот скрытия пароля при отпускании кнопки
    */
    void on_pushButton_draw_password_released();

private:
    Ui::auth_form *ui; ///< Указатель на UI форму
    Client* client = nullptr; ///< Указатель на клиентский сокет

    /**
    * @brief Создает уведомление
    * @param title Заголовок уведомления
    * @param text Текст уведомления
    */
    void create_notification(QString title, QString text);

    /**
    * @brief Записывает данные авторизации в кэш
    */
    void write_info_in_cache();

    /**
    * @brief Заполняет форму данными из JSON-кэша
    */
    void fill_from_json();
};

#endif // AUTH_FORM_H
