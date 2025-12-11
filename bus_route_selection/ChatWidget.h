#pragma once
#include <QWidget>
#include <QTextBrowser>
#include <QTextEdit> 
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
#include <QKeyEvent> 

class ChatWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChatWidget(QWidget* parent = nullptr);
    ~ChatWidget();

    //API与URL
    const QString API_KEY = "sk-f9db67db1d404e3085b1ff8078bbdab1";
    const QString API_URL = "https://api.deepseek.com/chat/completions";


protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void sendMessage();
    void onApiReply(QNetworkReply* reply);
    void closeChat();

private:
    void setupUi();
    void appendMessage(const QString& role, const QString& text);

    
    QTextBrowser* m_chatDisplay;
    QTextEdit* m_inputEdit; 
    QPushButton* m_sendBtn;
    QPushButton* m_closeBtn;

    QNetworkAccessManager* m_networkManager;
    QJsonArray m_history;
};