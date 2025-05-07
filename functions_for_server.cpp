#include "../include/functions_for_server.h"
#include <QDebug>
#include "../libraries/SMTPEmail/include/SmtpMime"

/// Статический член класса
functions_for_server* functions_for_server::p_instance = nullptr;

/**
 * @brief Конструктор класса functions_for_server
 */
functions_for_server::functions_for_server(){}

/**
 * @brief Отправляет email с кодом подтверждения
 * @param email Адрес электронной почты
 * @param code Код подтверждения
 */
void functions_for_server::send_email_to_client(QString email, QString code)
{
    // Настройка сообщения
    MimeMessage message;
    EmailAddress sender("roma-leonenko@bk.ru", "Roman");
    EmailAddress receiver(email, "Code for reset password");
    message.setSender(sender);
    message.addRecipient(receiver);
    message.setSubject(QString("Code for receive password"));

    // Текст сообщения
    MimeText text_email;
    text_email.setText(QString("Hi. Here is your password recovery code for your account: %1\n"
                               "If you did not request the code, please ignore the message.").arg(code));
    message.addPart(&text_email);

    // Настройка SMTP клиента
    SmtpClient smtp("smtp.mail.ru", 465, SmtpClient::SslConnection);

    // Подключение и отправка
    smtp.connectToHost();
    if (!smtp.waitForReadyConnected()) {
        qDebug() << "Ошибка подключения к mail.ru!";
        return;
    }

    smtp.login("roma-leonenko@bk.ru", "fv2TpkktaiaimAqKN6iH");
    if (!smtp.waitForAuthenticated()) {
        qDebug() << "Ошибка авторизации!";
        return;
    }

    smtp.sendMail(message);
    if (!smtp.waitForMailSent()) {
        qDebug() << "Ошибка отправки сообщения";
        return;
    }

    qDebug() << "Письмо успешно отправлено!";
    smtp.quit();
}

/**
 * @brief Возвращает экземпляр синглтона
 * @return Указатель на экземпляр functions_for_server
 */
functions_for_server* functions_for_server::get_instance() {
    if (p_instance == nullptr) {
        p_instance = new functions_for_server();
    }
    return p_instance;
}

/**
 * @brief Возвращает текущее время сервера
 * @return Строка с отформатированным временем
 */
QString functions_for_server::get_server_time() {
    time_t now_time = time(nullptr);
    tm* time_struct = localtime(&now_time);
    char time_format[100];
    strftime(time_format, sizeof(time_format), "[%b %d %Y %H:%M:%S]", time_struct);
    return QString(time_format);
}

/**
 * @brief Генерирует диапазоны значений
 * @param a Начальное значение
 * @param b Конечное значение
 * @param step Шаг
 * @return Вектор диапазонов
 */
QVector<QPair<double, double>> functions_for_server::diaposons(double a, double b, double step) {
    QVector<QPair<double, double>> diaposon;
    for (double x = a; x < b; x += step) {
        diaposon.push_back({x, x + step});
    }
    return diaposon;
}

/**
 * @brief Находит корни уравнения на заданных интервалах
 * @param diapozon Вектор интервалов поиска
 * @param a Коэффициент a
 * @param b Коэффициент b
 * @param c Коэффициент c
 * @return Вектор найденных корней
 */
QVector<double> functions_for_server::find_x(const QVector<QPair<double, double>>& diapozon,
                                             double a, double b, double c) {
    QVector<double> answers;
    const double eps = 1e-8;
    const double zero_eps = 1e-10;

    auto isZero = [zero_eps](double val) {
        return std::abs(val) < zero_eps;
    };

    for (auto interval : diapozon) {
        double left = interval.first;
        double right = interval.second;
        double f_left = Calc(a, b, c, left);
        double f_right = Calc(a, b, c, right);

        // Проверка корней на границах
        if (isZero(f_left)) {
            answers.push_back(left);
            continue;
        }
        if (isZero(f_right)) {
            answers.push_back(right);
            continue;
        }

        // Поиск корня методом бисекции
        if (f_left * f_right <= 0 || std::min(std::abs(f_left), std::abs(f_right)) < zero_eps) {
            while (std::abs(right - left) > eps) {
                double mid = (left + right) / 2;
                double f_mid = Calc(a, b, c, mid);

                if (isZero(f_mid)) {
                    answers.push_back(mid);
                    break;
                }

                if (f_left * f_mid < 0) {
                    right = mid;
                    f_right = f_mid;
                } else {
                    left = mid;
                    f_left = f_mid;
                }
            }
            double root = (left + right) / 2;
            if (isZero(Calc(a, b, c, root))) {
                answers.push_back(root);
            }
        }
    }

    // Удаление дубликатов
    QVector<double> unique_answers;
    for (double root : answers) {
        bool exists = false;
        for (double unique_root : unique_answers) {
            if (qFuzzyCompare(root, unique_root)) {
                exists = true;
                break;
            }
        }
        if (!exists) unique_answers.push_back(root);
    }

    return unique_answers;
}

