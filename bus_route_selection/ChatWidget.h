#pragma once
#include <QWidget>
#include <QTextBrowser>
#include <QTextEdit> // [修改] 替换 QLineEdit
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel> 
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest> 
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QKeyEvent> // [新增] 用于处理键盘事件

class ChatWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChatWidget(QWidget* parent = nullptr);
    ~ChatWidget();

    // 你的 API Key
    const QString API_KEY = "sk-f9db67db1d404e3085b1ff8078bbdab1";
    const QString API_URL = "https://api.deepseek.com/chat/completions";

protected:
    // [新增] 事件过滤器，用于拦截输入框的回车键
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void sendMessage();
    void onApiReply(QNetworkReply* reply);
    void closeChat();

private:
    void setupUi();
    void appendMessage(const QString& role, const QString& text);

    // UI 组件
    QTextBrowser* m_chatDisplay;
    QTextEdit* m_inputEdit; // [修改] 变为多行文本框
    QPushButton* m_sendBtn;
    QPushButton* m_closeBtn;

    QNetworkAccessManager* m_networkManager;
    QJsonArray m_history;
};