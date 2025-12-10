#include "ChatWidget.h"
#include <QMessageBox>
#include <QDebug>
#include <QLabel> 
#include <QScrollBar>

ChatWidget::ChatWidget(QWidget* parent) : QWidget(parent) {
    // 样式美化
    this->setStyleSheet(
        "QWidget { background-color: #F5F5F5; border: 1px solid #ccc; border-radius: 8px; }" // 整体浅灰背景
        "QTextBrowser { border: none; background-color: #F5F5F5; padding: 10px; }" // 聊天背景同色
        "QTextEdit { border: 1px solid #ddd; border-radius: 4px; padding: 5px; background-color: white; }" // 输入框白色
        "QPushButton { background-color: #0078d7; color: white; border-radius: 4px; padding: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #005a9e; }"
        "#closeBtn { background-color: #ff4d4f; }"
    );

    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &ChatWidget::onApiReply);

    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = "你是一个智能公交助手。";
    m_history.append(systemMsg);

    setupUi();
}

ChatWidget::~ChatWidget() {}

void ChatWidget::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10); // 设置边距

    // 1. 顶部栏
    QHBoxLayout* topLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("DeepSeek 智能助手");
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333; border: none;");
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    m_closeBtn = new QPushButton("×"); // 简化为关闭符号
    m_closeBtn->setObjectName("closeBtn");
    m_closeBtn->setFixedSize(30, 30);
    connect(m_closeBtn, &QPushButton::clicked, this, &ChatWidget::closeChat);
    topLayout->addWidget(m_closeBtn);

    // 2. 聊天显示区
    m_chatDisplay = new QTextBrowser();
    m_chatDisplay->setOpenExternalLinks(true); // 允许点链接

    // 3. 底部输入区
    QHBoxLayout* inputLayout = new QHBoxLayout();

    // [修改] 使用 QTextEdit 实现多行输入
    m_inputEdit = new QTextEdit();
    m_inputEdit->setPlaceholderText("请输入问题 (Enter 发送, Ctrl+Enter 换行)");
    m_inputEdit->setFixedHeight(50); // 固定高度，类似单行但可换行
    // 安装事件过滤器，为了捕获回车键
    m_inputEdit->installEventFilter(this);

    m_sendBtn = new QPushButton("发送");
    m_sendBtn->setFixedSize(60, 50); // 高度与输入框一致
    connect(m_sendBtn, &QPushButton::clicked, this, &ChatWidget::sendMessage);

    inputLayout->addWidget(m_inputEdit);
    inputLayout->addWidget(m_sendBtn);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_chatDisplay);
    mainLayout->addSpacing(5);
    mainLayout->addLayout(inputLayout);
}

// [新增] 事件过滤器：处理输入框的回车键
bool ChatWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_inputEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        // 如果按下 Enter 且没有按 Ctrl -> 发送消息
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (!(keyEvent->modifiers() & Qt::ControlModifier)) {
                sendMessage();
                return true; // 事件已处理，不再传递
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

// [核心修改] 构建精美的气泡 HTML
void ChatWidget::appendMessage(const QString& role, const QString& text) {
    bool isUser = (role == "user");

    // 1. 设置对齐方式
    // 用户：div 右对齐；AI：div 左对齐
    QString align = isUser ? "right" : "left";

    // 2. 设置气泡颜色
    // 用户：微信绿 (#95EC69)；AI：纯白 (#FFFFFF)
    QString bubbleColor = isUser ? "#95EC69" : "#FFFFFF";

    // 3. 设置头像/名称文本
    QString name = isUser ? "我" : "DeepSeek";

    // 4. 核心 HTML 构造
    // max-width: 280px 大约对应 15-20 个汉字的宽度，超过会自动换行
    // word-wrap: break-word 确保长英文单词也能换行
    // display: inline-block 确保气泡大小根据文字内容自适应，而不是占满整行
    QString html = QString(
        "<div style='width: 100%%; margin-top: 10px; margin-bottom: 10px;' align='%1'>" // 外层 div 控制左右
            // 名字行
        "<div style='font-size: 10px; color: #888; margin-bottom: 2px;'>%2</div>"
        // 气泡本体
        "<div style='"
        "background-color: %3;"
        "border-radius: 6px;"  // 圆角
        "padding: 8px 12px;"   // 内边距
        "color: #000;"         // 字体黑色
        "font-size: 14px;"
        "text-align: left;"    // 气泡内文字永远左对齐
        "max-width: 280px;"    // [重点] 限制最大宽度，强制换行
        "word-wrap: break-word;" // [重点] 强制断行
        "'>%4</div>"
        "</div>"
    ).arg(align, name, bubbleColor, text); // 注意 text 需要做 HTML 转义，但 Qt 的 QTextBrowser 通常能处理基础文本

    // 插入 HTML
    m_chatDisplay->append(html);

    // 自动滚动到底部
    m_chatDisplay->verticalScrollBar()->setValue(m_chatDisplay->verticalScrollBar()->maximum());
}

void ChatWidget::sendMessage() {
    QString text = m_inputEdit->toPlainText().trimmed(); // [修改] 使用 toPlainText
    if (text.isEmpty()) return;

    appendMessage("user", text);
    m_inputEdit->clear();
    m_inputEdit->setDisabled(true);
    m_sendBtn->setText("...");

    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = text;
    m_history.append(userMsg);

    QUrl url(API_URL);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + API_KEY).toUtf8());

    QJsonObject jsonBody;
    jsonBody["model"] = "deepseek-chat";
    jsonBody["messages"] = m_history;
    jsonBody["stream"] = false;

    m_networkManager->post(request, QJsonDocument(jsonBody).toJson());
}

void ChatWidget::onApiReply(QNetworkReply* reply) {
    m_inputEdit->setDisabled(false);
    m_sendBtn->setText("发送");
    m_inputEdit->setFocus(); // 发送完焦点回到输入框

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();

        if (jsonObj.contains("choices") && jsonObj["choices"].isArray()) {
            QJsonArray choices = jsonObj["choices"].toArray();
            if (!choices.isEmpty()) {
                QJsonObject firstChoice = choices[0].toObject();
                QJsonObject message = firstChoice["message"].toObject();
                QString content = message["content"].toString();

                // 处理换行符，因为 HTML 需要 <br>
                content.replace("\n", "<br>");

                appendMessage("assistant", content);

                QJsonObject aiMsg;
                aiMsg["role"] = "assistant";
                aiMsg["content"] = content;
                m_history.append(aiMsg);
            }
        }
    }
    else {
        appendMessage("assistant", "API 请求失败: " + reply->errorString());
    }
    reply->deleteLater();
}

void ChatWidget::closeChat() {
    this->hide();
}