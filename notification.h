#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class notification;
}

/**
 * @brief Класс всплывающего уведомления
 *
 * Предоставляет функционал для отображения временных уведомлений
 * с возможностью перемещения и автоматического закрытия
 */
class notification : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор уведомления
     * @param title Заголовок уведомления
     * @param text Текст уведомления
     * @param parent Родительский виджет
     */
    explicit notification(QString title, QString text, QWidget *parent = nullptr);

    /**
     * @brief Деструктор уведомления
     */
    ~notification();

private slots:
    /**
     * @brief Слот обработки нажатия кнопки закрытия
     */
    void on_pushButton_close_clicked();

private:
    Ui::notification *ui; ///< Указатель на графический интерфейс
    int current_value_progress_bar = 0; ///< Текущее значение прогресс-бара
    QTimer* timer; ///< Таймер для анимации прогресс-бара
    QString text; ///< Текст уведомления
    QPoint last_press_position; ///< Последняя позиция курсора при нажатии

    /**
     * @brief Обновляет значение прогресс-бара
     */
    void update_progress_bar();

    /**
     * @brief Закрывает окно уведомления
     */
    void close_window();

    /**
     * @brief Обработчик нажатия кнопки мыши (переопределение)
     * @param event Событие мыши
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief Обработчик перемещения мыши (переопределение)
     * @param event Событие мыши
     */
    void mouseMoveEvent(QMouseEvent* event) override;
};

#endif // NOTIFICATION_H
