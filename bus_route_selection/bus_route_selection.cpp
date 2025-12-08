#include "bus_route_selection.h"
#include "BusManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>
#include <QFormLayout>
#include <QInputDialog>
#include <QRegularExpression>
#include <QPixmap>
#include <QIcon>

// --- 全局样式 (保持不变) ---
const QString GLOBAL_STYLE = R"(
    QMainWindow, QWidget {
        background-color: #FFFFFF;
        color: #333333;
        font-family: "Microsoft YaHei", Arial;
    }
    QPushButton {
        border: 2px solid #003366;
        border-radius: 6px;
        padding: 8px 15px;
        background-color: #FFFFFF;
        color: #003366;
        font-weight: bold;
    }
    QPushButton:hover {
        background-color: #E6F0FF;
    }
    QPushButton:pressed {
        background-color: #003366;
        color: white;
    }
    QLineEdit {
        border: 2px solid #003366;
        border-radius: 4px;
        padding: 5px;
        selection-background-color: #003366;
    }
    QGroupBox {
        border: 2px solid #003366;
        border-radius: 6px;
        margin-top: 20px;
        font-weight: bold;
        color: #003366;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        subcontrol-position: top center;
        padding: 0 5px;
    }
)";

bus_route_selection::bus_route_selection(QWidget* parent)
    : QMainWindow(parent)
{
    this->setStyleSheet(GLOBAL_STYLE);

    // 1. 先初始化音频系统
    initAudio();

    // 2. 构建界面 (包含悬浮按钮的创建)
    setupUi();

    resize(900, 700);

    // 3. 默认开启背景音乐
    playBackgroundMusic();
}

bus_route_selection::~bus_route_selection()
{
}

// --- 音频系统初始化 ---
void bus_route_selection::initAudio() {
    // 配置 BGM
    m_bgmPlayer = new QMediaPlayer(this);
    m_bgmOutput = new QAudioOutput(this);
    m_bgmPlayer->setAudioOutput(m_bgmOutput);
    m_bgmOutput->setVolume(0.3); // 背景音乐音量小一点
    m_bgmPlayer->setLoops(QMediaPlayer::Infinite); // 无限循环

    // TODO: 请替换为背景音乐的绝对路径
    m_bgmPlayer->setSource(QUrl::fromLocalFile("E:/B24011015/soft_design/audio/Background_Fragments.mp3"));

    // 配置 语音
    m_voicePlayer = new QMediaPlayer(this);
    m_voiceOutput = new QAudioOutput(this);
    m_voicePlayer->setAudioOutput(m_voiceOutput);
    m_voiceOutput->setVolume(1.0); // 语音音量最大

    m_isMusicOn = true; // 默认状态
}

void bus_route_selection::playBackgroundMusic() {
    if (m_isMusicOn && m_bgmPlayer->playbackState() != QMediaPlayer::PlayingState) {
        m_bgmPlayer->play();
    }
}

void bus_route_selection::stopBackgroundMusic() {
    m_bgmPlayer->pause();
}

void bus_route_selection::toggleBgm() {
    m_isMusicOn = !m_isMusicOn;

    if (m_isMusicOn) {
        // 开启音乐
        m_bgmPlayer->play();
        // TODO: 请替换为“音乐开启”图标路径
        m_musicBtn->setIcon(QIcon("E:/B24011015/soft_design/music_on.png"));
        m_musicBtn->setToolTip("关闭背景音乐");
    }
    else {
        // 关闭音乐
        m_bgmPlayer->pause();
        // TODO: 请替换为“音乐关闭”图标路径
        m_musicBtn->setIcon(QIcon("E:/B24011015/soft_design/music_off.png"));
        m_musicBtn->setToolTip("开启背景音乐");
    }
}

void bus_route_selection::playVoice(const QString& filePath) {
    m_voicePlayer->stop();
    m_voicePlayer->setSource(QUrl::fromLocalFile(filePath));
    m_voicePlayer->play();
}

