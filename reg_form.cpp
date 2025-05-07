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
 * @brief Конструктор формы регистрации
 * @param Client Указатель на клиентское соединение
 * @param parent Родительский виджет
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
 * Проверяет введенные данные и отправляет их на сервер
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
    if (current_login and current_password and current_email and
        !is_empty_name and !is_empty_last_name) {
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
 * @brief Обработчик нажатия кнопки перехода к авторизации
 */
void Widget::on_toolButton_auth_clicked()
{
    this->hide();
    new auth_form(client);
    this->close();
}

/**
 * @brief Слот успешной регистрации
 *
 * Открывает главное окно клиента после успешной регистрации
 */
void Widget::register_successful() {
    this->hide();
    new client_main_window(this->client);
    this->close();
}

/**
 * @brief Слот ошибки регистрации
 *
 * Показывает уведомление об ошибке регистрации
 */
void Widget::register_error() {
    new notification("Ошибка", REG_ERROR);
}

/**
 * @brief Обработчик отображения пароля при нажатии кнопки
 */
void Widget::on_pushButton_draw_password_pressed()
{
    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Normal);
}

/**
 * @brief Обработчик скрытия пароля при отпускании кнопки
 */
void Widget::on_pushButton_draw_password_released()
{
    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
}
