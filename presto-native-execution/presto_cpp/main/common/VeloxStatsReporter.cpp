//
// Created by Karteek Murthy Samba Murthy on 12/21/22.
//

#include "VeloxStatsReporter.h"

void VeloxStatsReporter::addStatExportType(
    folly::StringPiece key,
    facebook::velox::StatType statType) const {
  registeredStat.emplace(key.str(), statType);
}

void VeloxStatsReporter::addStatValue(folly::StringPiece key, size_t value)
    const {
  auto keyStr = key.str();
  auto it = registeredStat.find(keyStr);
  // VELOX_CHECK(it != registeredStat.end(), "addStatValue() for unregistered
  // stat");
  if (it == registeredStat.end())
    return;
  auto statsItr = statsVectorMap.find(keyStr);
  std::shared_ptr<Metric> metric = std::make_shared<Metric>();
  auto p1 = std::chrono::system_clock::now();
  metric->timestamp =
      std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch())
          .count();
  metric->value = value;
  if (statsItr == statsVectorMap.end()) {
    statsVectorMap.emplace(keyStr, std::vector<std::shared_ptr<Metric>>());
  }
  statsVectorMap[keyStr].push_back(std::move(metric));

  if (statsVectorMap[keyStr].size() == kMaxMetricSize) {
    // flush metrics to a file and clear the map.
    serializeMetrics(keyStr);
    statsVectorMap[keyStr].clear();
  }
}
