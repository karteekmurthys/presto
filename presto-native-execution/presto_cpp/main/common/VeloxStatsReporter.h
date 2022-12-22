/*
 * Copyright (c) Ahana Cloud Inc. and its affiliates.
 *
 */

#include <fstream>
#include <iostream>
#include "presto_cpp/main/common/Counters.h"
#include "velox/common/base/StatsReporter.h"
#include "velox/velox/common/base/Exceptions.h"

const uint64_t kMaxMetricSize = 100;

class VeloxStatsReporter : public facebook::velox::BaseStatsReporter {
 public:
  struct Metric {
    uint64_t timestamp; // 8
    size_t value; // 4
  };

  void addStatExportType(const char* key, facebook::velox::StatType statType)
      const override {}

  void addStatExportType(
      folly::StringPiece /*key*/,
      facebook::velox::StatType /*statType*/) const override;

  void addStatValue(const std::string& key, size_t value) const override{};
  void addStatValue(const char* /* key */, size_t /* value */) const override {}

  void addStatValue(folly::StringPiece /* key */, size_t /* value */)
      const override;

 private:
  void serializeMetrics(const std::string& key) const {
    std::ofstream file;
    file.open("/tmp/metrics.lop", std::ios::app);
    for (auto metric : statsVectorMap[key]) {
      file << metric->value << "," << metric->timestamp << "\n";
    }
    file.close();
  }

  mutable std::unordered_map<std::string, facebook::velox::StatType>
      registeredStat;
  mutable std::unordered_map<std::string, std::vector<std::shared_ptr<Metric>>>
      statsVectorMap;
}; // VeloxStatsReporter
