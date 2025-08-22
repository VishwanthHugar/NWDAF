#include <ctime>
#include "../../header/utility/JsonPraser.h"
#include "../../header/Logger.h"

std::optional<KPIData> JsonParser::parseKPIRequest(const std::string& jsonStr, std::string& requestId) {
    try {
        auto body = json::parse(jsonStr);

        KPIData data;
        data.nfId = body.value("nfId", "");
        data.kpiName = body.value("kpiName", "");
        data.value = body.value("value", 0.0);
        data.sliceId = body.value("sliceId", "");
        data.timestamp = static_cast<uint64_t>(std::time(nullptr));
        requestId = body.value("requestId", "");  // Read requestId

        std::string callerNF = body.value("callerNF", "");

        if (!validateKPIData(data, callerNF) || requestId.empty()) {
            LOG_ERROR_FILE("Validation failed for KPI request or missing requestId: " + jsonStr);
            return std::nullopt;
        }

        return data;
    }
    catch (const std::exception& e) {
        LOG_ERROR_FILE(std::string("JSON parse failed: ") + e.what());
        return std::nullopt;
    }
}

bool JsonParser::validateKPIData(const KPIData& data, const std::string& callerNF) {
    if (data.nfId.empty() || data.sliceId.empty() || callerNF.empty() || data.kpiName.empty()) {
        return false;
    }
    if (data.kpiName == "LoadLevel" && (data.value < 0.0 || data.value > 100.0)) {
        return false;
    }
    return true;
}

std::string JsonParser::buildKPIResponse(const KPIData& data, const std::string& callerNF, const std::string& requestId) {
    json reply = {
        {"requestId", requestId},        // Include requestId
        {"status", "accepted"},
        {"nfId", data.nfId},
        {"sliceId", data.sliceId},
        {"kpiName", data.kpiName},
        {"callerNF", callerNF},
        {"currentValue", data.value},
        {"timestamp", data.timestamp}
    };
    return reply.dump(4);
}
