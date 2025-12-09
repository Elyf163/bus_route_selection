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
#include <QCoreApplication> // [新增]
#include <QDir>             // [新增]

// --- [核心函数] 动态获取资源路径 ---
QString getMediaPath(const QString& fileName) {
    // 自动找到 exe 旁边的 media 文件夹
    return QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/media/" + fileName);
}

// --- 全局样式 ---
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
    initAudio();
    setupUi();
    resize(900, 700);
    playBackgroundMusic();
}

bus_route_selection::~bus_route_selection()
{
}

void bus_route_selection::initAudio() {
    m_bgmPlayer = new QMediaPlayer(this);
    m_bgmOutput = new QAudioOutput(this);
    m_bgmPlayer->setAudioOutput(m_bgmOutput);
    m_bgmOutput->setVolume(0.3);
    m_bgmPlayer->setLoops(QMediaPlayer::Infinite);

    // [修改] 使用相对路径
    m_bgmPlayer->setSource(QUrl::fromLocalFile(getMediaPath("Background_Fragments.mp3")));

    m_voicePlayer = new QMediaPlayer(this);
    m_voiceOutput = new QAudioOutput(this);
    m_voicePlayer->setAudioOutput(m_voiceOutput);
    m_voiceOutput->setVolume(1.0);

    m_isMusicOn = true;
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
        m_bgmPlayer->play();
        // [修改] 使用相对路径
        m_musicBtn->setIcon(QIcon(getMediaPath("music_on.png")));
        m_musicBtn->setToolTip("关闭背景音乐");
    }
    else {
        m_bgmPlayer->pause();
        // [修改] 使用相对路径
        m_musicBtn->setIcon(QIcon(getMediaPath("music_off.png")));
        m_musicBtn->setToolTip("开启背景音乐");
    }
}

void bus_route_selection::playVoice(const QString& fileName) { // 参数名改为 fileName 更贴切
    m_voicePlayer->stop();
    // [修改] 自动拼接路径
    m_voicePlayer->setSource(QUrl::fromLocalFile(getMediaPath(fileName)));
    m_voicePlayer->play();
}

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

    m_musicBtn = new QPushButton(this);
    m_musicBtn->setFixedSize(50, 50);
    // [修改] 使用相对路径
    m_musicBtn->setIcon(QIcon(getMediaPath("music_on.png")));
    m_musicBtn->setIconSize(QSize(30, 30));
    m_musicBtn->setCursor(Qt::PointingHandCursor);
    m_musicBtn->setToolTip("背景音乐开关");
    m_musicBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #E6F7FF; "
        "   border: 2px solid #003366; "
        "   border-radius: 25px; "
        "}"
        "QPushButton:hover { background-color: #BAE7FF; }"
    );

    connect(m_musicBtn, &QPushButton::clicked, this, &bus_route_selection::toggleBgm);
}

void bus_route_selection::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    int x = this->width() - m_musicBtn->width() - 20;
    int y = this->height() - m_musicBtn->height() - 20;
    m_musicBtn->move(x, y);
    m_musicBtn->raise();
}

void bus_route_selection::setupSelectionUi() {
    selectionPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(selectionPage);

    QLabel* logoLabel = new QLabel();
    logoLabel->setFixedSize(150, 150);
    // [修改] 使用相对路径
    QPixmap logoPix(getMediaPath("school_badge.png"));
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

void bus_route_selection::handleLogin() {
    QString user = userEdit->text().trimmed();
    QString pass = passEdit->text().trimmed();

    if (m_isAdminLogin) {
        if (user == ADMIN_USER && pass == ADMIN_PASS) {
            // [修改] 仅传文件名
            playVoice("admin_window.mp3");
            showAdminPanel();
        }
        else {
            QMessageBox::critical(this, "错误", "管理员账号或密码错误！");
        }
    }
    else {
        QRegularExpression regex("^[0-9]{6}$");
        if (regex.match(user).hasMatch() && regex.match(pass).hasMatch()) {
            // [修改] 仅传文件名
            playVoice("user_window.mp3");
            showUserPanel();
        }
        else {
            QMessageBox::warning(this, "格式错误", "普通用户账号和密码必须均为6位数字！");
        }
    }
}

void bus_route_selection::handleForgotPwd() {
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
    userPage = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(userPage);

    QLabel* bannerLabel = new QLabel();
    bannerLabel->setFixedHeight(110);
    bannerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // [修改] 使用相对路径
    QPixmap bannerPix(getMediaPath("school_badge_name.png"));
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
        // [修改] 仅传文件名
        playVoice("cannot_find_route.mp3");

        resultDisplay->append("<h3 style='color:red'>未找到可行路线。</h3><p>原因可能为：<br>1. 两个站点间无法通过公交换乘到达。<br>2. 需要超过2次换乘(系统限制)。</p>");
        return;
    }

    // [修改] 仅传文件名
    playVoice("find_route.mp3");

    resultDisplay->clear();
    resultDisplay->append(QString("<h3 style='color:#003366'>查询结果: 从 %1 到 %2 (共 %3 种方案)</h3>")
        .arg(start, end).arg(results.size()));

    int index = 1;
    for (const auto& res : results) {
        QString html;

        // --- 核心修复：使用 align="left" 属性强制左对齐 ---

        // 1. 容器层强制左对齐
        if (res.isRecommended) {
            // [修改点] 添加 align='left' 属性，这是最稳的修复
            html += QString("<div align='left' style='background-color:#E6F7FF; border:1px solid #1890FF; padding:10px; margin-bottom:10px;'>");

            // [修改点] 标题单独包裹一个 div 并强制左对齐
            html += QString("<div align='left'><b style='color:#FF0000; font-size:16px;'>【最优推荐】 方案 %1</b>").arg(index++);
            html += QString(" <span style='color:#666; font-size:12px;'>(耗时最短)</span></div>");
        }
        else {
            // [修改点] 添加 align='left' 属性
            html += QString("<div align='left' style='border-bottom:1px solid #ccc; padding:10px; margin-bottom:10px;'>");

            // [修改点] 标题单独包裹一个 div 并强制左对齐
            html += QString("<div align='left'><b>方案 %1:</b></div>").arg(index++);
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

        // 底部统计保持右对齐 (使用 align='right' 更稳定)
        html += QString("<div align='right' style='font-weight:bold; color:#333;'>总计: %1 站 | 预计耗时: <font color='#E65100'>%2 分钟</font></div>")
            .arg(res.totalStops).arg(res.totalTime);

        html += "</div>"; // 关闭外层 div

        // 强制插入一个清除属性，确保下一个循环从左边开始 (双重保险)
        html += "<div style='clear:both;'></div>";

        resultDisplay->append(html);
    }
}

void bus_route_selection::searchLine() {
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
    adminPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(adminPage);

    QLabel* bannerLabel = new QLabel();
    bannerLabel->setFixedHeight(110);
    bannerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // [修改] 使用相对路径
    QPixmap bannerPix(getMediaPath("school_badge_name.png"));
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