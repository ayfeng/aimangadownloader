#include <fstream>
#include <vector>
#include <string>
#include "../../src/curlpprequest.cpp"
#include "aidownloader.h"
#include "ui_aidownloader.h"
#include <QStringListModel>
#include <QList>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
#include <QObject>
#include <QByteArray>
#include <QImage>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>


AiDownloader::AiDownloader(QWidget *parent) :
    ui(new Ui::AiDownloader)
{
    ui->setupUi(this);
    active_list = new std::vector<QString>();
}

AiDownloader::~AiDownloader()
{
    delete ui;
    delete active_list;
}

void AiDownloader::updateMangaList() {
    QFile input_file("mangalist");
    if (input_file.open(QIODevice::ReadOnly)) {
        QTextStream in(&input_file);
        while (!in.atEnd())
            active_list->push_back(in.readLine().split("|").at(0));
        input_file.close();
    } else {
        CurlRequest curl = CurlRequest();
        std::vector<QString> manga_data = curl.requestList();
        for (auto i = manga_data.begin(); i != manga_data.end(); ++i)
            active_list->push_back(i->split("|").at(0));
    }
    ui->listView->setModel(new QStringListModel(QList<QString>::fromVector(QVector<QString>::fromStdVector(*active_list))));
}

void AiDownloader::on_listView_clicked() {
    QModelIndex selected = ui->listView->selectionModel()->currentIndex();
    QFile input_file("mangalist");
    CurlRequest curl = CurlRequest();
    if (input_file.open(QIODevice::ReadOnly)) {
        QTextStream in(&input_file);
        for (int i = 0; i < selected.row(); ++i, in.readLine());
        QStringList manga_data = in.readLine().split("|"); //TODO
        QString url = manga_data.value(manga_data.length()-1);

        QVector<QPair<QString, QString>> chapter_data = curl.getChapters(url);
        QVector<QString> chapter_url;
        std::transform(chapter_data.begin(), chapter_data.end(), std::back_inserter(chapter_url), [](const QPair<QString, QString>& a) { return a.second; });
        qDebug() << chapter_url;
        QStringListModel* model = new QStringListModel(QList<QString>::fromVector(chapter_url));
        qDebug() << model->setData(selected, QString("HI"), Qt::StatusTipRole); //Qt::StatusTipRole
        ui->tabWidget->findChild<QListView*>("chapters")->setModel(model);
        qDebug() << model->data(selected, Qt::StatusTipRole).toString();

//        xmlpp::NodeSet image_links = curl.getChapterImages(chapter_url);
//        curl.getAllImages(image_links);

//        QNetworkAccessManager* access_manager = new QNetworkAccessManager();
//        QNetworkRequest request(curl.getImageLink(url));
//        QNetworkReply* reply = access_manager->get(request);
//        QEventLoop loop;
//        QTimer timer;
//        timer.setSingleShot(true);
//        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
//        QObject::connect(access_manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
//        timer.start(6000); //3s timeout
//        loop.exec();
//
//        if (timer.isActive())
//            timer.stop(); //download complete
//        else
//            std::cout << "TIME OUT" << std::endl;
//
//        QByteArray bytes = reply->readAll();
//        QImage image(20, 20, QImage::Format_Indexed8);
//        image.loadFromData(bytes);
//        ui->label_2->setScaledContents(true);
//        ui->label_2->setPixmap(QPixmap::fromImage(image));
    }
}

void AiDownloader::on_chapters_clicked() {
    QModelIndex selected = ui->listView->selectionModel()->currentIndex();
    QFile input_file("mangalist");
    CurlRequest curl = CurlRequest();
    if (input_file.open(QIODevice::ReadOnly)) {
        QTextStream in(&input_file);
        for (int i = 0; i < selected.row(); ++i, in.readLine());
        QStringList manga_data = in.readLine().split("|"); //TODO
        QString url = manga_data.value(manga_data.length()-1);

        QVector<QPair<QString, QString>> chapter_data = curl.getChapters(url);
        QVector<QUrl> chapter_url;
        std::transform(chapter_data.begin(), chapter_data.end(), std::back_inserter(chapter_url), [](const QPair<QString, QString>& a) { return a.first; });
        QModelIndex selected_chapter = ui->chapters->selectionModel()->currentIndex();
        qDebug() << "P1 " << chapter_url[0];
        qDebug() << "P2 " << chapter_url.size() << " " << selected_chapter.row();
        curl.getAllImages(curl.getChapterImages(chapter_url[selected_chapter.row()]));
        qDebug() << "GOT IMAGE " << chapter_url[selected_chapter.row()];
    }
}
