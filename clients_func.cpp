#include "clients_func.h"
#include <QApplication>
#include <QVector>
#include <QRandomGenerator>
#include <QWidget>
#include <QLineEdit>
#include <QCryptographicHash>

/**
 * @brief Проверяет строку на допустимые символы
 * @param text Строка для проверки
 * @return true если строка содержит только допустимые символы, иначе false
 *
 * Допустимые символы: латинские буквы, цифры, пунктуация и символы,
 * кроме запрещенных символов '$' и '|'
 */
bool clients_func::english_symbols(QString text) {
    if (text.size() > 1) {
        for (QChar ch: text) {
            if (ch != QChar('$') and ch != QChar('|')) {
                if ((ch.toLatin1() >= 'A' and ch.toLatin1() <= 'Z') or
                    (ch.toLatin1() >= 'a' and ch.toLatin1() <= 'z') or
                    ch.isDigit() or ch.isPunct() or ch.isSymbol())
                    continue;
                else {
                    return false;
                }
            }
            else return false;
        }
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief Создает и отображает информационное окно
 * @param title Заголовок окна
 * @param message Текст сообщения
 */
void clients_func::create_messagebox(QString title, QString message)
{
    QMessageBox msg_box;
    msg_box.setIcon(QMessageBox::Information);
    msg_box.setStyleSheet("QMessageBox { background-color: rgb(33, 35, 40) }; }"
                          "QMessageBox QLabel { color: white; }");
    msg_box.setWindowTitle(title);
    msg_box.setText(message);
    msg_box.exec();
}

/**
 * @brief Проверяет корректность логина
 * @param login Логин для проверки
 * @return true если логин корректен, иначе false
 *
 * Логин должен содержать только латинские буквы, цифры
 * и не содержать запрещенных символов '|' и '$'
 */
bool clients_func::current_login(QString login) {
    bool english_symbols = false;
    for (auto symbol: login) {
        if ( ((symbol.toLatin1() >= 'A' and symbol.toLatin1() <= 'Z') or
             (symbol.toLatin1() >= 'a' and symbol.toLatin1() <= 'z') or
             symbol.isDigit()) and
            symbol != QChar('|') and symbol != QChar('$'))
            english_symbols = true;
        else
            english_symbols = false;
    }
    return english_symbols;
}

/**
 * @brief Проверяет корректность пароля
 * @param password Пароль для проверки
 * @return true если пароль корректен, иначе false
 *
 * Пароль должен:
 * - Содержать только допустимые символы
 * - Иметь длину не менее 5 символов
 * - Содержать минимум один спецсимвол
 * - Содержать минимум одну цифру
 * - Содержать минимум одну заглавную букву
 */
bool clients_func::current_password(QString password) {
    bool english_symbols = false;
    bool is_spec_symbols = false;
    bool is_numbers = false;
    bool is_upper_symbols = false;
    bool length_password_equal_or_more_5 = false;

    english_symbols = clients_func::english_symbols(password);

    for(auto symbol: password) {
        if (symbol.isPunct())
            is_spec_symbols = true;
        if (symbol.isDigit())
            is_numbers = true;
        if (symbol.isUpper())
            is_upper_symbols = true;
    }

    if (password.length() >= 5) {
        length_password_equal_or_more_5 = true;
    }
    return english_symbols & length_password_equal_or_more_5 &
           is_spec_symbols & is_numbers & is_upper_symbols;
}

/**
 * @brief Проверяет корректность email
 * @param email Email для проверки
 * @return true если email корректен, иначе false
 *
 * Email должен:
 * - Содержать ровно один символ '@'
 * - Иметь длину не более 254 символов
 * - Локальная часть не должна начинаться с точки
 * - Доменная часть должна содержать минимум одну точку
 */
bool clients_func::current_email(QString email) {
    QList parts_of_email = email.split(QChar('@'));
    if (parts_of_email.size() == 2 and email.size() <= 254) {
        QString local_part = parts_of_email.front();
        QString domain_part = parts_of_email.back();
        bool local_part_bool = clients_func::english_symbols(local_part);
        bool domain_part_bool = clients_func::english_symbols(domain_part);

        if (!local_part.startsWith('.'))
            local_part_bool = local_part_bool & true;
        else
            local_part_bool = local_part_bool & false;

        if (domain_part.count(QChar('.')) >= 1)
            domain_part_bool = domain_part_bool & true;
        return (local_part_bool & domain_part_bool);
    }
    else {
        return false;
    }
}

/**
 * @brief Возвращает текущее время в формате строки
 * @return Строка с текущим временем в формате "[Месяц День Год Часы:Минуты:Секунды]"
 */
QString clients_func::get_client_time() {
    time_t now_time = time(nullptr);
    tm* time_struct = localtime(&now_time);
    char time_format[100];
    strftime(time_format, sizeof(time_format), "[%b %d %Y %H:%M:%S]", time_struct);
    return QString(time_format);
}

/**
 * @brief Генерирует случайный пароль
 * @return Случайно сгенерированный пароль
 *
 * Пароль содержит:
 * - Заглавные и строчные латинские буквы
 * - Цифры
 * - Специальные символы
 * - Длину от 7 до 15 символов
 * - Гарантированно содержит минимум по одному символу из каждой группы
 */
QString clients_func::random_password() {
    using milliseconds = std::chrono::duration<unsigned long long, std::ratio<1,1000>>;
    QRandomGenerator object(std::chrono::duration_cast<milliseconds>(
                                std::chrono::system_clock::now().time_since_epoch()).count());

    int random_length = object.bounded(7, 16);
    QString symbols = QString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#%&'()*+,-./:;<=>?@[\\]^_`{}");
    QString upper_symbols = QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    QString lower_symbols = QString("abcdefghijklmnopqrstuvwxyz");
    QString digits = QString("0123456789");
    QString special_symbols = QString("!\"#%&'()*+,-./:;<=>?@[\\]^_`{}");

    QString random_pswd;
    int random_count = object.bounded(0, random_length / 5 + 1);
    for (int i = 0; i < random_count; i++ ) {
        random_pswd.append(upper_symbols[object.bounded(0, upper_symbols.length() - 1)]);
        random_pswd.append(lower_symbols[object.bounded(0, lower_symbols.length() - 1)]);
        random_pswd.append(digits[object.bounded(0, digits.length() - 1)]);
        random_pswd.append(special_symbols[object.bounded(0, special_symbols.length() - 1)]);
    }

    for (int i = 0; i < (random_length - random_count * 4); i++) {
        random_pswd.append(symbols[object.bounded(0, symbols.length())]);
    }

    std::random_shuffle(random_pswd.begin(), random_pswd.end());
    return random_pswd;
}

/**
 * @brief Управляет видимостью виджетов в layout
 * @param uravnenie Указатель на QHBoxLayout
 * @param effect Действие (показать/скрыть)
 */
void clients_func::equation(QHBoxLayout* uravnenie, action effect) {
    if (effect == action::HIDE) {
        for (int i = 0; i < uravnenie->count(); i++) {
            uravnenie->itemAt(i)->widget()->hide();
        }
    }
    else if (effect == action::SHOW) {
        for (int i = 0; i < uravnenie->count(); i++) {
            uravnenie->itemAt(i)->widget()->show();
        }
    }
}

/**
 * @brief Создает SHA-256 хеш строки
 * @param text Исходная строка
 * @return Хеш строки в hex-формате
 */
QString clients_func::create_hash(QString text)
{
    return QCryptographicHash::hash(text.toUtf8(),
                                    QCryptographicHash::Algorithm::Sha256).toHex();
}

/**
 * @brief Генерирует случайный 4-значный код
 * @return Случайный код от 1000 до 9999
 */
int clients_func::random_code() {
    QRandomGenerator generate_random_code(QTime::currentTime().msec());
    return generate_random_code.bounded(1000, 9999 + 1);
}
