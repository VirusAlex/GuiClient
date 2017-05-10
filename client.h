#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>
#include <QPointer>
#include <QTableWidgetItem>
#include "bigpic.h"

namespace Ui {
class client;
}

class client : public QMainWindow
{
    Q_OBJECT

public:
    explicit client(const QString &strHost, int nPort, QString &uName, QWidget *parent = 0);
    ~client();

private:
    Ui::client *ui;
    quint32 m_nNextBlockSize;
    QTcpSocket* m_pTcpSocket;
    quint32 wait_for_size = 0;
    char wait_for_type = 'u';
    QString myName;
    QPointer<BigPic> bp;

private slots:
     void slotReadyRead();

     void slotError(QAbstractSocket::SocketError);
     void slotSendToServer(QByteArray data, char t);

     void slotConnected();
     void on_pcmd_clicked();
     void on_lineEdit_returnPressed();
     void on_pushButton_clicked();
     void ProvideContextMenu(const QPoint &pos);
     void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);
};

#endif // CLIENT_H
