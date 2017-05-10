#include "client.h"
#include "ui_client.h"

#include <QFileDialog>
#include <QGraphicsScene>
#include <QTime>
#include <QMenu>

client::client(const QString &strHost, int nPort, QString &uName, QWidget *parent) :
    QMainWindow(parent), m_nNextBlockSize(0),
    ui(new Ui::client)
{
    ui->setupUi(this);

    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
    myName = uName;

    QByteArray hello;
    hello.append("<b>Клиент подключен: "+myName+"</b>");
    slotSendToServer(hello, 's');

    ui->tableFiles->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableFiles,SIGNAL(customContextMenuRequested(const QPoint &)),
            this,SLOT(ProvideContextMenu(const QPoint &)));

    ui->tableFiles->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed );
    ui->tableFiles->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch );
    ui->tableFiles->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed );
    ui->tableFiles->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Fixed );
    ui->tableFiles->setColumnWidth(0,40);
    ui->tableFiles->setColumnWidth(2,80);
    ui->tableFiles->setColumnWidth(3,100);
}

client::~client()
{
    m_pTcpSocket->close();
    delete ui;
}



void client::ProvideContextMenu(const QPoint &pos)
{
    QPoint subM = ui->tableFiles->mapToGlobal(pos);
    int itemRow = ui->tableFiles->indexAt(pos).row();
    if(itemRow!=-1){
        QMenu submenu;
        submenu.addAction("Сохранить");
        submenu.addAction("Удалить");
        QAction* rightClickItem = submenu.exec(subM);

        if (rightClickItem && rightClickItem->text().contains("Удалить") )
        {
            ui->tableFiles->removeRow(itemRow);
        }

        if (rightClickItem && rightClickItem->text().contains("Сохранить") )
        {
            QString defaultFileName = ui->tableFiles->item(itemRow,1)->text();
            QByteArray fileContent = ui->tableFiles->item(itemRow,0)->data(Qt::UserRole).toByteArray();
            QString fileName = QFileDialog::getSaveFileName(0,"Caption","://"+defaultFileName,
                                                            "All (*.*);;"
                                                            "Images (*.png *.bmp *.jpg)");

            QFile file(fileName);
            file.open(QIODevice::WriteOnly);
            file.write(fileContent);
            file.close();
        }
    }
}

void client::slotReadyRead()
{
    bool a = false;
    do{
        a = false;
        if (wait_for_size > 0) {
            if (m_pTcpSocket->bytesAvailable() >= wait_for_size) {
                QByteArray data = m_pTcpSocket->read(wait_for_size);
                if(wait_for_type=='s'){
                    ui->chatField->append(
                                "<b style = \"color: green;\">"
                                +QTime::currentTime().toString()
                                +" </b>"
                                +QString(data));
                }
                else if (wait_for_type=='i'){
                    QDataStream dataStream(data);
                    QString fileName;
                    QByteArray fileContent;
                    dataStream >> fileName;
                    dataStream >> fileContent;

                    QPixmap pixmap;
                    QImage img = QImage::fromData(fileContent);
                    if (img.isNull())
                    {
                        img = QImage(":/new/prefix1/File.png");
                    }
                    pixmap.convertFromImage(img);

                    int rCount = ui->tableFiles->rowCount();
                    ui->tableFiles->setRowCount(rCount + 1);

                    QTableWidgetItem * titem = new QTableWidgetItem();
                    titem->setIcon(QIcon(pixmap));
                    titem->setData(Qt::UserRole, QVariant(fileContent));
                    titem->setTextAlignment(Qt::AlignCenter);
                    int aasd = fileContent.size();
                    ui->tableFiles->setItem(rCount,0,titem);

                    titem = new QTableWidgetItem(fileName);
                    ui->tableFiles->setItem(rCount,1,titem);

                    titem = new QTableWidgetItem(QString::number(aasd/1000)+" KB");
                    titem->setTextAlignment(Qt::AlignCenter);
                    ui->tableFiles->setItem(rCount,2,titem);

                    titem = new QTableWidgetItem("Сервер");
                    titem->setTextAlignment(Qt::AlignCenter);
                    ui->tableFiles->setItem(rCount,3,titem);

                    ui->chatField->append(
                                "<b style = \"color: green;\">"
                                +QTime::currentTime().toString()
                                +"<i> Получен файл"
                                +" \""+fileName+"\""
                                +"</i></b>");
                }
                wait_for_size=0;
                a = true;
            }
        } else
            if(m_pTcpSocket->bytesAvailable() > 4)
            {
                QByteArray info = m_pTcpSocket->read(4);
                memcpy(&wait_for_size, info.data(), 4);

                info = m_pTcpSocket->read(1);
                memcpy(&wait_for_type, info.data(), 1);
                a = true;
            }
    }while (a);
}

