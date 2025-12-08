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
#include <QResizeEvent> // 需要引入这个来处理按钮位置

class bus_route_selection : public QMainWindow
{
    Q_OBJECT

public:
    bus_route_selection(QWidget* parent = nullptr);
    ~bus_route_selection();

    // 管理员账户配置
    const QString ADMIN_USER = "LYF";
    const QString ADMIN_PASS = "elyfsian";
    const QString RECOVERY_CODE = "Elysia";

protected:
    // 重写调整大小事件，用于固定音乐按钮位置
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
    void toggleBgm();                       // 新增：切换背景音乐开关
    void playVoice(const QString& filePath);

private:
    void setupUi();
    void setupSelectionUi();
    void setupLoginUi();
    void setupUserUi();
    void setupAdminUi();
    void refreshAdminTable();

    // --- 成员变量 ---
    QStackedWidget* stackedWidget;

    // 音乐控制按钮 (悬浮)
    QPushButton* m_musicBtn;
    bool m_isMusicOn; // 记录音乐状态

    // 音频播放器
    QMediaPlayer* m_bgmPlayer;
    QAudioOutput* m_bgmOutput;
    QMediaPlayer* m_voicePlayer;
    QAudioOutput* m_voiceOutput;

    // 界面组件指针 (保持原样)
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