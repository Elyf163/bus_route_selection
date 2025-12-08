#pragma once
#include <QString>
#include <QList>
#include <QTime>

// 站点信息
struct Station {
    QString name;
    int timeFromStart;
};

// 线路信息
struct BusRoute {
    QString routeId;
    QList<Station> stations;
    QTime firstBus;
    QTime lastBus;
};

// 路径中的一段 (例如 D1: 迈皋桥 -> 新街口)
struct PathSegment {
    QString routeId;
    QString startStation;
    QString endStation;
    int stops;
    int timeCost;
};

// 最终搜索结果
struct RouteResult {
    QList<PathSegment> segments; // 由多段组成 (直达只有1段，换乘有2段或3段)
    int totalStops;
    int totalTime;      // 总耗时 (含换乘等待)
    bool isRecommended; // 是否为推荐路线
};