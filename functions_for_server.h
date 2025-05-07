#ifndef FUNCTIONS_FOR_SERVER_H
#define FUNCTIONS_FOR_SERVER_H

#include <QString>
#include <ctime>
#include <QObject>
#include <QList>

/**
 * @brief Класс вспомогательных функций для сервера
 *
 * Реализует паттерн Singleton и предоставляет различные серверные функции:
 * - работу с временем
 * - отправку email
 * - математические вычисления
 */
class functions_for_server: public QObject
{
    Q_OBJECT
private:
    functions_for_server(); ///< Приватный конструктор (реализация Singleton)
    functions_for_server(const functions_for_server&); ///< Запрещенный конструктор копирования
    static functions_for_server* p_instance; ///< Указатель на единственный экземпляр класса

public:
    /**
     * @brief Получение экземпляра класса (Singleton)
     * @return Указатель на единственный экземпляр
     */
    static functions_for_server* get_instance();

    /**
     * @brief Получение текущего времени сервера
     * @return Строка с текущим временем сервера
     */
    QString get_server_time();

    /**
     * @brief Отправка кода подтверждения на email клиента
     * @param email Адрес электронной почты
     * @param code Код подтверждения
     */
    void send_email_to_client(QString email, QString code);

    /**
     * @brief Генерация диапазонов значений
     * @param a Начальное значение
     * @param b Конечное значение
     * @param step Шаг
     * @return Вектор пар, представляющих диапазоны
     */
    QVector<QPair<double, double>> diaposons(double a, double b, double step);

    /**
     * @brief Поиск значений x в заданных диапазонах
     * @param diapozon Вектор диапазонов для поиска
     * @param a Коэффициент a уравнения
     * @param b Коэффициент b уравнения
     * @param c Коэффициент c уравнения
     * @return Вектор найденных значений x
     */
    QVector<double> find_x(const QVector<QPair<double, double>>& diapozon, double a, double b, double c);

    /**
     * @brief Вычисление значения уравнения
     * @param a Коэффициент a
     * @param b Коэффициент b
     * @param c Коэффициент c
     * @param x Значение переменной x
     * @return Результат вычисления уравнения
     */
    double Calc(double a, double b, double c, double x);

signals:
    /// @name Сигналы главного клиентского окна
    /// @{
    /**
     * @brief Сигнал с решением уравнения
     * @param Решение уравнения в строковом формате
     */
    void signal_equation_solution(QString);
    /// @}

public slots:
    /// @name Слоты для работы с уравнениями
    /// @{
    /**
     * @brief Решение линейного уравнения
     * @param a Коэффициент a (в строковом формате)
     * @param b Коэффициент b (в строковом формате)
     */
    void slot_linear_equation(QString a, QString b);

    /**
     * @brief Решение квадратного уравнения
     * @param a Коэффициент a (в строковом формате)
     * @param b Коэффициент b (в строковом формате)
     * @param c Коэффициент c (в строковом формате)
     */
    void slot_quadratic_equation(QString a, QString b, QString c);
    /// @}
};

#endif // FUNCTIONS_FOR_SERVER_H
