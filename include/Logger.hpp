#pragma once

#include <plog/Appenders/IAppender.h>
#include <plog/Log.h>
#include <plog/Severity.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <string>
#include <memory>

class PlogWrapper {
    static inline bool _initialized = false;
    static inline plog::Severity _level = plog::info;

public:
    enum class OutputType {
        Console,
        File,
        ConsoleAndFile
    };

    enum class Level {
        None,
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

    enum class FormatType {
        Full,      // 包含时间戳、级别、行号等的完整格式 (使用 TxtFormatter)
        MessageOnly// 只包含日志消息本身 (使用 MessageOnlyFormatter)
    };

    static void init(Level level = Level::Info, OutputType output = OutputType::Console,
                     FormatType format = FormatType::Full,
                     const std::string& filePath = "app.log",
                     unsigned maxFileSize = 10 * 1024 * 1024,// 10 MB
                     unsigned maxFiles = 3) {
        if (_initialized) {
            return;
        }

        switch (level) {
            case Level::None:
                _level = plog::none;
                break;
            case Level::Trace:
                _level = plog::verbose;
                break;
            case Level::Debug:
                _level = plog::debug;
                break;
            case Level::Info:
                _level = plog::info;
                break;
            case Level::Warn:
                _level = plog::warning;
                break;
            case Level::Error:
                _level = plog::error;
                break;
            case Level::Fatal:
                _level = plog::fatal;
                break;
            default:
                _level = plog::info;
                break;
        }

        std::unique_ptr<plog::IAppender> consoleAppender = nullptr;
        std::unique_ptr<plog::IAppender> fileAppender = nullptr;

        const bool needsConsole = (output == OutputType::Console || output == OutputType::ConsoleAndFile);
        const bool needsFile = (output == OutputType::File || output == OutputType::ConsoleAndFile);

        if (needsConsole) {
            if (format == FormatType::Full) {
                consoleAppender = std::make_unique<plog::ColorConsoleAppender<plog::TxtFormatter>>();
            } else {// FormatType::MessageOnly
                consoleAppender = std::make_unique<plog::ColorConsoleAppender<plog::MessageOnlyFormatter>>();
            }
        }

        if (needsFile) {
            if (format == FormatType::Full) {
                fileAppender = std::make_unique<plog::RollingFileAppender<plog::TxtFormatter>>(filePath.c_str(), maxFileSize, maxFiles);
            } else {// FormatType::MessageOnly
                fileAppender = std::make_unique<plog::RollingFileAppender<plog::MessageOnlyFormatter>>(filePath.c_str(), maxFileSize, maxFiles);
            }
        }

        plog::IAppender* appender = nullptr;

        if (consoleAppender) {
            appender = consoleAppender.get();
        } else if (fileAppender) {
            appender = fileAppender.get();
        }

        if (!appender) {
            consoleAppender = std::make_unique<plog::ColorConsoleAppender<plog::TxtFormatter>>();
            appender = consoleAppender.get();
        }

        plog::init(_level, appender);

        _initialized = true;
    }
};