// --- 界面构建 ---
void bus_route_selection::setupUi() {
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupSelectionUi();
    setupLoginUi();
    setupUserUi();
    setupAdminUi();

    stackedWidget->addWidget(selectionPage);
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(userPage);
    stackedWidget->addWidget(adminPage);

    stackedWidget->setCurrentWidget(selectionPage);

    // --- 创建右下角悬浮音乐按钮 ---
    // 注意：这里父对象设为 this (QMainWindow)，而不是 stackedWidget
    m_musicBtn = new QPushButton(this);
    m_musicBtn->setFixedSize(50, 50);
    // 设置初始图标 (TODO: 替换路径)
    m_musicBtn->setIcon(QIcon("E:/B24011015/soft_design/music_on.png"));
    m_musicBtn->setIconSize(QSize(30, 30));
    m_musicBtn->setCursor(Qt::PointingHandCursor);
    m_musicBtn->setToolTip("背景音乐开关");
    // 设置圆形样式
    m_musicBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #E6F7FF; "
        "   border: 2px solid #003366; "
        "   border-radius: 25px; " // 半径为宽高的一半，即圆形
        "}"
        "QPushButton:hover { background-color: #BAE7FF; }"
    );

    connect(m_musicBtn, &QPushButton::clicked, this, &bus_route_selection::toggleBgm);
}

// --- 核心：重写 resizeEvent 以固定按钮位置 ---
void bus_route_selection::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    // 计算位置：右下角，距离右边和底部各 20 像素
    int x = this->width() - m_musicBtn->width() - 20;
    int y = this->height() - m_musicBtn->height() - 20;

    m_musicBtn->move(x, y);
    m_musicBtn->raise(); // 确保按钮在最上层
}

