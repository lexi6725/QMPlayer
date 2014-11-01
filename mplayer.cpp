#include "mplayer.h"
#include "ui_mplayer.h"
#include <QDebug>
#include <unistd.h>

MPlayer::MPlayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MPlayer)
{
    ui->setupUi(this);
    isPlay  = true;
    isSound = true;
    isStop  = false;

    /**************** 为按键添加图标***************************/
    //play
    ui->pushButton_play->setIcon(QIcon(":/images/play_enabled.ico"));
    ui->pushButton_play->setIconSize(QSize(QPixmap(":/images/play_enabled.ico").size()));

    //stop
    ui->pushButton_stop->setIcon(QIcon(":/images/stop_enabled.ico"));
    ui->pushButton_stop->setIconSize(QSize(QPixmap(":/images/stop_enabled.ico").size()));

    //reward
   ui->pushButton_reward->setIcon(QIcon(":/images/reward_enabled.ico"));
   ui->pushButton_reward->setIconSize(QSize(QPixmap(":/images/reward_enabled.ico").size()));

   //forward
   ui->pushButton_forward->setIcon(QIcon(":/images/forward_enabled.ico"));
   ui->pushButton_forward->setIconSize(QSize(QPixmap(":/images/forward_enabled.ico").size()));

   //sound
  ui-> pushButton_sound->setIcon(QIcon(":/images/sound_enabled.png"));
  ui->pushButton_sound->setIconSize(QSize(QPixmap(":/images/forward_enabled.ico").size()));

  //previous
 ui->pushButton_pre->setIcon(QIcon(":/images/previous_enabled.ico"));
  ui->pushButton_pre->setIconSize(QSize(QPixmap(":/images/previous_enabled.ico").size()));

   //next
  ui->pushButton_next->setIcon(QIcon(":/images/next_enabled.ico"));
   ui->pushButton_next->setIconSize(QSize(QPixmap(":/images/next_enabled.ico").size()));

   /***************设置按钮无边框×××××××××××××××××××××××××××××××××××*/
   ui->pushButton_play->setFlat(true);
   ui->pushButton_stop->setFlat(true);
   ui->pushButton_reward->setFlat(true);
   ui->pushButton_forward->setFlat(true);
   ui->pushButton_sound->setFlat(true);
   ui->pushButton_pre->setFlat(true);
  ui-> pushButton_next->setFlat(true);

   /****************获得播放列表*************************************/
   directory.setPath("/work/sound");
   files    = directory.entryList(QDir::AllEntries, QDir::Time);
   file_name    = files[2];  //文件0和1为“.”“..”，所以从文件2开始播放
    file_count  = 2;
    ui->label_name->setText(files[2]);
    qDebug()<<files[0];
    qDebug()<<files[1];
    qDebug()<<files[2];

    /***************初始化进度条及QProcess类**************************/
    ui->horizontalSlider->setPageStep(1);
    process     = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);

    /***************初始化信号、slots**********************************/
    connect(ui->pushButton_play, SIGNAL(clicked()), this, SLOT(play_pause_slots()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(stop_slots()));
    connect(ui->pushButton_reward,SIGNAL(clicked()), this, SLOT(playerReward_slots()));
    connect(ui->pushButton_forward,SIGNAL(clicked()), this, SLOT(playerForward_slots()));
    connect(ui->pushButton_pre, SIGNAL(clicked()), this, SLOT(previous_slots()));
    connect(ui->pushButton_next, SIGNAL(clicked()), this, SLOT(next_slots()));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(set_volume_slots(int)));
    connect(ui->pushButton_sound, SIGNAL(clicked()), this, SLOT(set_sound_slots()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(back_message_slots()));
    connect(process, SIGNAL(started()),this, SLOT(mplayer_running_slots()));
    connect(process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(state_change_slots()));
    // when process should read return info of Mplayer, produce readyReadStandardOutput()信号
    QString arg;
    arg ="/usr/bin/mplayer";
    QString file_path;
    file_path = " /work/sound/";
    QStringList args;
    args<<"-slave";
    args<<"-zoom";
    args<<"-quiet";
    args<<"/work/sound/a.mp4";
    args<<" -wid"+QString::number((ui->widget->winId()));
    process->start(arg, args);     // 开始运行程序
    /*QString common = "mplayer -slave -zoom -quiet /work/sound/"+file_name+" -wid "+
            QString::number(ui->widget->winId());
    process->start(common);*/
    if(process->state()==QProcess::Starting)
    {
        qDebug()<<"Process Starting";
    }
  //  process->waitForStarted(3000000);
    ui->spinBox->setValue(40);
    timer   = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(get_time_slots()));
    // 定时获取MPlayer的时间信息
    timer->start(1000);// 启动定时器1秒timeout 1次
    //ui->label_pbar->setText("00:00:00");
    //ui->label_time_l->setText("00:00:00");
    qDebug()<<"Init Success";
}

MPlayer::~MPlayer()
{
    delete ui;
}

void MPlayer::play_pause_slots()
{
    qDebug()<<"play_pause_slots";
    if(!isPlay)
    {
        if(isStop)
        {
            file_name   = files[2];
            QString common = "mplayer -slave  -quiet /work/sound"+file_name+"-wid"+
                    QString::number(ui->widget->winId())+'\n';
            process->start((common));
            ui->pushButton_stop->setIcon(QIcon(":/images/stop_enabbled.ico"));
            isStop  = false;
        }
        else
        {
            process->write("pause\n");
        }
        ui->pushButton_play->setIcon(QIcon(":/images/play_enabled.ico"));
        isPlay  = true;
    }
    else
    {
        ui->pushButton_play->setIcon(QIcon(":/images/pause_enabled.ico"));
        isPlay  = false;
        process->write("pause\n");
    }
}

