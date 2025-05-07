#include "notification.h"
#include "ui_notification.h"
#include <QTimer>

/**
 * @brief Конструктор уведомления
 * @param title Заголовок уведомления
 * @param text Текст уведомления
 * @param parent Родительский виджет
 */
notification::notification(QString title, QString text, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::notification),
    text(text)
{
    ui->setupUi(this);

    // Настройка окна уведомления
    this->setWindowFlags(Qt::FramelessWindowHint); // Убираем рамку окна
    this->setWindowTitle(title);
    this->move(10, 10); // Позиционируем окно в левом верхнем углу

    // Настройка текста уведомления
    ui->label->setText(text);
    ui->label->setWordWrap(true); // Включаем перенос текста
    ui->label->resize(ui->label->sizeHint()); // Автоматический размер

    this->show();
    this->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose); // Автоматическое удаление при закрытии

    update_progress_bar(); // Запускаем прогресс-бар
}

/**
 * @brief Деструктор уведомления
 */
notification::~notification()
{
    qDebug() << "Вызван деструктор уведомления";
    delete ui;
}

/**
 * @brief Обновляет прогресс-бар уведомления
 *
 * Запускает таймер, который постепенно заполняет прогресс-бар.
 * При достижении 100% закрывает уведомление.
 */
void notification::update_progress_bar() {
    timer = new QTimer(this);
    timer->setInterval(20); // Интервал 20 мс

    connect(timer, &QTimer::timeout, [this]() -> void {
        if (this->ui->progressBar->value() != 100) {
            this->ui->progressBar->setValue(++this->current_value_progress_bar);
        }
        else if (this->ui->progressBar->value() == 100) {
            timer->deleteLater(); // Освобождаем таймер
            this->close_window(); // Закрываем уведомление
        }
    });

    timer->start();
}

/**
 * @brief Плавно закрывает уведомление
 *
 * Постепенно уменьшает прозрачность окна перед закрытием
 */
void notification::close_window()
{
    qreal start_opacity = 1;
    while (start_opacity > 0) {
        start_opacity -= 0.0001;
        this->setWindowOpacity(start_opacity);
    }
    this->close();
}

/**
 * @brief Обработчик нажатия кнопки мыши
 * @param event Событие мыши
 *
 * Запоминает позицию нажатия для последующего перемещения окна
 */
void notification::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        this->last_press_position = event->pos();
    }
    event->accept();
}

/**
 * @brief Обработчик перемещения мыши
 * @param event Событие мыши
 *
 * Перемещает окно при зажатой левой кнопке мыши
 */
void notification::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        this->move(std::move(event->globalPosition().toPoint() - last_press_position));
    }
    event->accept();
}

/**
 * @brief Обработчик нажатия кнопки закрытия
 *
 * Немедленно закрывает уведомление при нажатии на кнопку
 */
void notification::on_pushButton_close_clicked()
{
    this->close();
}
