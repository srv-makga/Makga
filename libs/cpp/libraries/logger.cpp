module makga.lib.logger;

import <string>;
import <sstream>;
import <unordered_map>;
import makga.lib.util;
import makga.lib.pattern.singleton;

namespace makga::lib {
MakgaLogger::MakgaLogger()
{
	level_streams_.emplace(LogLevel::DEBUG, MakgaLoggerSteam{.stream = std::ostringstream()});
	level_streams_.emplace(LogLevel::INFO, MakgaLoggerSteam{.stream = std::ostringstream()});
	level_streams_.emplace(LogLevel::WARN, MakgaLoggerSteam{.stream = std::ostringstream()});
	level_streams_.emplace(LogLevel::ERROR, MakgaLoggerSteam{.stream = std::ostringstream()});
	level_streams_.emplace(LogLevel::FATAL, MakgaLoggerSteam{.stream = std::ostringstream()});
}
} // namespace makga::lib