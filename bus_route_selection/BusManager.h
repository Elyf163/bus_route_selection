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
    //获取所有不重复的站点名称，用于自动补全
    QList<QString> getAllStations();
    QList<RouteResult> findPath(const QString& start, const QString& end);

private:
    BusManager();

    QList<BusRoute> m_routes;
    QString m_jsonPath;

    QList<BusRoute> getRoutesByStation(const QString& stationName);
};