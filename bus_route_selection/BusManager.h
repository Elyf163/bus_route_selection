#pragma once
#include <QObject>
#include "BusStructs.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QSet>
#include <QCoreApplication> // [新增]
#include <QDir>             // [新增]

class BusManager : public QObject {
    Q_OBJECT

public:
    static BusManager& instance();

    void loadRoutes();
    void saveRoutes();

    void addRoute(const BusRoute& route);
    QList<BusRoute> getAllRoutes() const;
    BusRoute getRouteById(const QString& id);

    QList<QString> getStopsByRouteId(const QString& routeId);
    QList<RouteResult> findPath(const QString& start, const QString& end);

private:
    BusManager();

    QList<BusRoute> m_routes;

    // [修改] 不再是 const 常量，而是变量，因为要在构造函数里动态赋值
    QString m_jsonPath;

    QList<BusRoute> getRoutesByStation(const QString& stationName);
};