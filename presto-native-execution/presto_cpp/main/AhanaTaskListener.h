#include <folly/experimental/FunctionScheduler.h>
#include "fstream"
#include "presto_cpp/main/common/Configs.h"
#include "velox/exec/PlanNodeStats.h"
#include "velox/exec/Task.h"
#include "velox/exec/TaskStats.h"

namespace ahana {

class AhanaTaskListener : public facebook::velox::exec::TaskListener {
  void onTaskCompletion(
      const std::string& taskUuid,
      const std::string& taskId,
      facebook::velox::exec::TaskState taskState,
      std::exception_ptr error,
      facebook::velox::exec::TaskStats taskStats) override {
    scheduler_.addFunctionOnce([taskUuid = taskUuid,
                                taskId = taskId,
                                taskState = taskState,
                                error = error,
                                taskStats = taskStats] {
      auto jsonStats = facebook::velox::exec::toPlanStatsJson(taskStats);
      std::ofstream metricsFile;
      metricsFile.open("/tmp/" + taskId + ".json", std::ios::app);
      //        auto jsonStr = jsonStats.asString();
      metricsFile << jsonStats;
      metricsFile.close();
    });
    scheduler_.start();
  }

  folly::FunctionScheduler scheduler_;
};
} // namespace ahana
