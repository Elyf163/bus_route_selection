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

class bus_route_selection : public QMainWindow
{
    Q_OBJECT

public:
    bus_route_selection(QWidget* parent = nullptr);
    ~bus_route_selection();

    // --- 管理员账户配置区域 ---
    // 你可以在这里修改管理员的账号和密码
    const QString ADMIN_USER = "LYF";
    const QString ADMIN_PASS = "elyfsian";
    const QString RECOVERY_CODE = "Elysia"; // 找回码
    // ------------------------

private slots:
    // 页面跳转槽
    void showSelectionPage();      // 返回选择界面
    void showUserLoginPage();      // 进入用户登录页
    void showAdminLoginPage();     // 进入管理员登录页

    // 功能槽
    void handleLogin();            // 处理登录逻辑
    void handleForgotPwd();        // 找回密码

    // 业务槽
    void showUserPanel();
    void showAdminPanel();
    void searchRoute();
    void searchLine();
    void saveNewRoute();

private:
    void setupUi();
    void setupSelectionUi(); // 新增：初始选择界面
    void setupLoginUi();     // 修改：通用登录界面
    void setupUserUi();
    void setupAdminUi();

    void refreshAdminTable();

    QStackedWidget* stackedWidget;

    // --- 1. 身份选择界面 ---
    QWidget* selectionPage;

    // --- 2. 登录界面 (复用) ---
    QWidget* loginPage;
    QLabel* loginTitleLabel;     // 动态标题 "管理员登录" / "用户登录"
    QLineEdit* userEdit;
    QLineEdit* passEdit;
    QPushButton* forgotBtn;      // 仅管理员显示
    bool m_isAdminLogin;         // 核心标志位：当前是否在进行管理员登录

    // --- 3. 业务界面 ---
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