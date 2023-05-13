#include "widget.h"
#include "ui_widget.h"
#include "QWindow"
#include <QDebug>
#include <QFile>
#include <QMouseEvent>
#include <QStyle>
#include <QFileDialog>
#include <QDirIterator>
#include <QStandardItemModel>
Widget::Widget(QWidget *parent)
    : MWidgetBase(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    init();
    connectSlot();
    //创建一个新的线程
    m_readThread = new ReadThread();
    //将线程与ui->playImage视频播放区域updateImaage相互联系
    connect(m_readThread, &ReadThread::updateImage, ui->playImage, &PlayImage::updateImage, Qt::DirectConnection);
    //线程的playState状态与本widget的
    connect(m_readThread, &ReadThread::playState, this, &Widget::on_playState);
    // Connect the clicked signal to the handleTreeViewClicked slot
    connect(ui->dirtree, &QTreeView::clicked, this, &Widget::handleTreeViewClicked);

    //让视频播放与slide相互绑定
    // Connect the valueChanged signal of the slider to the updatePosition slot
    connect(ui->slider_video, &QSlider::valueChanged, this, &Widget::updatePosition);
    //

}

Widget::~Widget()
{
    //释放视频读取线程
    if(m_readThread)
    {
        m_readThread->close();
        m_readThread->wait();
        delete m_readThread;
    }
    delete ui;
}
//桌面初始化
void Widget::init()
{
    loadStyle();
    this->setWindowTitle(QString("foryouos播放器 - V%1").arg(APP_VERSION));
    this->setTitleBar(ui->titleBar->getBackground());              // 设置标题栏
    ui->videoWidget->setMouseTracking(true);                       // 激活鼠标移动事件

    ui->videoWidget->installEventFilter(this);

    m_paSlider = new QPropertyAnimation(ui->slider_video, "size");
    m_paControlBar = new QPropertyAnimation(ui->controlBar, "size");
    m_paSidebar = new QPropertyAnimation(ui->sidebar, "pos");
    //设置持续时间
    m_paSlider->setDuration(500);
    m_paControlBar->setDuration(500);
    m_paSidebar->setDuration(500);
    ui->dirtree->setStyleSheet("QTreeView {background-color: transparent;}"
                           "QScrollBar:vertical {background-color: transparent;}"
                           "QScrollBar::handle:vertical {background-color: lightgray;}"
                           "QScrollBar::add-line:vertical {background-color: transparent;}"
                           "QScrollBar::sub-line:vertical {background-color: transparent;}");
}
//设置信号关联
void Widget::connectSlot()
{
    connect(this, &MWidgetBase::windowStateChanged, ui->titleBar, &TitleBar::on_windowStateChanged);

}

/* 获取本地视频路径，打开文件点击后，并设置自动播放
 *
 *
*/
void Widget::on_openfile_clicked()
{
    QString strName = QFileDialog::getOpenFileName(this, "选择播放视频~！", "/", "视频 (*.mp4 *.m4v *.mov *.avi *.flv);; 其它(*)");
    if(strName.isEmpty())
    {
        return;
    }
    //ui->com_url->setCurrentText(strName); //此ui充当存储视频地址功能，同时也作为传递线程打开UI的功能
    //存储到私有变量
    com_url = strName;
    qDebug()<<com_url;
    // 隐藏打开文件的窗口
    ui->openfile->hide();
    //设置自动播放
    m_readThread->open(strName); //打开线程地址

    updateLabel();  //需要在线程里面


}


/**
 * @brief 加载样式表
 */
void Widget::loadStyle()
{
    //加载样式文件
    QFile file(":/Style/main.css");
    if (file.open(QFile::ReadOnly))
    {
        //用QTextStream读取样式文件不用区分文件编码 带bom也行
        QStringList list;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line;
            in >> line;
            list << line;
        }

        file.close();
        QString qss = list.join("\n");
        this->setStyleSheet("");
        qApp->setStyleSheet(qss);  //设置样式
    }
}

/**
 * @brief       隐藏Widget设置标题文本功能，添加设置标题栏文本功能
 * @param title
 */
