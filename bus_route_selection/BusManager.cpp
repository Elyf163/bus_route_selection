#include "BusManager.h"
#include <QQueue>
#include <QSet>
#include <algorithm>
#include <QDebug>

BusManager& BusManager::instance() {
    static BusManager ins;
    return ins;
}

BusManager::BusManager() {
    // [核心修改] 动态获取 exe 所在目录 + routes.json
    // QDir::toNativeSeparators 用于自动处理 Windows 的反斜杠问题
    m_jsonPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/routes.json");

    loadRoutes();
}

void BusManager::addRoute(const BusRoute& route) {
    for (int i = 0; i < m_routes.size(); ++i) {
        if (m_routes[i].routeId == route.routeId) {
            m_routes.removeAt(i);
            break;
        }
    }
    m_routes.append(route);
    saveRoutes();
}

void BusManager::saveRoutes() {
    QJsonArray arr;
    for (const auto& r : m_routes) {
        QJsonObject obj;
        obj["id"] = r.routeId;
        obj["first"] = r.firstBus.toString("HH:mm");
        obj["last"] = r.lastBus.toString("HH:mm");
        QJsonArray stationsArr;
        for (const auto& s : r.stations) {
            QJsonObject sObj;
            sObj["name"] = s.name;
            sObj["time"] = s.timeFromStart;
            stationsArr.append(sObj);
        }
        obj["stations"] = stationsArr;
        arr.append(obj);
    }
    QJsonDocument doc(arr);

    // [修改] 使用动态路径 m_jsonPath
    QFile file(m_jsonPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void BusManager::loadRoutes() {
    QFile file(m_jsonPath);
    if (!file.open(QIODevice::ReadOnly)) return;

    m_routes.clear();
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray arr = doc.array();
    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        BusRoute r;
        r.routeId = obj["id"].toString();
        r.firstBus = QTime::fromString(obj["first"].toString(), "HH:mm");
        r.lastBus = QTime::fromString(obj["last"].toString(), "HH:mm");
        QJsonArray sArr = obj["stations"].toArray();
        for (const auto& sVal : sArr) {
            QJsonObject sObj = sVal.toObject();
            r.stations.append({ sObj["name"].toString(), sObj["time"].toInt() });
        }
        m_routes.append(r);
    }

    // 2. [新增] 自动生成返程路线
    QList<BusRoute> reverseRoutes; // 临时存一下，防止遍历时修改容器导致迭代器失效

    for (const auto& originalRoute : m_routes) {
        // 如果已经是自动生成的返程路线，就跳过，防止无限循环
        if (originalRoute.routeId.endsWith("_REV")) continue;

        BusRoute revRoute;
        // 1. 设置 ID，例如 "D1" 变成 "D1_REV"
        revRoute.routeId = originalRoute.routeId + "_REV";

        // 2. 设置首末班时间 (返程通常也是这个时间，或者你可以倒过来)
        revRoute.firstBus = originalRoute.firstBus;
        revRoute.lastBus = originalRoute.lastBus;

        // 3. 反转站点
        // 原本: A(0) -> B(5) -> C(15)
        // 目标: C(0) -> B(10) -> A(15) 
        // 算法: 新的累积时间 = 总时间 - 原本该站的时间

        int totalTime = originalRoute.stations.last().timeFromStart; // 总耗时

        // 从后往前遍历原路线
        for (int i = originalRoute.stations.size() - 1; i >= 0; --i) {
            Station s = originalRoute.stations[i];

            // 计算新的累积时间
            // 例如 C站原为15，Total 15。新时间 = 15 - 15 = 0
            // 例如 B站原为5， Total 15。新时间 = 15 - 5 = 10
            int newTime = totalTime - s.timeFromStart;

            revRoute.stations.append({ s.name, newTime });
        }

        reverseRoutes.append(revRoute);
    }

    // 3. 将生成的返程路线合并到主列表中
    for (const auto& r : reverseRoutes) {
        addRoute(r); // 假设你有一个 addRoute 函数将路线加入 m_routes 和 map
    }
}

// ... 下面的函数保持不变 ...
QList<BusRoute> BusManager::getAllRoutes() const { return m_routes; }

BusRoute BusManager::getRouteById(const QString& id) {
    for (const auto& r : m_routes) if (r.routeId == id) return r;
    return BusRoute();
}

QList<QString> BusManager::getStopsByRouteId(const QString& routeId) {
    QList<QString> list;
    for (const auto& r : m_routes) {
        if (r.routeId == routeId) {
            for (const auto& s : r.stations) list << s.name;
            return list;
        }
    }
    return list;
}

QList<BusRoute> BusManager::getRoutesByStation(const QString& stationName) {
    QList<BusRoute> list;
    for (const auto& r : m_routes) {
        for (const auto& s : r.stations) {
            if (s.name == stationName) {
                list.append(r);
                break;
            }
        }
    }
    return list;
}

QList<RouteResult> BusManager::findPath(const QString& start, const QString& end) {
    QList<RouteResult> results;
    if (start == end) return results;

    struct SearchNode {
        QString currentStation;
        QList<PathSegment> history;
        QSet<QString> visitedRoutes;
    };

    QQueue<SearchNode> queue;

    QList<BusRoute> startRoutes = getRoutesByStation(start);
    for (const auto& route : startRoutes) {
        int startIdx = -1;
        for (int i = 0; i < route.stations.size(); ++i) if (route.stations[i].name == start) startIdx = i;

        for (int i = startIdx + 1; i < route.stations.size(); ++i) {
            QString nextStation = route.stations[i].name;
            int timeCost = route.stations[i].timeFromStart - route.stations[startIdx].timeFromStart;

            PathSegment seg;
            seg.routeId = route.routeId;
            seg.startStation = start;
            seg.endStation = nextStation;
            seg.stops = i - startIdx;
            seg.timeCost = timeCost;

            SearchNode node;
            node.currentStation = nextStation;
            node.history.append(seg);
            node.visitedRoutes.insert(route.routeId);

            queue.enqueue(node);
        }
    }

    const int MAX_SEGMENTS = 3;

    while (!queue.isEmpty()) {
        SearchNode current = queue.dequeue();

        if (current.currentStation == end) {
            RouteResult res;
            res.segments = current.history;
            res.totalStops = 0;
            res.totalTime = 0;
            res.isRecommended = false;

            for (const auto& seg : res.segments) {
                res.totalStops += seg.stops;
                res.totalTime += seg.timeCost;
            }
            if (res.segments.size() > 1) {
                res.totalTime += (res.segments.size() - 1) * 5;
            }
            results.append(res);
            continue;
        }

        if (current.history.size() >= MAX_SEGMENTS) continue;

        QList<BusRoute> nextRoutes = getRoutesByStation(current.currentStation);
        for (const auto& route : nextRoutes) {
            if (current.visitedRoutes.contains(route.routeId)) continue;

            int currIdx = -1;
            for (int i = 0; i < route.stations.size(); ++i) if (route.stations[i].name == current.currentStation) currIdx = i;

            for (int i = currIdx + 1; i < route.stations.size(); ++i) {
                QString nextStation = route.stations[i].name;
                bool isBackTrack = false;
                for (const auto& h : current.history) if (h.startStation == nextStation) isBackTrack = true;
                if (isBackTrack) continue;

                PathSegment seg;
                seg.routeId = route.routeId;
                seg.startStation = current.currentStation;
                seg.endStation = nextStation;
                seg.stops = i - currIdx;
                seg.timeCost = route.stations[i].timeFromStart - route.stations[currIdx].timeFromStart;

                SearchNode nextNode = current;
                nextNode.currentStation = nextStation;
                nextNode.history.append(seg);
                nextNode.visitedRoutes.insert(route.routeId);

                queue.enqueue(nextNode);
            }
        }
    }

    for (int i = 0; i < results.size(); ++i) {
        for (int j = i + 1; j < results.size(); ) {
            bool samePath = true;
            if (results[i].segments.size() != results[j].segments.size()) samePath = false;
            else {
                for (int k = 0; k < results[i].segments.size(); ++k) {
                    if (results[i].segments[k].routeId != results[j].segments[k].routeId) {
                        samePath = false; break;
                    }
                }
            }
            if (samePath) results.removeAt(j);
            else ++j;
        }
    }

    std::sort(results.begin(), results.end(), [](const RouteResult& a, const RouteResult& b) {
        return a.totalTime < b.totalTime;
        });

    if (!results.isEmpty()) results[0].isRecommended = true;

    return results;
}
//获取所有不重复的站点名称
QList<QString> BusManager::getAllStations() {
    QSet<QString> uniqueStations; // 使用 Set 自动去重
    for (const auto& route : m_routes) {
        for (const auto& station : route.stations) {
            uniqueStations.insert(station.name);
        }
    }
    // 将 Set 转换为 List 返回
    return uniqueStations.values();
}