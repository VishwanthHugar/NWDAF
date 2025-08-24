#ifndef _INCLUDE_JSONPARSER_H
#define _INCLUDE_JSONPARSER_H

#include "../3rdparty/json.hpp"
#include "../KPIData.h"
#include <string>
#include <optional>

using json = nlohmann::json;

class JsonParser {
public:
    // Parse KPIData from JSON string and validate
    static std::optional<KPIData> parseKPIRequest(const std::string& jsonStr, std::string& requestId);

    // Build JSON response from KPIData
    static std::string buildKPIResponse(const KPIData& data, const std::string& callerNF, const std::string& requestId);

private:
    static bool validateKPIData(const KPIData& data, const std::string& callerNF);
};

#endif // _INCLUDE_JSONPARSER_H
