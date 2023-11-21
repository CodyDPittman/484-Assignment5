#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Initializing the socket
void MainWindow::initSocket()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("100.69.238.163", 80);
}

// Reading and loading user selected image
void MainWindow::on_pushButton_clicked()
{
    // Reading image
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), tr("Images (*.bmp)"));
    file1 = fileName;
    char ch;

    // Loading image
    QImage image(fileName);
    QPixmap p = QPixmap::fromImage(image);


    // Obtaing images dimensions
    int w = ui->ogPic->width();
    int h = ui->ogPic->height();

    // Placing image onto the UI
    ui->ogPic->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));
    ui->label_3->setPixmap(p.scaled(w,h, Qt::KeepAspectRatio));
}

// Reading and loading user selected overlay image
void MainWindow::on_pushButton_2_clicked()
{
    // Reading image
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), tr("Images (*.bmp)"));
    file2 = fileName;

    // Loading image
    QImage image(fileName);
    QPixmap p = QPixmap::fromImage(image);

    // Obtaining images dimensions
    int w = ui->overPic->width();
    int h = ui->overPic->height();

    // Placing image onto the UI
    ui->overPic->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));
    ui->label_4->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));

}

// Transfer Orignal Button
void MainWindow::on_pushButton_3_clicked()
{
    // Create a QImage of the recently displayed bitmap
    QImage image(file1);

    // Creating a byte array for the QImage object
    QByteArray arr;
    QBuffer buffer(&arr);

    // Transforming the image into a JPEG
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG");

    arr.append("o"); // Creating sentinal value

    // Sending data to the server
   if(socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Connected!";
        socket->write(arr);
        socket->waitForBytesWritten(50000);
    }
    else
        qDebug() << "Not connected";
}

// Transfer Overlay Button
void MainWindow::on_pushButton_4_clicked()
{
    // Create QImage of the overlay image
    QImage image(file2);

    // Creating byte array for QImage object
    QByteArray arr;
    QBuffer buffer(&arr);

    // Transforming the image into a JPEG
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG");

    arr.append("p"); // Creating sentinal value

    // Sending data to the server
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Connected!";
        socket->write(arr);
        socket->waitForBytesWritten(50000);
    }
    else
        qDebug() << "Not connected";
}

QImage MainWindow::changeBrightness(QImage &image, int factor)
{
    double r,g,b;
    int pixels;
    unsigned int *data;

    // Obtaining the image dimensions
    pixels = image.width() * image.height();

    // Creating a byte array
    data = (unsigned int *)image.bits();

    // Changing each bits brightness
    for(int i = 0; i < pixels; i++)
    {
        // Manipulating each bits RGB color components
        r = qRed(data[i])+factor;

        // makes sure values are in the range of 0 - 255
        if(r < 0)
            r = 0;
        else if(r > 255)
            r =  255;

        g = qGreen(data[i])+factor;
        if( g < 0)
            g = 0;
        else if(g > 255)
            g =  255;

        b = qBlue(data[i])+factor;
        if(b < 0)
            b = 0;
        else if(b > 255)
            b =  255;

        data[i] = qRgb(r,g,b);

    }
    return image;
}

// Calculating the image contrast change
QImage MainWindow::changeContrast(QImage &image, int factor)
{
    double r,g,b, contrast;
    int pixels;
    unsigned int *data;

    contrast = (100.0+factor)/100;

    // Obtaining the image dimensions
    pixels = image.width() * image.height();

    // Creating a byte array
    data = (unsigned int *)image.bits();

    // Changing each bits brightness
    for(int i = 0; i < pixels; i++)
    {
        // Manipulating each bits RGB color components
        r = (contrast*(qRed(data[i]))) + factor;

        // makes sure values are in the range of 0 - 255
        if(r < 0)
            r = 0;
        else if(r > 255)
            r =  255;

        g = (contrast*(qGreen(data[i]))) + factor;
        if(g < 0)
            g = 0;
        else if(g > 255)
            g =  255;

        b = (contrast*(qBlue(data[i]))) + factor;
        if(b < 0)
            b = 0;
        else if(b > 255)
            b =  255;

        data[i] = qRgba(r,g,b, qAlpha(data[i]));

    }
    return image;
}


// Brightness Slider
void MainWindow::on_horizontalSlider_valueChanged(int value)
{

    QImage image;
    QPixmap pix;
    int temp1, temp2;
    int zero = 0;
    //QString s, temp1, temp2;

    image = ui->ogPic->pixmap()->toImage();
    image = changeBrightness(image, value);
    pix = QPixmap::fromImage(image);
    ui->ogPic->setPixmap(pix);
    ui->label_3->setPixmap(pix);

    // Converting the integer to string for the byte array
    //s = QString::number(value);

    // Copying the last 2 bytes of the array so they are not lost when inserting new values
    temp1 = Data[2];
    temp2 = Data[3];

    // Insert string value into the first two bytes
    Data.insert(0, value);

    // Replacing the last 2 bytes with the original values
    Data.insert(2, temp1);
    Data.insert(3, temp2);


    // If the value is a single digit number x, format to be 0X
    if(value < 10)
    {
        Data.insert(0, zero);

        // Replacing the last 2 bytes with the original values
        Data.insert(2, temp1);
        Data.insert(3, temp2);
    }

    // Writing the slider value to the server
    Data.resize(4);
    socket->write(Data);
    qDebug() << Data;
}

// Contrast Slider
void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{

    QImage image;
    QPixmap pix;
    QString s;
    int zero = 0;

    image = ui->overPic->pixmap()->toImage();
    image = changeContrast(image, value);
    pix = QPixmap::fromImage(image);
    ui->overPic->setPixmap(pix);
    ui->label_3->setPixmap(pix);
    //ui->label_4->setPixmap(pix);

    // Converting the integer to string for byte array
    //s = QString::number(value);

    // Insert string value into the last two bytes
    Data.insert(2, value);

    // If the value is a single digit number X, format to be 0X
    if(value < 10)
    {
        Data.insert(2, zero);
    }

    // Writing the slider value to the server
    Data.resize(4);
    socket->write(Data);
    //socket->waitForBytesWritten(5000);
    qDebug() << Data;
}

// Save Image Button
void MainWindow::on_pushButton_5_clicked()
{
    QString fileName;

    // Saving the modified image under a pre-described name
    fileName = QFileDialog::getSaveFileName(this, "Image", "modifiedImage.bmp", "*.bmp");
    ui->label_3->pixmap()->save(fileName);

}

// Toggle FPGA Overlay
void MainWindow::on_pushButton_6_clicked()
{
    // Send character d to toggle FPGA overlay,
    // where Data[0] = d toggles overlay ON
    // where Data[0] = e toggles overlay OFF

    Data.insert(0, "d");
    Data.resize(4);
    socket->write(Data);


    if(on == true)
    {
        Data.insert(0, "e");
        Data.resize(4);
        socket->write(Data);
        on = false;
    }

    on = true;

    if(Data[0] == 'e')
    on = false;

}