void Widget::setWindowTitle(const QString &title)
{
    QWidget::setWindowTitle(title);
    ui->titleBar->setWindowTitle(title);
}

/**
 * @brief  界面动态布局
 */
void Widget::windowLayout()
{
    // 设置组件大小
    int l_widget = ui->videoWidget->width() * 3 / 5;
    QSize size = ui->controlBar->size();
    size.setWidth(l_widget);
    ui->controlBar->resize(size);                        // 设置控制栏大小
    size.setHeight(ui->slider_video->height());
    ui->slider_video->resize(size);                      // 设置进度条大小

    // 设置组件位置
    int x = (ui->videoWidget->width() - ui->controlBar->width()) / 2;
    int y = ui->videoWidget->height() - ui->controlBar->height() - 20;
    ui->controlBar->move(x, y);
    int sliderY = y - ui->slider_video->height() - 5;
    ui->slider_video->move(x, sliderY);

    // 侧边栏
    size = ui->sidebar->size();
    size.setHeight(sliderY - 20);
    ui->sidebar->resize(size);
    //侧边栏的ui->dirtree也根据侧边栏变化而变化
    ui->sidebar->move(0, 0);
    m_visible = true;
    size.setHeight(sliderY-20);
    ui->dirtree->resize(size);
    ui->dirtree->move(0,0);
    //让opendir位于sidebar的下边
    ui->opendir->move(40,sliderY-70);

    //  视频大小根据videowidget的大小而变化
    ui->playImage->resize(ui->videoWidget->width(),ui->videoWidget->height());

    //设置打开文件的位置变化？
    // 重新定位 openfile 按钮到 playImage 中心
    int buttonX = (ui->playImage->width() - ui->openfile->width()) / 2;
    int buttonY = (ui->playImage->height() - ui->openfile->height()) / 2;
    ui->openfile->move(buttonX, buttonY);


}

void Widget::dynamicShowNormal()
{
    if(ui->controlBar->width() > 0)
    {
        dynamicHide();
    }
    else
    {
        dynamicShow();
    }
}


void Widget::dynamicHide()
{
    if(!m_visible)
    {
        return;
    }

    // 隐藏进度条
    m_paSlider->setStartValue(ui->slider_video->size());
    m_paSlider->setEndValue(QSize(0, ui->slider_video->height()));
    m_paSlider->setEasingCurve(QEasingCurve::OutQuad);
    m_paSlider->start();

    // 隐藏控制栏
    m_paControlBar->setStartValue(ui->controlBar->size());
    m_paControlBar->setEndValue(QSize(0, ui->controlBar->height()));
    m_paControlBar->setEasingCurve(QEasingCurve::OutQuad);
    m_paControlBar->start();

    // 隐藏侧边栏
    m_paSidebar->setStartValue(QPoint(0, 0));
    m_paSidebar->setEndValue(QPoint(-ui->sidebar->width(), 0));
    m_paSidebar->setEasingCurve(QEasingCurve::OutQuad);
    m_paSidebar->start();

    m_visible = false;
}

void Widget::dynamicShow()
{
    if(m_visible)
    {
        return;
    }

    int l_widget = ui->videoWidget->width() * 3 / 5;
    m_paSlider->setStartValue(QSize(0, ui->slider_video->height()));
    m_paSlider->setEndValue(QSize(l_widget, ui->slider_video->height()));
    m_paSlider->setEasingCurve(QEasingCurve::OutQuad);
    m_paSlider->start();

    m_paControlBar->setStartValue(QSize(0, ui->controlBar->height()));
    m_paControlBar->setEndValue(QSize(l_widget, ui->controlBar->height()));
    m_paControlBar->setEasingCurve(QEasingCurve::OutQuad);
    m_paControlBar->start();

    m_paSidebar->setStartValue(QPoint(-ui->sidebar->width(), 0));
    m_paSidebar->setEndValue(QPoint(0, 0));
    m_paSidebar->setEasingCurve(QEasingCurve::OutQuad);
    m_paSidebar->start();

    m_visible = true;
}


/**
 * @brief         窗口显示事件
 * @param event
 */
void Widget::showEvent(QShowEvent *event)
{
    MWidgetBase::showEvent(event);
    windowLayout();
}

