#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QStringList>
#include <QString>
#include <QImageReader>
#include <QByteArray>
#include <QBitmap>
#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QStringLiteral>
#include <QBuffer>
#include <QDataStream>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    QTcpSocket *socket;
    QString file1, file2;
    bool on = false;

    QByteArray Data = QByteArray(0000);

    QImage changeBrightness(QImage &image, int factor);
    QImage changeContrast(QImage &image, int factor);

    void initSocket();

private slots:

    // Original photo
    void on_pushButton_clicked();

    // Overlay photo
    void on_pushButton_2_clicked();


    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
