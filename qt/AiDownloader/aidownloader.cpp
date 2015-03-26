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
    if (input_file.open(QIODevice::ReadOnly)) {
        QTextStream in(&input_file);
        for (int i = 0; i < selected.row(); ++i, in.readLine());
        QStringList manga_data = in.readLine().split("|"); //TODO
        QString url = manga_data.value(manga_data.length()-1);
        ui->right->setModel(new QStringListModel(QList<QString>({url})));
    }
    std::cout << selected.row() << std::endl;
}
