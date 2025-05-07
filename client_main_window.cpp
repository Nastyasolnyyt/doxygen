#include "client_main_window.h"
#include "ui_client_main_window.h"
#include "reg_form.h"
#include "client.h"
#include "clients_func.h"
#include <QMessageBox>
#include <QLabel>
#include "notification.h"

#define NOTIFICATION_ERROR "Убедитесь, что вы ввели корректные коэффициенты."

/**
 * @brief Конструктор главного окна клиента
 * @param client Указатель на клиентское соединение
 * @param parent Родительский виджет
 */
client_main_window::client_main_window(Client* client, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::client_main_window),
    client(client)
{
    ui->setupUi(this);
    // Настройка окна
    this->setWindowFlag(Qt::WindowType::MSWindowsFixedSizeDialogHint);
    this->setWindowTitle(QString("Метод половинного деления"));
    ui->pushButton->setToolTip("Выйти из учётной записи.");
    ui->comboBox->setToolTip("Выберите вид уравнения.");
    this->setAttribute(Qt::WA_DeleteOnClose);

    // Начальная настройка интерфейса
    clients_func::equation(ui->Layout_quadratic, action::HIDE);
    ui->label_answer_x->hide();

    // Подключение сигналов
    connect(this->client, &Client::equation_ok, this, &client_main_window::slot_equation_ok);
    connect(this->client, &Client::equation_fail, this, &client_main_window::slot_equation_fail);

    this->show();
}

/**
 * @brief Деструктор главного окна клиента
 */
client_main_window::~client_main_window()
{
    qDebug() << "Вызвался деструктор окна клиента";
    delete ui;
}

/**
 * @brief Обработчик нажатия кнопки выхода из учетной записи
 */
void client_main_window::on_pushButton_clicked()
{
    this->hide();
    new Widget(this->client);
    this->close();
}

/**
 * @brief Обработчик выбора типа уравнения в комбобоксе
 * @param index Индекс выбранного элемента
 */
void client_main_window::on_comboBox_activated(int index)
{
    if (index == 0) {
        // Линейное уравнение
        clients_func::equation(ui->Layout_quadratic, action::HIDE);
        clients_func::equation(ui->layout_linear, action::SHOW);
    }
    else if (index == 1) {
        // Квадратное уравнение
        clients_func::equation(ui->Layout_quadratic, action::SHOW);
        clients_func::equation(ui->layout_linear, action::HIDE);
    }
    ui->label_answer_x->hide();
}

/**
 * @brief Обработчик нажатия кнопки решения уравнения
 */
void client_main_window::on_pushButton_solve_equation_clicked()
{
    if (ui->comboBox->currentIndex() == 0) {
        // Обработка линейного уравнения
        bool bool_arg_a = false;
        int arg_a = ui->lineEdit_a_linear->text().toDouble(&bool_arg_a);

        bool bool_arg_b = false;
        int arg_b = ui->lineEdit_b_linear->text().toDouble(&bool_arg_b);

        if (bool_arg_a and bool_arg_b) {
            QString text_in_dialogbox = QString("Ваше уравнение: %1%2x%3%4 = 0")
                .arg(ui->comboBox_sign_linear->currentText())
                .arg(ui->lineEdit_a_linear->text())
                .arg(ui->comboBox_sign2_linear->currentText())
                .arg(ui->lineEdit_b_linear->text());

            qDebug() << text_in_dialogbox;

            // Формируем и отправляем уравнение на сервер
            this->client->write(QString("equation|linear|%1%2$%3%4")
                .arg(ui->comboBox_sign_linear->currentText())
                .arg(ui->lineEdit_a_linear->text())
                .arg(ui->comboBox_sign2_linear->currentText())
                .arg(ui->lineEdit_b_linear->text()));
        }
        else {
            new notification("Ошибка", NOTIFICATION_ERROR, this);
        }
    }
    else if (ui->comboBox->currentIndex() == 1) {
        // Обработка квадратного уравнения
        bool bool_arg_a = false;
        int arg_a = ui->lineEdit_a_quadratic->text().toDouble(&bool_arg_a);
        bool bool_arg_b = false;
        int arg_b = ui->lineEdit_b_quadratic->text().toDouble(&bool_arg_b);
        bool bool_arg_c = false;
        int arg_c = ui->lineEdit_c_quadratic->text().toDouble(&bool_arg_c);

        if (bool_arg_a and bool_arg_b and bool_arg_c) {
            // Формируем и отправляем уравнение на сервер
            this->client->write((QString("equation|quadratic|%1%2$%3%4$%5%6")
                .arg(ui->comboBox_sign2_quardratic->currentText())
                .arg(ui->lineEdit_a_quadratic->text())
                .arg(ui->comboBox_sign2_quadratic_2->currentText())
                .arg(ui->lineEdit_b_quadratic->text())
                .arg(ui->comboBox_sign2_quadratic_3->currentText())
                .arg(ui->lineEdit_c_quadratic->text()));
        }
        else {
            qDebug() << bool_arg_a << " " << bool_arg_b << " " << bool_arg_c;
            new notification("Ошибка", NOTIFICATION_ERROR);
        }
    }
}

/**
 * @brief Слот успешного решения уравнения
 * @param answer Ответ сервера с решением
 */
void client_main_window::slot_equation_ok(QString answer)
{
    QStringList answers = answer.split("$");
    QString text_for_notification = QString("Ответ: ");

    // Формируем строку с ответами
    for (int i = 0; i < answers.size(); i++) {
        text_for_notification += QString("%1 ").arg(answers[i]);
    }

    // Отображаем ответ
    this->ui->label_answer_x->setText(text_for_notification);
    this->ui->label_answer_x->resize(ui->label_answer_x->sizeHint());

    if (this->ui->label_answer_x->isHidden()) {
        this->ui->label_answer_x->show();
    }
}

/**
 * @brief Слот ошибки решения уравнения
 * @param fail Сообщение об ошибке
 */
void client_main_window::slot_equation_fail(QString& fail)
{
    QString text_for_answer = QString("Ошибка: %1").arg(fail);
    this->ui->label_answer_x->setText(text_for_answer);
    this->ui->label_answer_x->resize(ui->label_answer_x->sizeHint());

    if (this->ui->label_answer_x->isHidden()) {
        this->ui->label_answer_x->show();
    }
}
