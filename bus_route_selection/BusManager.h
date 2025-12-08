#pragma once
#include <QObject>
#include "BusStructs.h" // 确保引用了更新后的结构体
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QSet>

class BusManager : public QObject {
    Q_OBJECT

public:
    static BusManager& instance(); // 单例模式

    // --- 文件操作 ---
    void loadRoutes();
    void saveRoutes();

    // --- 路线管理 (管理员) ---
    void addRoute(const BusRoute& route);
    QList<BusRoute> getAllRoutes() const;
    BusRoute getRouteById(const QString& id);

    // --- 查询功能 (用户) ---
    // 1. 线路查询 (查询 D1 的所有站点)
    QList<QString> getStopsByRouteId(const QString& routeId);

    // 2. 路线规划 (支持多级换乘 BFS)
    QList<RouteResult> findPath(const QString& start, const QString& end);

private:
    BusManager();

    // 成员变量
    QList<BusRoute> m_routes;
    const QString FILE_NAME = "routes.json";

    // --- 内部辅助函数 (之前报错就是因为缺这个声明) ---
    QList<BusRoute> getRoutesByStation(const QString& stationName);
};