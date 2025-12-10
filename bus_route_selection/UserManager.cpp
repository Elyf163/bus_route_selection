#include "UserManager.h"
#include <QDebug>

UserManager& UserManager::instance() {
    static UserManager ins;
    return ins;
}

UserManager::UserManager() {
    // 动态获取路径：exe所在目录 + users.json
    m_jsonPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/users.json");
    loadUsers();
}

void UserManager::loadUsers() {
    QFile file(m_jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开用户数据库:" << m_jsonPath;
        return;
    }

    m_users.clear();
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());

    if (doc.isArray()) {
        QJsonArray arr = doc.array();
        for (const auto& val : arr) {
            QJsonObject obj = val.toObject();
            UserInfo u;
            u.username = obj["username"].toString();
            u.password = obj["password"].toString();
            u.role = obj["role"].toString();
            m_users.append(u);
        }
    }
    file.close();
}

bool UserManager::verifyUser(const QString& username, const QString& password, const QString& role) {
    // 遍历内存中的列表，查找匹配项
    for (const auto& user : m_users) {
        if (user.username == username &&
            user.password == password &&
            user.role == role) {
            return true; // 账号、密码、角色全部匹配
        }
    }
    return false; // 未找到
}