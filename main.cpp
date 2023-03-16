#include <QApplication>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsBlurEffect>
#include <QLabel>
#include <QPainter>
#include <QSlider>

class BlurImageApp : public QWidget
{
private:
    Q_OBJECT
    QLabel *label = nullptr;
    QSlider *slider = nullptr;
    QPushButton *openFileButton = nullptr;
    QPushButton *saveFileButton = nullptr;
    QHBoxLayout *buttonBox = nullptr;
    QVBoxLayout *vBox = nullptr;
    QImage picture;
    QString openFilePath;
    QString saveFilePath;
    QGraphicsBlurEffect blur;
    int width;
    int height;
public:
    BlurImageApp(QWidget *parent = nullptr);
    ~BlurImageApp();
    QImage blurImage(QImage pic, int blurRadius);
public slots:
    void openFile();
    void saveFile();
    void setBlurValue(int value);
};

BlurImageApp::BlurImageApp(QWidget *parent) : QWidget(parent)
{
    vBox = new QVBoxLayout(this);
    label = new QLabel(this);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label->setText("Press \"Open image\" to choose picture you want to blur.");
    slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(0);
    slider->setMaximum(10);
    openFileButton = new QPushButton("Open image", this);
    saveFileButton = new QPushButton("Save image", this);
    buttonBox = new QHBoxLayout(this);
    buttonBox->addWidget(openFileButton);
    buttonBox->addWidget(saveFileButton);
    vBox->addWidget(label);
    vBox->addWidget(slider);
    vBox->addLayout(buttonBox);

    QObject::connect(openFileButton, &QPushButton::clicked, this, &BlurImageApp::openFile);
    QObject::connect(saveFileButton, &QPushButton::clicked, this, &BlurImageApp::saveFile);
    QObject::connect(slider, &QSlider::valueChanged, this, &BlurImageApp::setBlurValue);
}

BlurImageApp::~BlurImageApp() {}

QImage BlurImageApp::blurImage(QImage pic, int blurRadius)
{
    if(pic.isNull()) return QImage();
    QGraphicsScene scene;
    QGraphicsPixmapItem grItem;
    grItem.setPixmap(QPixmap::fromImage(pic));
    blur.setBlurRadius(blurRadius);
    grItem.setGraphicsEffect(&blur);
    scene.addItem(&grItem);
    QImage result(pic.size(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);
    QPainter painter(&result);
    scene.render(&painter, QRectF(), QRectF( 0, 0, pic.width(), pic.height()));
    return result;
}

void BlurImageApp::openFile()
{
    openFilePath = QFileDialog::getOpenFileName(nullptr,
                                                "Open image file",
                                                "C:\\",
                                                "JPG file (*.jpg);;PNG file (*.png)");
    if (openFilePath.isEmpty())
    {
        label->clear();
        label->setText("Press \"Open image\" to choose picture you want to blur.");
        return;
    }
    picture = QImage(openFilePath);
    width = picture.width();
    height = picture.height();
    if (width > label->width() || height > label->height())
    {
        width = label->width();
        height = label->height();
    }
    label->clear();
    label->setPixmap(QPixmap::fromImage(picture).scaled(width, height, Qt::KeepAspectRatio));
    slider->setValue(0);
}

void BlurImageApp::saveFile()
{
    if (!picture.isNull())
    {
        saveFilePath = QFileDialog::getSaveFileName(nullptr,
                                                    "Save file",
                                                    "C:\\",
                                                    "JPG file (*.jpg);;PNG file (*.png)");
        if (!saveFilePath.isEmpty())
        {
            auto blured = blurImage(picture, slider->value());
            blured.save(saveFilePath);
        }
    }
}

void BlurImageApp::setBlurValue(int value)
{
    if (!picture.isNull())
    {
        blur.setBlurRadius(value);
        label->setGraphicsEffect(&blur);
        //label->clear();
        //label->setPixmap(QPixmap::fromImage(blurImage(picture, value)).scaled(width, height, Qt::KeepAspectRatio));
    }
    else
        slider->setValue(0);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    BlurImageApp blurApp;
    blurApp.resize(1280, 720);
    blurApp.show();
    return app.exec();
}

#include "main.moc"
