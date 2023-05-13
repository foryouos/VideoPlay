#ifndef WIDGET_H
#define WIDGET_H

#include <QTimer>
#include <QWidget>
#include <qpropertyanimation.h>
#include "mwidgetbase.h"
#include "readthread.h"  //读取视频解码线程
#include "playimage.h"
#include "controlbar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public MWidgetBase
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget() override;

private slots:
    void init();               // 初始化
    void connectSlot();        // 绑定信号槽
    void loadStyle();          // 加载样式表
    void setWindowTitle(const QString& title);
    void windowLayout();       // 界面布局
    void dynamicShowNormal();
    void dynamicShow();
    void dynamicHide();
    void on_playState(ReadThread::PlayState state);  //播放状态

    void on_openfile_clicked();  //视频中间打开视频文件


    void on_opendir_clicked();    //点击用户目录
    //连接鼠标点击TreeView
    void handleTreeViewClicked(const QModelIndex &index);  //鼠标点击获取treeview的内容地址，并修改视频播放

    void on_but_speed_clicked();
    //更新播放位置
    void updatePosition(qint64 position);
    void updateLabel();

protected:
    void showEvent(QShowEvent *event)     override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    void videoWidgetEvent(QEvent* event);             // 视频显示窗口事件处理

private:
    Ui::Widget *ui;
    ReadThread* m_readThread = nullptr;  //全局线程变量
    QPropertyAnimation* m_paSlider = nullptr;              // 动画对象，负责打开关闭进度条窗口动画
    QPropertyAnimation* m_paControlBar = nullptr;          // 动画对象，负责打开关闭控制栏窗口动画
    QPropertyAnimation* m_paSidebar = nullptr;             // 动画对象，负责打开关闭侧边栏窗口动画
    bool m_visible = true;
    QString com_url;  //存储文件地址和url的私有函数变量
};
#endif // WIDGET_H
