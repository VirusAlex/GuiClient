#ifndef BIGPIC_H
#define BIGPIC_H

#include <QWidget>
#include <QGraphicsScene>

namespace Ui {
class BigPic;
}

class BigPic : public QWidget
{
    Q_OBJECT

protected:
    virtual void wheelEvent(QWheelEvent* event);

public:
    explicit BigPic(QWidget *parent = 0);
    explicit BigPic(QGraphicsScene *sc, QWidget *parent = 0);
    void setSc(QGraphicsScene *sc);
    ~BigPic();

private:
    Ui::BigPic *ui;

};

#endif // BIGPIC_H