void client::slotError(QAbstractSocket::SocketError err) {
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                             "The host was not found.":
                             err == QAbstractSocket::RemoteHostClosedError ?
                                 "The remote host is closed." :
                                 err == QAbstractSocket::ConnectionRefusedError ?
                                     "The connection was refused." :
                                     QString(m_pTcpSocket->errorString()));
    ui->chatField->append(strError);
}


void client::slotSendToServer(QByteArray data, char t) {
    quint32 s = data.size();
    m_pTcpSocket->write((char *)&s, sizeof(s));
    m_pTcpSocket->write((char *)&t, sizeof(t));
    m_pTcpSocket->write(data);
}

void client::slotConnected() {
    ui->chatField->append("Новое подключение...");
}

void client::on_pcmd_clicked()
{
    if(ui->messageLine->text() == "") {
        return;
    }
    QByteArray st;
    st.append("<b style = \"color: green;\">" + myName +": </b>" + ui->messageLine->text());
    slotSendToServer(st, 's');
    ui->chatField->append("<b style = \"color: black;\">"
                          +QTime::currentTime().toString()
                          +" Вы: </b>"
                          +ui->messageLine->text());
    ui->chatField->setText("");
}

void client::on_lineEdit_returnPressed()
{
    on_pcmd_clicked();
}

void client::on_pushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(0, "Выбор изображения для отправки", "://",
                                                    "All (*.*);;"
                                                    "Images (*.png *.bmp *.jpg)");
    //if (fileName.isEmpty())
    //    return;

    QFile file(filePath);
    QFileInfo info(file);
    if (!file.exists())
        return;
    QString fileName = info.fileName();

    file.open(QIODevice::ReadOnly);
    QByteArray fileContent = file.readAll();
    int aasd = fileContent.size();

    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
    dataStream << fileName;
    dataStream << fileContent;

    slotSendToServer(byteArray, 'i');


    //======================================

    QPixmap pixmap;
    QImage img = QImage::fromData(fileContent);
    if (img.isNull())
    {
        img = QImage(":/new/prefix1/File.png");
    }
    pixmap.convertFromImage(img);


    int rCount = ui->tableFiles->rowCount();
    ui->tableFiles->setRowCount(rCount + 1);

    QTableWidgetItem * titem = new QTableWidgetItem();
    titem->setIcon(QIcon(pixmap));
    titem->setData(Qt::UserRole, QVariant(fileContent));
    titem->setTextAlignment(Qt::AlignCenter);
    ui->tableFiles->setItem(rCount,0,titem);

    titem = new QTableWidgetItem(fileName);
    ui->tableFiles->setItem(rCount,1,titem);

    titem = new QTableWidgetItem(QString::number(aasd/1000)+" KB");
    titem->setTextAlignment(Qt::AlignCenter);
    ui->tableFiles->setItem(rCount,2,titem);

    titem = new QTableWidgetItem("Клиент");
    titem->setTextAlignment(Qt::AlignCenter);
    ui->tableFiles->setItem(rCount,3,titem);

    ui->chatField->append(
                "<b>"
                +QTime::currentTime().toString()
                +"<i> Отправлен файл"
                +" \""+fileName+"\""
                +"</i></b>");
}

void client::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    int row = item->row();
    QTableWidgetItem *titem = ui->tableFiles->item(row,0);

    QByteArray fileContent = titem->data(Qt::UserRole).toByteArray();
    QPixmap pixmap;
    QImage img = QImage::fromData(fileContent);
    if (img.isNull())
    {
        //img = QImage(":/new/prefix1/File.png");
        return;
    }
    pixmap.convertFromImage(img);
    QGraphicsScene *sc = new QGraphicsScene;
    sc->addPixmap(pixmap);

    if(!bp)
        bp = new BigPic(sc);
    else
        bp->setSc(sc);

    if(!bp->isVisible() || bp->isHidden())
        bp->show();
}
