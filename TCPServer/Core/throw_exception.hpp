#pragma once

# include <system_error>

namespace core
{
using error_code = std::error_code;
using system_error = std::system_error;

template<typename Exception>
inline void throw_exception(const Exception& _e)
{
	throw _e;
}

inline void do_throw_error(const error_code _err)
{
	system_error e(_err);
	throw_exception(e);
}

inline  void do_throw_error(const error_code _err, const char* _location)
{
	system_error e(_err, _location);
	throw_exception(e);
}

inline void throw_error(const error_code _err)
{
	if (_err)
		do_throw_error(_err);
}

inline void throw_error(const error_code _err, const char* _location)
{
	if (_err)
		do_throw_error(_err, _location);
}
} // namespace core