/**
 * @brief Вычисляет значение квадратного уравнения
 * @param a Коэффициент a
 * @param b Коэффициент b
 * @param c Коэффициент c
 * @param x Значение переменной
 * @return Результат вычисления
 */
double functions_for_server::Calc(double a, double b, double c, double x){
    return x*x*a + x*b + c;
}

/// @name Обработчики уравнений
/// @{
/**
 * @brief Решает линейное уравнение
 * @param a Коэффициент a (в строковом формате)
 * @param b Коэффициент b (в строковом формате)
 */
void functions_for_server::slot_linear_equation(QString a, QString b)
{
    bool ok1, ok2;
    double coeff_a = a.toDouble(&ok1);
    double coeff_b = b.toDouble(&ok2);
    QString solution;

    if (!ok1 || !ok2) {
        solution = "answer|Некорректный ввод!";
        emit this->signal_equation_solution(solution);
        return;
    }

    if (qFuzzyIsNull(coeff_a)) {
        if (qFuzzyIsNull(coeff_b)) {
            solution = "answer|Бесконечное число решений";
        } else {
            solution = "answer|Решений нет";
        }
    } else {
        double vertex = -coeff_b/(2*coeff_a);
        QVector<QPair<double, double>> ans = diaposons(vertex-5, vertex+5, 0.01);
        QVector<double> korni = find_x(ans, 0, coeff_a, coeff_b);

        if (korni.size() > 0) {
            solution = QString("answer|");
            for (const auto &el : korni) {
                solution.append(QString::number(el));
                solution.append("$");
            }
            solution.chop(1);
            double answer = -coeff_b / coeff_a;
            solution = QString("answer|%1").arg(answer);
        }
        else {
            solution = QString("answer|Решений нет");
        }
    }

    emit this->signal_equation_solution(solution);
}

/**
 * @brief Решает квадратное уравнение
 * @param a Коэффициент a (в строковом формате)
 * @param b Коэффициент b (в строковом формате)
 * @param c Коэффициент c (в строковом формате)
 */
void functions_for_server::slot_quadratic_equation(QString a, QString b, QString c)
{
    bool ok1, ok2, ok3;
    double coeff_a = a.toDouble(&ok1);
    double coeff_b = b.toDouble(&ok2);
    double coeff_c = c.toDouble(&ok3);
    QString solution;

    if (!ok1 || !ok2 || !ok3) {
        solution = "answer|Некорректный ввод";
        emit this->signal_equation_solution(solution);
        return;
    }

    if (qFuzzyIsNull(coeff_a) && qFuzzyIsNull(coeff_b)) {
        if (qFuzzyIsNull(coeff_c)) {
            emit this->signal_equation_solution("answer|Бесконечное число решений");
            return;
        }
        emit this->signal_equation_solution("answer|Решений нет");
        return;
    }

    QVector<QPair<double, double>> ans = diaposons(-10, 10, 0.001);
    QVector<double> korni = find_x(ans, coeff_a, coeff_b, coeff_c);

    if (korni.size() > 0) {
        solution = QString("answer|");
        for (const auto &el : korni) {
            solution.append(QString::number(el));
            solution.append("$");
        }
        solution.chop(1);
    }
    else {
        solution = QString("answer|Решений нет");
    }

    emit this->signal_equation_solution(solution);
}
/// @}
