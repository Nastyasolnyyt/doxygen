#include "reg_form.h"
#include "ui_reg_form.h"
#include "clients_func.h"
#include <QMessageBox>
#include "notification.h"
#include "auth_form.h"
#include "client_main_window.h"
#include "client.h"

#define REG_ERROR "Ошибка при регистрации. Данная учётная запись уже зарегистрирована"

/**
 * @file reg_form.cpp
 * @brief Реализация класса формы регистрации
 */

/**
 * @class Widget
 * @brief Класс, реализующий форму регистрации пользователя
 *
 * Класс предоставляет интерфейс для регистрации новых пользователей,
 * включая валидацию вводимых данных и взаимодействие с сервером.
 */

/**
 * @brief Конструктор формы регистрации
 * @param[in] Client Указатель на объект клиентского соединения
 * @param[in] parent Указатель на родительский виджет (по умолчанию nullptr)
 *
 * Инициализирует форму регистрации, настраивает интерфейс и соединяет сигналы
 * от клиентского соединения с соответствующими слотами формы.
 */
Widget::Widget(Client* Client, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Widget),
    client(Client)
{
    ui->setupUi(this);
    // Настройка окна
    this->setWindowTitle(QString("Метод половинного деления"));
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->ui->lineEdit_login->setFocus();

    // Подключение сигналов
    connect(this->client, &Client::register_ok, this, &Widget::register_successful);
    connect(this->client, &Client::register_error, this, &Widget::register_error);

    this->show();
}

/**
 * @brief Деструктор формы регистрации
 *
 * Освобождает ресурсы, отключает сигналы и выводит отладочное сообщение
 * о разрушении объекта.
 */
Widget::~Widget()
{
    // Отключение сигналов
    disconnect(this->client, &Client::register_ok, this, &Widget::register_successful);
    disconnect(this->client, &Client::register_error, this, &Widget::register_error);

    qDebug() << "Вызвался деструктор окна регистрации";
    delete ui;
}

/**
 * @brief Обработчик нажатия кнопки регистрации
 *
 * Собирает данные из полей формы, проверяет их корректность и,
 * если все данные валидны, отправляет запрос на сервер для регистрации
 * нового пользователя.
 *
 * @note В случае невалидных данных выводит соответствующие предупреждения.
 */
void Widget::on_pushButton_reg_clicked()
{
    // Получение данных из полей ввода
    QString login = ui->lineEdit_login->text();
    QString password = ui->lineEdit_password->text();
    QString hash_password = clients_func::create_hash(password);
    QString email = ui->lineEdit_email->text();

    // Проверка корректности данных
    bool current_login = clients_func::current_login(login);
    bool current_password = clients_func::current_password(password);
    bool current_email = clients_func::current_email(email);
    bool is_empty_name = ui->lineEdit_name->text().isEmpty();
    bool is_empty_last_name = ui->lineEdit_lastname->text().isEmpty();

    // Вывод сообщений об ошибках
    if (!current_login)
        clients_func::create_messagebox("Предупреждение об ошибке",
                                        "Вы ввели логин в некорректном формате.\n\n"
                                        "Логин должен содержать следующие символы: A-Z ; a-z; 0-9;\n"
                                        "спец.символы(кроме $ и | )");

    if (!current_password)
        clients_func::create_messagebox("Предупреждение об ошибке",
                                        "Вы ввели пароль в некорректном формате.\n\n"
                                        "Пароль должен содержать следующие символы: A-Z ; a-z; 0-9;\n"
                                        "спец.символы(кроме $ и | ) и длина не меньше 5 символов.");

    if (!current_email)
        clients_func::create_messagebox("Предупреждение об ошибке",
                                        "Вы ввели почту в некорректном формате.\n\n"
                                        "Почта должна содержать следующие символы: A-Z ; a-z; 0-9;\n"
                                        "спец.символы(кроме $ и | ).\n\n"
                                        "Локальная часть почты не должна начинаться с \".\" и доменная часть почты не должна иметь более двух точек.");

    if (is_empty_name)
        clients_func::create_messagebox("Предупреждение об ошибке", "Введите своё имя!");
    if (is_empty_last_name)
        clients_func::create_messagebox("Предупреждение об ошибке", "Введите свою фамилию");

    // Если все данные корректны - отправляем на сервер
    if (current_login && current_password && current_email &&
        !is_empty_name && !is_empty_last_name) {
        QString final_data = QString("reg|%1$%2$%3$%4$%5$%6")
        .arg(login)
            .arg(hash_password)
            .arg(email)
            .arg(ui->lineEdit_lastname->text())
            .arg(ui->lineEdit_name->text())
            .arg(ui->lineEdit_middlename->text());
        client->write(final_data.toUtf8());
    }
}

/**
 * @brief Обработчик нажатия кнопки перехода к форме авторизации
 *
 * Скрывает текущую форму регистрации и открывает форму авторизации.
 */
void Widget::on_toolButton_auth_clicked()
{
    this->hide();
    new auth_form(client);
    this->close();
}

/**
 * @brief Слот обработки успешной регистрации
 *
 * Вызывается при получении подтверждения успешной регистрации от сервера.
 * Скрывает текущую форму и открывает главное окно клиента.
 */
void Widget::register_successful() {
    this->hide();
    new client_main_window(this->client);
    this->close();
}

/**
 * @brief Слот обработки ошибки регистрации
 *
 * Вызывается при получении сообщения об ошибке регистрации от сервера.
 * Отображает уведомление с соответствующим сообщением об ошибке.
 */
void Widget::register_error() {
    new notification("Ошибка", REG_ERROR);
}

/**
 * @brief Обработчик нажатия кнопки показа пароля
 *
 * Временно отображает введенный пароль в открытом виде.
 */
void Widget::on_pushButton_draw_password_pressed()
{
    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Normal);
}

/**
 * @brief Обработчик отпускания кнопки показа пароля
 *
 * Возвращает отображение пароля в скрытый режим.
 */
void Widget::on_pushButton_draw_password_released()
{
    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
}
