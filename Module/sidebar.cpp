#include "sidebar.h"
#include "ui_sidebar.h"
#include <QPropertyAnimation>
#include <qdebug.h>
#include <QFileDialog>
#include <QDirIterator>
#include <QStandardItemModel>
Sidebar::Sidebar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Sidebar)
{
    ui->setupUi(this);
    // Connect the resize signal of the ui->slide widget to a slot that resizes the ui->dir_tree widget

    //connect(ui->background, &QWidget::resize, this, &Sidebar::resizeDirTree);


}

Sidebar::~Sidebar()
{
    delete ui;
}

//当用户点击add_file pushbutton时，读取对应视频的文件夹，
// 将文件夹下对应的视频文件输出到ui->dir_tree 的treeViem列表下




