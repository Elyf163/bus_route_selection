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
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QResizeEvent> 
#include <QMouseEvent> 
#include "ChatWidget.h"
#include "UserManager.h"


class DraggableButton : public QPushButton {
public:
    explicit DraggableButton(QWidget* parent = nullptr) : QPushButton(parent) {}
    explicit DraggableButton(const QString& text, QWidget* parent = nullptr) : QPushButton(text, parent) {}

protected:
    // 鼠标按下：记录鼠标在按钮内部的相对位置
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            m_isDragging = true;
            // 记录偏移量 = 鼠标全局位置 - 按钮左上角位置
            m_dragOffset = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
        }
        QPushButton::mousePressEvent(event); 
    }

    // 鼠标移动：更新按钮位置
    void mouseMoveEvent(QMouseEvent* event) override {
        if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
            // 新位置 = 当前鼠标全局位置 - 初始偏移量
            this->move(event->globalPosition().toPoint() - m_dragOffset);
            // 阻止事件继续传播，防止触发点击
            return;
        }
        QPushButton::mouseMoveEvent(event);
    }

    // 鼠标释放
    void mouseReleaseEvent(QMouseEvent* event) override {
        m_isDragging = false;
        QPushButton::mouseReleaseEvent(event);
    }

private:
    bool m_isDragging = false;
    QPoint m_dragOffset;
};



class bus_route_selection : public QMainWindow
{
    Q_OBJECT

public:
    bus_route_selection(QWidget* parent = nullptr);
    ~bus_route_selection();

    const QString RECOVERY_CODE = "Elysia"; // 找回码

protected:
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
    DraggableButton* m_musicBtn;       // 音乐按钮
   
    bool m_isMusicOn;

    // 聊天控制按钮
    DraggableButton* m_chatTriggerBtn; // 聊天呼出按钮
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