#pragma once
#include <string>
#include <functional>
#include <fmt/format.h>
#include <better-enums/enum.h>
#include "Common/LumiereMacro.h"
#include "Common/LumiereSingleton.h"
#include "Common/LumierePlatform.h"
#include "Time/LumiereDateTime.h"

#define LUMIERE_LOG(message, logLevel) NAMESPACE_NAME::LumiereLog(message, __CODE__SITE__, logLevel)

#define LUMIERE_TRACE(message) LUMIERE_LOG(message, NAMESPACE_NAME::ELogLevel::TRACE_LEVEL)
#define LUMIERE_DEBUG(message) LUMIERE_LOG(message, NAMESPACE_NAME::ELogLevel::DEBUG_LEVEL)
#define LUMIERE_INFO(message)  LUMIERE_LOG(message, NAMESPACE_NAME::ELogLevel::INFO_LEVEL)
#define LUMIERE_WARN(message)  LUMIERE_LOG(message, NAMESPACE_NAME::ELogLevel::WARN_LEVEL)
#define LUMIERE_ERROR(message) LUMIERE_LOG(message, NAMESPACE_NAME::ELogLevel::ERROR_LEVEL)
#define LUMIERE_FAULT(message) LUMIERE_LOG(message, NAMESPACE_NAME::ELogLevel::FAULT_LEVEL)

#define LUMIERE_TRACE_FMT(fmtStr, ...) LUMIERE_TRACE(fmt::format(fmtStr, __VA_ARGS__))
#define LUMIERE_DEBUG_FMT(fmtStr, ...) LUMIERE_DEBUG(fmt::format(fmtStr, __VA_ARGS__))
#define LUMIERE_INFO_FMT(fmtStr, ...)  LUMIERE_INFO(fmt::format(fmtStr, __VA_ARGS__))
#define LUMIERE_WARN_FMT(fmtStr, ...)  LUMIERE_WARN(fmt::format(fmtStr, __VA_ARGS__))
#define LUMIERE_ERROR_FMT(fmtStr, ...) LUMIERE_ERROR(fmt::format(fmtStr, __VA_ARGS__))
#define LUMIERE_FAULT_FMT(fmtStr, ...) LUMIERE_FAULT(fmt::format(fmtStr, __VA_ARGS__))


BEGIN_LUMIERE_NAMESPACE

BETTER_ENUM(ELogLevel, std::uint8_t, TRACE_LEVEL, DEBUG_LEVEL, INFO_LEVEL, WARN_LEVEL, ERROR_LEVEL, FAULT_LEVEL, NUM_LOG_LEVEL);

extern void LumiereLog(const std::string& message, const std::string& fileName, const std::string& functionName, int lineNumber, ELogLevel logLevel);

class LogSource {
public:
    LogSource(const std::string& fileName, const std::string& functionName, int lineNumber);

public:
    const std::string fileName;
    const std::string functionName;
    const int lineNumber;
};


class LogInfo {
public:
    LogInfo(const std::string& message, const LogSource& source, const DateTime& time, ELogLevel logLevel);

public:
    const std::string message;
    const LogSource source;
    const DateTime time;
    const ELogLevel logLevel;
};


std::string DefaultMessageFormater(const LogInfo& log);


class OutputStream {
public:
    virtual void add(const std::string& message);
};


class LogManager : public Singleton<LogManager> {
public:
    using MessageFormater = std::function<std::string(const LogInfo&)>;

public:
    explicit LogManager(const MessageFormater& formater = DefaultMessageFormater, const OutputStream& outputStream = OutputStream());
    ~LogManager() = default;
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    void add(const LogInfo& log);

private:
    MessageFormater mFormater;
    OutputStream mOutputStream;
};

END_LUMIERE_NAMESPACE
