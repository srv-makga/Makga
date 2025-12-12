module makga.lib.logger;

import <string>;
import <sstream>;
import <unordered_map>;
import makga.lib.util;
import makga.lib.pattern.singleton;

namespace makga::lib {
MakgaLogger::MakgaLogger()
{
	level_streams_.emplace(LogLevel::DEBUG, MakgaLoggerSteam());
	level_streams_.emplace(LogLevel::INFO, MakgaLoggerSteam());
	level_streams_.emplace(LogLevel::WARN, MakgaLoggerSteam());
	level_streams_.emplace(LogLevel::ERROR, MakgaLoggerSteam());
	level_streams_.emplace(LogLevel::FATAL, MakgaLoggerSteam());
}
} // namespace makga::lib