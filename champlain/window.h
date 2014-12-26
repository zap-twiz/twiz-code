// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QSlider>

class QTimer;

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

protected:
    void keyPressEvent(QKeyEvent *event);

protected slots:
    void timeOutSlot();

    void timeOut();

private:
    Ui::Window *ui;

    QTimer *m_timer;
};

#endif // WINDOW_H