// ... setupSelectionUi, setupLoginUi 等保持原有逻辑 ...
void bus_route_selection::setupSelectionUi() {
    selectionPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(selectionPage);

    QLabel* logoLabel = new QLabel();
    logoLabel->setFixedSize(150, 150);
    QPixmap logoPix("E:/B24011015/soft_design/school_badge.png");
    if (!logoPix.isNull()) {
        logoLabel->setPixmap(logoPix.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    logoLabel->setAlignment(Qt::AlignCenter);

    QLabel* title = new QLabel("欢迎使用公交路线系统\n请选择您的身份");
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #003366; margin-top: 20px;");
    title->setAlignment(Qt::AlignCenter);

    QPushButton* btnUser = new QPushButton("我是普通用户");
    btnUser->setMinimumHeight(60);
    btnUser->setStyleSheet("font-size: 18px;");
    connect(btnUser, &QPushButton::clicked, this, &bus_route_selection::showUserLoginPage);

    QPushButton* btnAdmin = new QPushButton("我是管理员");
    btnAdmin->setMinimumHeight(60);
    btnAdmin->setStyleSheet("font-size: 18px;");
    connect(btnAdmin, &QPushButton::clicked, this, &bus_route_selection::showAdminLoginPage);

    layout->addStretch();
    layout->addWidget(logoLabel, 0, Qt::AlignCenter);
    layout->addWidget(title);
    layout->addSpacing(40);
    layout->addWidget(btnUser);
    layout->addWidget(btnAdmin);
    layout->addStretch();
    layout->setContentsMargins(200, 50, 200, 50);
}

void bus_route_selection::setupLoginUi() {
    // 保持原有代码，无需修改
    loginPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(loginPage);

    QHBoxLayout* topLayout = new QHBoxLayout();
    QPushButton* backBtn = new QPushButton("<< 返回");
    backBtn->setFixedSize(120, 35);
    backBtn->setStyleSheet("border: none; color: #003366; text-align: left; font-weight: normal;");
    connect(backBtn, &QPushButton::clicked, this, &bus_route_selection::showSelectionPage);
    topLayout->addWidget(backBtn);
    topLayout->addStretch();

    loginTitleLabel = new QLabel("登录");
    loginTitleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #003366; margin-bottom: 20px;");
    loginTitleLabel->setAlignment(Qt::AlignCenter);

    userEdit = new QLineEdit();
    passEdit = new QLineEdit();
    passEdit->setEchoMode(QLineEdit::Password);

    QPushButton* loginBtn = new QPushButton("登 录");
    loginBtn->setMinimumHeight(45);
    loginBtn->setStyleSheet("background-color: #003366; color: white; font-size: 16px;");
    connect(loginBtn, &QPushButton::clicked, this, &bus_route_selection::handleLogin);

    forgotBtn = new QPushButton("忘记密码？");
    forgotBtn->setFlat(true);
    forgotBtn->setStyleSheet("border: none; color: #666; text-decoration: underline;");
    connect(forgotBtn, &QPushButton::clicked, this, &bus_route_selection::handleForgotPwd);

    layout->addLayout(topLayout);
    layout->addStretch();
    layout->addWidget(loginTitleLabel);
    layout->addWidget(userEdit);
    layout->addWidget(passEdit);
    layout->addSpacing(20);
    layout->addWidget(loginBtn);
    layout->addWidget(forgotBtn);
    layout->addStretch();
    layout->setContentsMargins(200, 20, 200, 50);
}

void bus_route_selection::showSelectionPage() {
    stackedWidget->setCurrentWidget(selectionPage);
}

void bus_route_selection::showUserLoginPage() {
    m_isAdminLogin = false;
    loginTitleLabel->setText("普通用户登录");
    userEdit->setPlaceholderText("请输入6位数字账号");
    passEdit->setPlaceholderText("请输入6位数字密码");
    userEdit->clear();
    passEdit->clear();
    forgotBtn->hide();
    stackedWidget->setCurrentWidget(loginPage);
}

void bus_route_selection::showAdminLoginPage() {
    m_isAdminLogin = true;
    loginTitleLabel->setText("管理员登录");
    userEdit->setPlaceholderText("管理员账号");
    passEdit->setPlaceholderText("管理员密码");
    userEdit->clear();
    passEdit->clear();
    forgotBtn->show();
    stackedWidget->setCurrentWidget(loginPage);
}

// --- 登录处理 (添加语音播报) ---
void bus_route_selection::handleLogin() {
    QString user = userEdit->text().trimmed();
    QString pass = passEdit->text().trimmed();

    if (m_isAdminLogin) {
        if (user == ADMIN_USER && pass == ADMIN_PASS) {
            // 管理员登录成功语音 (TODO: 替换路径)
            playVoice("E:/B24011015/soft_design/audio/admin_window.mp3");
            showAdminPanel();
        }
        else {
            QMessageBox::critical(this, "错误", "管理员账号或密码错误！");
        }
    }
    else {
        QRegularExpression regex("^[0-9]{6}$");
        if (regex.match(user).hasMatch() && regex.match(pass).hasMatch()) {
            // 普通用户登录成功语音 (TODO: 替换路径)
            playVoice("E:/B24011015/soft_design/audio/user_window.mp3");
            showUserPanel();
        }
        else {
            QMessageBox::warning(this, "格式错误", "普通用户账号和密码必须均为6位数字！");
        }
    }
}

void bus_route_selection::handleForgotPwd() {
    // 保持原有代码
    bool ok;
    QString code = QInputDialog::getText(this, "找回密码",
        "请输入找回码 (Recovery Code):",
        QLineEdit::Normal, "", &ok);
    if (ok && !code.isEmpty()) {
        if (code == RECOVERY_CODE) {
            QString msg = QString("验证成功！\n\n您的管理员密码是: %1").arg(ADMIN_PASS);
            QMessageBox::information(this, "密码找回", msg);
        }
        else {
            QMessageBox::warning(this, "错误", "找回码错误！请联系开发者。");
        }
    }
}

void bus_route_selection::setupUserUi() {
    // 保持原有代码
    userPage = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(userPage);

    QLabel* bannerLabel = new QLabel();
    bannerLabel->setFixedHeight(110);
    bannerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QPixmap bannerPix("E:/B24011015/soft_design/school_badge_name.png");
    if (!bannerPix.isNull()) {
        bannerLabel->setPixmap(bannerPix.scaledToHeight(100, Qt::SmoothTransformation));
        bannerLabel->setAlignment(Qt::AlignCenter);
    }
    else {
        bannerLabel->setText("BANNER 加载失败");
    }

    QHBoxLayout* topLayout = new QHBoxLayout();
    QLabel* welcomeMsg = new QLabel("当前用户: 普通用户");
    welcomeMsg->setStyleSheet("font-style: italic; color: #555;");
    QPushButton* backBtn = new QPushButton("退出登录");
    connect(backBtn, &QPushButton::clicked, this, &bus_route_selection::showSelectionPage);
    topLayout->addWidget(welcomeMsg);
    topLayout->addStretch();
    topLayout->addWidget(backBtn);

    QGroupBox* pathGroup = new QGroupBox("路线规划 (点到点)");
    QHBoxLayout* pathLayout = new QHBoxLayout(pathGroup);
    startInput = new QLineEdit(); startInput->setPlaceholderText("起点站");
    endInput = new QLineEdit(); endInput->setPlaceholderText("终点站");
    QPushButton* searchBtn = new QPushButton("查询方案");
    connect(searchBtn, &QPushButton::clicked, this, &bus_route_selection::searchRoute);
    pathLayout->addWidget(new QLabel("从"));
    pathLayout->addWidget(startInput);
    pathLayout->addWidget(new QLabel("到"));
    pathLayout->addWidget(endInput);
    pathLayout->addWidget(searchBtn);

    QGroupBox* lineGroup = new QGroupBox("车次查询 (查线路)");
    QHBoxLayout* lineLayout = new QHBoxLayout(lineGroup);
    lineQueryInput = new QLineEdit(); lineQueryInput->setPlaceholderText("线路号 (如: D1)");
    QPushButton* lineBtn = new QPushButton("查询详情");
    connect(lineBtn, &QPushButton::clicked, this, &bus_route_selection::searchLine);
    lineLayout->addWidget(new QLabel("线路:"));
    lineLayout->addWidget(lineQueryInput);
    lineLayout->addWidget(lineBtn);

    resultDisplay = new QTextBrowser();
    resultDisplay->setStyleSheet("border: 2px solid #003366; background-color: #FAFAFA;");

    mainLayout->addWidget(bannerLabel, 0, Qt::AlignHCenter);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(pathGroup);
    mainLayout->addWidget(lineGroup);
    mainLayout->addWidget(resultDisplay);
}

// --- 路线查询 (添加语音播报) ---
void bus_route_selection::searchRoute() {
    QString start = startInput->text().trimmed();
    QString end = endInput->text().trimmed();

    if (start.isEmpty() || end.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入起点和终点");
        return;
    }

    resultDisplay->clear();
    resultDisplay->append(QString("<h3 style='color:#003366'>正在查询: 从 %1 到 %2 ...</h3>").arg(start, end));

    auto results = BusManager::instance().findPath(start, end);

    if (results.isEmpty()) {
        // 失败语音 (TODO: 替换路径)
        playVoice("E:/B24011015/soft_design/audio/cannot_find_route.mp3");

        resultDisplay->append("<h3 style='color:red'>未找到可行路线。</h3><p>原因可能为：<br>1. 两个站点间无法通过公交换乘到达。<br>2. 需要超过2次换乘(系统限制)。</p>");
        return;
    }

    // 成功语音 (TODO: 替换路径)
    playVoice("E:/B24011015/soft_design/audio/find_route.mp3");

    resultDisplay->clear();
    resultDisplay->append(QString("<h3 style='color:#003366'>查询结果: 从 %1 到 %2 (共 %3 种方案)</h3>")
        .arg(start, end).arg(results.size()));

    int index = 1;
    for (const auto& res : results) {
        QString html;
        if (res.isRecommended) {
            html += QString("<div style='background-color:#E6F7FF; border:1px solid #1890FF; padding:10px; margin-bottom:10px;'>");
            html += QString("<b style='color:#FF0000; font-size:16px;'>【最优推荐】 方案 %1</b>").arg(index++);
            html += QString(" <span style='color:#666; font-size:12px;'>(耗时最短)</span><br>");
        }
        else {
            html += QString("<div style='border-bottom:1px solid #ccc; padding:10px; margin-bottom:10px;'>");
            html += QString("<b>方案 %1:</b><br>").arg(index++);
        }

        html += "<ul style='margin-top:5px;'>";
        for (int i = 0; i < res.segments.size(); ++i) {
            const auto& seg = res.segments[i];
            html += QString("<li><b>第 %1 程:</b> 乘坐 <font color='#0078d7'><b>%2</b></font> 从 %3 到 <b>%4</b> (%5 站, %6 分钟)</li>")
                .arg(i + 1)
                .arg(seg.routeId)
                .arg(seg.startStation)
                .arg(seg.endStation)
                .arg(seg.stops)
                .arg(seg.timeCost);

            if (i < res.segments.size() - 1) {
                html += QString("<div style='color:#666; margin-left:20px;'>⬇ 在 <b>%1</b> 换乘 (预计等待 5 分钟)</div>")
                    .arg(seg.endStation);
            }
        }
        html += "</ul>";
        html += QString("<div style='text-align:right; font-weight:bold; color:#333;'>总计: %1 站 | 预计耗时: <font color='#E65100'>%2 分钟</font></div>")
            .arg(res.totalStops).arg(res.totalTime);
        html += "</div>";
        resultDisplay->append(html);
    }
}

void bus_route_selection::searchLine() {
    // 保持原有代码
    QString id = lineQueryInput->text().trimmed();
    auto stops = BusManager::instance().getStopsByRouteId(id);

    resultDisplay->clear();
    if (stops.isEmpty()) {
        resultDisplay->append("未找到该线路信息。");
        return;
    }

    QString html = QString("<h3 style='color:#003366'>线路 %1 详情:</h3>").arg(id);
    html += stops.join(" -> ");
    BusRoute r = BusManager::instance().getRouteById(id);
    if (!r.routeId.isEmpty()) {
        html += QString("<br><br>首班车: %1, 末班车: %2")
            .arg(r.firstBus.toString("HH:mm"), r.lastBus.toString("HH:mm"));
    }
    resultDisplay->append(html);
}

void bus_route_selection::setupAdminUi() {
    // 保持原有代码
    adminPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(adminPage);

    QLabel* bannerLabel = new QLabel();
    bannerLabel->setFixedHeight(110);
    bannerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QPixmap bannerPix("E:/B24011015/soft_design/school_badge_name.png");
    if (!bannerPix.isNull()) {
        bannerLabel->setPixmap(bannerPix.scaledToHeight(100, Qt::SmoothTransformation));
        bannerLabel->setAlignment(Qt::AlignCenter);
    }
    else {
        bannerLabel->setText("BANNER 加载失败");
    }

    QHBoxLayout* top = new QHBoxLayout();
    QLabel* title = new QLabel("路线管理后台");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #003366;");
    QPushButton* logout = new QPushButton("退出管理");
    connect(logout, &QPushButton::clicked, this, &bus_route_selection::showSelectionPage);
    top->addWidget(title);
    top->addStretch();
    top->addWidget(logout);

    QGroupBox* inputGroup = new QGroupBox("录入/更新路线");
    QFormLayout* form = new QFormLayout(inputGroup);

    routeIdEdit = new QLineEdit();
    stationsEdit = new QLineEdit();
    stationsEdit->setPlaceholderText("StopA,StopB,StopC (用逗号分隔)");

    QPushButton* saveBtn = new QPushButton("保存路线");
    connect(saveBtn, &QPushButton::clicked, this, &bus_route_selection::saveNewRoute);

    form->addRow("线路名称:", routeIdEdit);
    form->addRow("途径站点:", stationsEdit);
    form->addRow("", saveBtn);

    routeTable = new QTableWidget();
    routeTable->setColumnCount(3);
    routeTable->setHorizontalHeaderLabels({ "线路", "首末班", "站点预览" });
    routeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    routeTable->setStyleSheet("border: 2px solid #003366; selection-background-color: #003366;");

    layout->addWidget(bannerLabel);
    layout->addLayout(top);
    layout->addWidget(inputGroup);
    layout->addWidget(routeTable);
}

void bus_route_selection::showAdminPanel() {
    stackedWidget->setCurrentWidget(adminPage);
    refreshAdminTable();
}

void bus_route_selection::showUserPanel() {
    stackedWidget->setCurrentWidget(userPage);
}

void bus_route_selection::saveNewRoute() {
    QString id = routeIdEdit->text().trimmed();
    QString stopsStr = stationsEdit->text().trimmed();

    if (id.isEmpty() || stopsStr.isEmpty()) return;

    BusRoute r;
    r.routeId = id;
    r.firstBus = QTime(6, 0);
    r.lastBus = QTime(22, 0);

    QStringList parts = stopsStr.split(",");
    int timeAcc = 0;
    for (const auto& s : parts) {
        if (s.trimmed().isEmpty()) continue;
        r.stations.append({ s.trimmed(), timeAcc });
        timeAcc += 5;
    }

    BusManager::instance().addRoute(r);
    QMessageBox::information(this, "成功", "路线已保存");
    refreshAdminTable();

    routeIdEdit->clear();
    stationsEdit->clear();
}

void bus_route_selection::refreshAdminTable() {
    auto routes = BusManager::instance().getAllRoutes();
    routeTable->setRowCount(0);

    for (const auto& r : routes) {
        int row = routeTable->rowCount();
        routeTable->insertRow(row);

        routeTable->setItem(row, 0, new QTableWidgetItem(r.routeId));
        routeTable->setItem(row, 1, new QTableWidgetItem(r.firstBus.toString("HH:mm") + "-" + r.lastBus.toString("HH:mm")));

        QString stopPreview;
        for (const auto& s : r.stations) stopPreview += s.name + " ";
        routeTable->setItem(row, 2, new QTableWidgetItem(stopPreview));
    }
} 