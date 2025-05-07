#include "auth_form.h"
#include "ui_auth_form.h"
#include "clients_func.h"
#include "client_main_window.h"
#include "reset_password.h"
#include "reg_form.h"
#include <QMessageBox>
#include "notification.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <fstream>
#include <filesystem>

#define AUTH_ERROR "Неверный логин/пароль"

/**
 * @brief Конструктор формы авторизации
 * @param client_socket Указатель на клиентское соединение
 * @param parent Родительский виджет
 */
auth_form::auth_form(Client* client_socket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::auth_form),
    client(client_socket)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Метод половинного деления"));
    this->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    this->ui->lineEdit_login->setFocus();
    this->setAttribute(Qt::WA_DeleteOnClose);

    // Подключаем сигналы авторизации
    connect(this->client, &Client::auth_ok, this, &auth_form::auth_ok);
    connect(this->client, &Client::auth_error, this, &auth_form::auth_error);

    ui->pushButton_draw_password->setFixedSize(QSize(20,ui->pushButton_draw_password->height()));
    this->show();
    this->fill_from_json();
}

/**
 * @brief Деструктор формы авторизации
 */
auth_form::~auth_form()
{
    // Отключаем сигналы авторизации
    disconnect(this->client, &Client::auth_ok, this, &auth_form::auth_ok);
    disconnect(this->client, &Client::auth_error, this, &auth_form::auth_error);

    qDebug() << "Вызвался деструктор окна авторизации";
    delete ui;
}

/**
 * @brief Обработчик нажатия кнопки авторизации
 */
void auth_form::on_pushButton_login_clicked()
{
    bool current_login = clients_func::current_login(ui->lineEdit_login->text());
    bool current_password = clients_func::current_password(ui->lineEdit_password->text());

    // Проверка формата логина
    if (!current_login) {
        clients_func::create_messagebox("Предупреждение об ошибке",
                                        "Вы ввели логин в некорректном формате.\n\n"
                                        "Логин должен содержать следующие символы: A-Z ; a-z; 0-9;\n"
                                        "спец.символы(кроме $ и | )");
    }

    // Проверка формата пароля
    if (!current_password) {
        clients_func::create_messagebox("Предупреждение об ошибке",
                                        "Вы ввели пароль в некорректном формате.\n\n"
                                        "Пароль должен содержать следующие символы: A-Z ; a-z; 0-9;\n"
                                        "спец.символы(кроме $ и | ) и длина не меньше 5 символов.");
    }

    // Если данные введены корректно
    if (current_login and current_password) {
        QString login = ui->lineEdit_login->text();
        QString password = ui->lineEdit_password->text();
        QString hash_password = clients_func::create_hash(password);

        qDebug() << "Расшифрованный хэш: " << hash_password;

        // Формируем и отправляем данные на сервер
        QString final_data = QString("login|%1$%2").arg(login).arg(hash_password);
        client->write(final_data);
    }
}

/**
 * @brief Обработчик нажатия кнопки сброса пароля
 */
void auth_form::on_pushButton_reset_password_clicked()
{
    this->hide();
    new reset_password(this->client);
    this->close();
}

/**
 * @brief Обработчик нажатия кнопки перехода к регистрации
 */
void auth_form::on_pushButton_to_reg_clicked()
{
    this->hide();
    new Widget(this->client);
    this->close();
}

/**
 * @brief Слот успешной авторизации
 */
void auth_form::auth_ok() {
    // Сохраняем данные, если отмечен чекбокс "Запомнить меня"
    if (ui->checkBox_remamber_me->isChecked()) {
        this->write_info_in_cache();
        qDebug() << "Данные записаны";
    }

    this->hide();
    new client_main_window(this->client);
    this->close();
}

/**
 * @brief Слот ошибки авторизации
 */
void auth_form::auth_error() {
    new notification("Ошибка", AUTH_ERROR);
}

/**
 * @brief Обработчик отображения пароля при нажатии кнопки
 */
void auth_form::on_pushButton_draw_password_pressed()
{
    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Normal);
}

/**
 * @brief Обработчик скрытия пароля при отпускании кнопки
 */
void auth_form::on_pushButton_draw_password_released()
{
    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
}

/**
 * @brief Записывает данные авторизации в кэш (JSON-файл)
 */
void auth_form::write_info_in_cache()
{
    QString login = ui->lineEdit_login->text();
    QString password = ui->lineEdit_password->text();

    QJsonObject main_object;
    main_object["login"] = QJsonValue(login);
    main_object["password"] = QJsonValue(password);

    QJsonDocument json_doc;
    json_doc.setObject(main_object);

    // Создаем директорию для кэша, если она не существует
    if (std::filesystem::create_directory("./cache")) {
        qDebug() << "Директория успешно создана";
    }
    else {
        qDebug() << "Ошибка! Вероятно директория уже существует";
    }

    // Записываем данные в файл
    std::fstream json_file;
    json_file.open("cache/auth_data.json", std::ios_base::out);

    if (json_file.is_open()) {
        json_file.write(json_doc.toJson(), json_doc.toJson().size());
        new notification("Успех", "Ваши данные записаны!");
        json_file.close();
    }
    else {
        new notification("Ошибка", "Непредвиденная ошибка при попытке записи JSON-файла");
    }
}

/**
 * @brief Заполняет поля формы данными из кэша (JSON-файла)
 */
void auth_form::fill_from_json() {
    QJsonDocument json_data;
    std::fstream json_file;
    std::string lines_from_file;

    // Чтение данных из файла
    json_file.open("cache/auth_data.json", std::ios_base::in);

    if (json_file.is_open()) {
        qDebug() << "json file успешно открыт";
        std::string line_from_file;

        while (std::getline(json_file, line_from_file)) {
            lines_from_file += line_from_file;
        }

        json_data = QJsonDocument::fromJson(QByteArray(lines_from_file.c_str()));
        qDebug().noquote() << json_data.toJson();
    }
    else {
        qDebug() << "Ошибка при открытии json-файла";
        return;
    }

    // Заполнение полей ввода
    ui->lineEdit_login->setText(json_data.object()["login"].toString());
    ui->lineEdit_password->setText(json_data.object()["password"].toString());
}