/**
 * @brief        窗口大小改变事件
 * @param event
 */
void Widget::resizeEvent(QResizeEvent *event)
{
    MWidgetBase::resizeEvent(event);
    windowLayout();
}

/**
 * @brief         事件过滤器
 * @param watched
 * @param event
 * @return
 */
bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->videoWidget)
    {
        videoWidgetEvent(event);
    }

    return MWidgetBase::eventFilter(watched, event);
}

/**
 * @brief        处理窗口中的视频显示界面事件
 * @param event
 */
void Widget::videoWidgetEvent(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        switch (e->button())
        {
        case Qt::LeftButton:           // 左键双击全屏显示、还原
        {
            if(this->isFullScreen())
            {
                ui->titleBar->show();
                this->showNormal();
                dynamicShow();
            }
            else
            {
                ui->titleBar->hide();
                this->showFullScreen();
                dynamicHide();
            }
            break;
        }
        case Qt::RightButton:         // 右键双击显示隐藏控制栏、侧边栏、进度条
        {
            dynamicShowNormal();
            break;
        }
        default:break;
        }

        break;
    }
    case QEvent::MouseMove:
    {
        break;
    }
    default:break;
    }
}
void Widget::on_playState(ReadThread::PlayState state)
{
    if(state == ReadThread::play)
    {
        //this->setWindowTitle(QString("正在播放：%1").arg(m_readThread->url()));
        //ui->but_open->setText("停止播放");
    }
    else
    {
        //ui->but_open->setText("开始播放");
        //ui->but_pause->setText("暂停");
        //this->setWindowTitle(QString("Qt+ffmpeg视频播放（软解码）Demo V%1").arg(APP_VERSION));
    }
}



void Widget::on_opendir_clicked()
{
    // Prompt the user to select a video folder
   QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Video Folder"), QDir::homePath());

   // Create a new QStandardItemModel to hold the video files
   QStandardItemModel *model = new QStandardItemModel(this);

   // Create a new QDirIterator to iterate through the video files
   QDirIterator it(folderPath, QStringList() << "*.mp4" << "*.avi" << "*.mkv", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

   while (it.hasNext()) {
       QString filePath = it.next();

       // Create a new QStandardItem for the video file
       QStandardItem *item = new QStandardItem(it.fileName());
       // Store the file path as an additional data role
       item->setData(filePath, Qt::UserRole + 1);

       // Add the item to the model
       model->appendRow(item);
   }

   // Set the model for the QTreeView
   ui->dirtree->setModel(model);
}

void Widget::handleTreeViewClicked(const QModelIndex &index)
{

    // Retrieve the file path from the model using the clicked index
   QString filePath = index.data(Qt::UserRole + 1).toString();

   // Store the file path in a variable for later use
   // (e.g. passing it to the video playback thread)
   com_url = filePath;


   //调用线程播放对应的文件
   //如果当前播放线程打开着，贯标播放线程
    if(m_readThread->status())
    {
        //先关闭之前线程
        m_readThread->close();

    }
    while(!m_readThread->status()) { //当线程时关注的时候
        ui->openfile->hide();
        //设置自动播放
        m_readThread->open(com_url); //打开线程地址
        updateLabel();
    } // wait for thread to finish closing
//   // 隐藏打开文件的窗口
//   ui->openfile->hide();
//   //设置自动播放
//   m_readThread->open(com_url); //打开线程地址

   // Output the file name to the console or wherever you want
   QString fileName = index.data(Qt::DisplayRole).toString();
   qDebug() << "Clicked file: " <<com_url  << fileName;

}





//当点击倍速播放
void Widget::on_but_speed_clicked()
{
    m_readThread->setPlaybackSpeed(2);
}

void Widget::updatePosition(qint64 position)
{
    m_readThread->setVideoPosition(position);
}
//更新时间

void Widget::updateLabel()
{
    qint64 total_time = m_readThread->gettotaltime();
    qint64 now_time = m_readThread->getcurrenttime();
    //ui->label_time->setText(QString("%1 / %2").arg(now_time).arg(total_time));
    qDebug()<<total_time<<now_time;
}
