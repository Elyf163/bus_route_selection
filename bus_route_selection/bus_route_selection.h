#pragma once
#include <QtWidgets/QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <QTextBrowser>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>

// 引入多媒体模块
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QResizeEvent> 

// 引入聊天组件
#include "ChatWidget.h"

//用户数据管理
#include "UserManager.h"

class bus_route_selection : public QMainWindow
{
    Q_OBJECT

public:
    bus_route_selection(QWidget* parent = nullptr);
    ~bus_route_selection();

    const QString RECOVERY_CODE = "Elysia"; // 找回码

protected:
    // [注意] 确保这里只有这一行 resizeEvent 声明
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void showSelectionPage();
    void showUserLoginPage();
    void showAdminLoginPage();
    void handleLogin();
    void handleForgotPwd();
    void showUserPanel();
    void showAdminPanel();
    void searchRoute();
    void searchLine();
    void saveNewRoute();

    // --- 音频相关槽函数 ---
    void initAudio();
    void playBackgroundMusic();
    void stopBackgroundMusic();
    void toggleBgm();
    void playVoice(const QString& filePath);

    // --- 聊天相关 ---
    void toggleChat();

private:
    void setupUi();
    void setupSelectionUi();
    void setupLoginUi();
    void setupUserUi();
    void setupAdminUi();
    void refreshAdminTable();

    // --- 成员变量 ---
    QStackedWidget* stackedWidget;

    // 音乐控制按钮
    QPushButton* m_musicBtn;
    bool m_isMusicOn;

    // 聊天控制按钮
    QPushButton* m_chatTriggerBtn;
    ChatWidget* m_chatWidget;

    // 音频播放器
    QMediaPlayer* m_bgmPlayer;
    QAudioOutput* m_bgmOutput;
    QMediaPlayer* m_voicePlayer;
    QAudioOutput* m_voiceOutput;

    // 界面组件指针
    QWidget* selectionPage;
    QWidget* loginPage;
    QLabel* loginTitleLabel;
    QLineEdit* userEdit;
    QLineEdit* passEdit;
    QPushButton* forgotBtn;
    bool m_isAdminLogin;

    QWidget* userPage;
    QLineEdit* startInput;
    QLineEdit* endInput;
    QTextBrowser* resultDisplay;
    QLineEdit* lineQueryInput;

    QWidget* adminPage;
    QLineEdit* routeIdEdit;
    QLineEdit* stationsEdit;
    QTableWidget* routeTable;
};