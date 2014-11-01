#ifndef MPLAYER_H
#define MPLAYER_H

#include <QDialog>
#include <QIcon>
#include <QProcess>
#include <QTimer>
#include <QTime>
#include <QDir>
#include <QString>
#include <QStringList>
#include "ui_mplayer.h"

namespace Ui {
class MPlayer;
}

class MPlayer : public QDialog,private Ui_MPlayer
{
    Q_OBJECT

public:
    explicit MPlayer(QWidget *parent = 0);
    ~MPlayer();
    QTime int_to_time(int);             //

public slots:
    void play_pause_slots();		// 暂停
    void stop_slots();					// 停止
    void previous_slots();			// 上一曲
    void next_slots();                  // 下一曲
    void seek_slots(int);                 // 快进
    void get_time_slots();              // 获取播放时间
    void set_volume_slots(int);        //  设置音量
    void set_sound_slots();         // 静音
    void playerReward_slots();  // 快退
    void playerForward_slots();     // 快进
    void back_message_slots();      // 更新显示信息

private:
    Ui::MPlayer *ui;
    QProcess    *process;
    QStringList files;
    QDir    directory;
    int     file_count;
    QString file_name;
    bool    isPlay;
    bool    isSound;
    bool    isStop;
    QTimer  *timer;
    int     file_length;
    int     curr_time;
};

#endif // MPLAYER_H
