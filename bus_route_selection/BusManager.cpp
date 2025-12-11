//寻路算法实现
#include "BusManager.h"
#include <QQueue>
#include <QSet>
#include <algorithm>
#include <QDebug>

//单例模式
BusManager& BusManager::instance() {
    static BusManager ins;
    return ins;
}

//构造函数，加载路线数据
BusManager::BusManager() {
    m_jsonPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/routes.json");
    loadRoutes();
}


//添加路径
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


//将路径保存至routes.json
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
    QFile file(m_jsonPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

//从routes.json加载路径，并生成返程路线
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

	// 生成返程路线,从而实现双向路线选择

    QList<BusRoute> reverseRoutes; // 反向路线寄存

    for (const auto& originalRoute : m_routes) {
        // 跳过自动生成路线
        if (originalRoute.routeId.endsWith("_REV")) continue;

        BusRoute revRoute;
        // 设置反向ID(加上“_REV”后缀)
        revRoute.routeId = originalRoute.routeId + "_REV";

        //设置首末班时间
        revRoute.firstBus = originalRoute.firstBus;
        revRoute.lastBus = originalRoute.lastBus;

        //反转站点
        int totalTime = originalRoute.stations.last().timeFromStart; // 总耗时

        // 从后往前遍历原路线
        for (int i = originalRoute.stations.size() - 1; i >= 0; --i) {
            Station s = originalRoute.stations[i];

            // 计算新的累积时间
            // 新的累积时间 = 总时间 - 原本该站的时间
            int newTime = totalTime - s.timeFromStart;

            revRoute.stations.append({ s.name, newTime });
        }

        reverseRoutes.append(revRoute);
    }

    //将生成的返程路线合并
    for (const auto& r : reverseRoutes) {
        addRoute(r); 
    }
}

//获取所有路线
QList<BusRoute> BusManager::getAllRoutes() const { return m_routes; }

//根据ID获取路线
BusRoute BusManager::getRouteById(const QString& id) {
    for (const auto& r : m_routes) if (r.routeId == id) return r;
    return BusRoute();
}

//根据路线ID获取站点名称列表
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

//根据站点名称获取包含该站点的所有路线
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

//寻路算法（BFS+剪枝）
QList<RouteResult> BusManager::findPath(const QString& start, const QString& end) {
    QList<RouteResult> results;
	if (start == end) return results;   // 起点终点相同，直接返回空结果
	// BFS 队列节点结构
    struct SearchNode {
		QString currentStation;             // 当前站点
		QList<PathSegment> history;         // 历史路径段
		QSet<QString> visitedRoutes;        // 已访问路线ID集合
    };
	// BFS 队列
    QQueue<SearchNode> queue;
	// 初始化队列：从起点出发的所有可能路径段
    QList<BusRoute> startRoutes = getRoutesByStation(start);
	// 将起点的所有可能路径段加入队列
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
	// 最大换乘次数限制为3，结合实际情况，避免过多换乘
    const int MAX_SEGMENTS = 3;

	// BFS 主循环
    while (!queue.isEmpty()) {
		// 取出队列头节点
        SearchNode current = queue.dequeue();
		// 如果到达终点，记录路径结果
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
		// 超过最大换乘次数，剪枝
        if (current.history.size() >= MAX_SEGMENTS) continue;
		// 扩展当前节点的下一步路径
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

	// 去除重复路径（相同路线ID序列）
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
	// 按总耗时排序，标记推荐路径
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