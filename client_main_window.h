#ifndef CLIENT_MAIN_WINDOW_H
#define CLIENT_MAIN_WINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QIntValidator>
#include "notification.h"

// Предварительные объявления классов
class Widget; ///< Класс окна регистрации
class Client; ///< Класс клиентского соединения

namespace Ui {
class client_main_window;
}

/**
 * @brief Класс главного окна клиентского приложения
 *
 * Предоставляет интерфейс для работы с математическими уравнениями,
 * управления сессией и взаимодействия с сервером
 */
class client_main_window : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна клиента
     * @param client Указатель на объект клиентского соединения
     * @param parent Родительский виджет
     */
    explicit client_main_window(Client* client, QWidget *parent = nullptr);

    /**
     * @brief Деструктор главного окна клиента
     */
    ~client_main_window();

private slots:
    /**
     * @brief Слот обработки нажатия кнопки выхода из учетной записи
     */
    void on_pushButton_clicked();

    /**
     * @brief Слот обработки выбора типа уравнения в комбобоксе
     * @param index Индекс выбранного элемента
     */
    void on_comboBox_activated(int index);

    /**
     * @brief Слот обработки нажатия кнопки решения уравнения
     */
    void on_pushButton_solve_equation_clicked();

    /**
     * @brief Слот успешного решения уравнения
     * @param answer Строка с ответом от сервера
     */
    void slot_equation_ok(QString answer);

    /**
     * @brief Слот ошибки при решении уравнения
     * @param fail Строка с описанием ошибки
     */
    void slot_equation_fail(QString& fail);

private:
    Ui::client_main_window *ui; ///< Указатель на графический интерфейс
    Client* client = nullptr;   ///< Указатель на клиентское соединение
};

#endif // CLIENT_MAIN_WINDOW_H