void MPlayer::stop_slots()
{
    qDebug()<<"stop_slots";
    if(!isStop)
    {
        process->write("quit\n");
        ui->pushButton_play->setIcon(QIcon(":/images/play_enabled.ico"));
        isPlay  = false;
        ui->pushButton_stop->setIcon(QIcon(":/images/stop_enabled.ico"));
        isStop  = true;
        ui->label_pbar->setText("00:00:00");
        ui->label_time_l->setText("00:00:00");
    }
}

void MPlayer::previous_slots()
{
    qDebug()<<"previous_slots";
    if(file_count > 2)
    {
        if(file_count == (files.size()-1))
        {
            ui->pushButton_next->setIcon(QIcon(":/images/next_enabled.ico"));
        }
        process->write("quit\n");
        process = new QProcess(this);
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(back_message_slots));
        file_count--;
        if(!isStop)
        {
            file_name   = files[file_count];
            QString common = "mplayer -slave -quiet  /work/sound"+file_name+"-wid" +
                    QString::number(ui->widget->winId())+'\n';
            process->start(common);
        }
        if (file_count == 2)
        {
           ui-> pushButton_pre->setIcon(QIcon(":/images/previous_enabled.ico"));
        }
        ui->label_name->setText(files[file_count]);
    }
}

void MPlayer::next_slots()
{
    qDebug()<<"next_slots";
    if(file_count<(files.size()-1))
    {
        if(file_count == 2)
        {
            ui->pushButton_pre->setIcon(QIcon(":/images/previous_enabled.ico"));
        }
        process->write("quit\n");
        process->close();
        process = new QProcess(this);
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(back_message_slots()));
        file_count++;
        if(!isStop)
        {
            file_name   = files[file_count];
            QString common = "mplayer -slave /work/sound/"+file_name+"-wid "+
                    QString::number(ui->widget->winId())+'\n';
            process->start(common);
        }
        if(file_count == (files.size()-1))
        {
            ui->pushButton_next->setIcon(QIcon(":/images/next_enabled.ico"));
        }
    }
    ui->label_name->setText(files[file_count]);
}

void MPlayer::seek_slots(int seek_num)
{
    qDebug()<<seek_num;
    if(process && process->state()==QProcess::Running)
    {
        process->write(QString("seek "+QString::number(qMin(seek_num, 100))+"1\n").toLatin1());
    }
}

void MPlayer::get_time_slots()
{
    qDebug()<<"time_slots";
    if(isPlay)
    {
        process->write("get_time_length\n");
        process->write("get_time_pos\n");
        qDebug()<<"get_time";
    }
}

void MPlayer::set_volume_slots(int volume)
{
    qDebug()<<volume;
    process->write(QString("volume +" +QString::number(volume)+"\n").toLatin1());
}

void MPlayer::set_sound_slots()
{
    if(isSound)
    {
        process->write("mute 1\n");
        ui->pushButton_sound->setIcon(QIcon(":/images/sound_enabled.png"));
        isSound = false;
    }
    else
    {
        process->write("mute 0\n");
        ui->pushButton_sound->setIcon(QIcon(":/images/sound_enabled.png"));
        isSound = true;
    }
}

void MPlayer::playerReward_slots()
{
  //  if(process && process->state()==QProcess::Running && !isPlay)
    {
        qDebug()<<"Reward";
    }
}

void MPlayer::playerForward_slots()
{
    qDebug()<<"Forward";
}

void MPlayer::back_message_slots()
{
    while(process->canReadLine())
    {
        QString message(process->readLine());
        QStringList message_list = message.split("=");
        if(message_list[0] == "ANS_TIME_POSITION")
        {
            curr_time = message_list[1].toDouble();
            QTime time = int_to_time(curr_time);
            ui->label_pbar->setText(time.toString("hh:mm:ss"));
            ui->horizontalSlider->setValue(100*curr_time/file_length);
            //ui->horizontalSlider->setValue(100*14/);
            qDebug()<<"ANS_TIME_POSITION";
        }
        else if (message_list[0] == "ANS_LENGTH")
        {
            file_length = message_list[1].toDouble();
            QTime time = int_to_time(file_length);
           ui-> label_time_l->setText(time.toString("hh:mm::ss"));
           qDebug()<<"ANS_LENGTH";
        }
        qDebug()<<"Message";
    }
}

void MPlayer::mplayer_running_slots()
{
    qDebug()<<"MPlayer started";
    qDebug()<<QString::number(process->state()).toLatin1();

}

void MPlayer::state_change_slots()
{
    qDebug()<<"MPlayer state change";
}

QTime MPlayer::int_to_time(int second)
{
    int sec = 0;
    int min = 0;
    int hour = 0;
    QTime time;
    if(second < 60)
    {
        sec = second;
        min = 0;
        hour = 0;
    }
    if(second >= 60 && second < 3600)
    {
        sec = second%60;
        min = second/60;
        hour = 0;
    }
    if(second>= 3600)
    {
        sec = second%60;
        min = (second/60)%60;
        hour = second/3600;
    }
    time.setHMS(hour, min, sec);
    return time;
}
