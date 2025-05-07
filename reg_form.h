#ifndef REG_FORM_H
#define REG_FORM_H

#include <QWidget>
#include <QApplication>
#include <QScreen>
#include "client.h"
#include <QCloseEvent>

// Предварительные объявления классов
class auth_form;          ///< Класс формы авторизации
class client_main_window; ///< Класс главного окна клиента

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

/**
 * @brief Класс формы регистрации пользователя
 *
 * Предоставляет интерфейс для регистрации новых пользователей
 * и обработки результатов регистрации
 */
class Widget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор формы регистрации
     * @param client Указатель на клиентское соединение
     * @param parent Родительский виджет
     */
    Widget(Client* client, QWidget *parent = nullptr);

    /**
     * @brief Деструктор формы регистрации
     */
    ~Widget();

private slots:
    /// @name Обработчики кнопок
    /// @{
    /**
     * @brief Слот обработки нажатия кнопки регистрации
     */
    void on_pushButton_reg_clicked();

    /**
     * @brief Слот обработки нажатия кнопки перехода к авторизации
     */
    void on_toolButton_auth_clicked();
    /// @}

    /// @name Слоты обработки результатов регистрации
    /// @{
    /**
     * @brief Слот успешной регистрации
     */
    void register_successful();

    /**
     * @brief Слот ошибки регистрации
     */
    void register_error();
    /// @}

    /// @name Обработчики отображения пароля
    /// @{
    /**
     * @brief Слот отображения пароля при нажатии кнопки
     */
    void on_pushButton_draw_password_pressed();

    /**
     * @brief Слот скрытия пароля при отпускании кнопки
     */
    void on_pushButton_draw_password_released();
    /// @}

private:
    Ui::Widget *ui;       ///< Указатель на графический интерфейс
    Client* client = nullptr; ///< Указатель на клиентское соединение

    /**
     * @brief Создает и отображает уведомление
     * @param title Заголовок уведомления
     * @param text Текст уведомления
     */
    void create_notification(QString title, QString text);

    /**
     * @brief Сбрасывает текст в полях ввода
     */
    void reset_text();
};

#endif // REG_FORM_H
