#ifndef RESET_PASSWORD_H
#define RESET_PASSWORD_H

#include <QWidget>
#include "client.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QThread>

// Предварительные объявления классов
class Widget;     ///< Класс формы регистрации
class auth_form;  ///< Класс формы авторизации

namespace Ui {
class reset_password;
}

/**
 * @brief Класс формы сброса пароля
 *
 * Предоставляет функционал для восстановления пароля пользователя
 * через отправку кода подтверждения на email
 */
class reset_password : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор формы сброса пароля
     * @param client Указатель на клиентское соединение
     * @param parent Родительский виджет
     */
    explicit reset_password(Client* client, QWidget *parent = nullptr);

    /**
     * @brief Деструктор формы сброса пароля
     */
    ~reset_password();

private slots:
    /// @name Обработчики кнопок
    /// @{
    /**
     * @brief Слот обработки нажатия кнопки сброса пароля
     */
    void on_pushButton_reset_password_clicked();

    /**
     * @brief Слот обработки нажатия кнопки перехода к регистрации
     */
    void on_pushButton_to_reg_clicked();

    /**
     * @brief Слот обработки нажатия кнопки перехода к авторизации
     */
    void on_pushButton_to_auth_clicked();

    /**
     * @brief Слот обработки нажатия кнопки отправки кода подтверждения
     */
    void on_pushButton_code_clicked();

    /**
     * @brief Слот обработки нажатия кнопки закрытия формы
     */
    void on_pushButton_clicked();
    /// @}

    /**
     * @brief Слот обработки ошибки сброса пароля
     */
    void slot_reset_error();

private:
    Ui::reset_password *ui; ///< Указатель на графический интерфейс
    Client* client = nullptr; ///< Указатель на клиентское соединение
    int generate_code; ///< Сгенерированный код подтверждения
};

#endif // RESET_PASSWORD_H
