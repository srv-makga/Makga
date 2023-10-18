#pragma once

#include "core_header.h"

/* 로그 레벨
* FATAL	아주 심각한 에러가 발생한 상태를 나타냄. 시스템적으로 심각한 문제가 발생해서 어플리케이션 작동이 불가능할 경우가 해당하는데, 일반적으로는 어플리케이션에서는 사용할 일이 없음.
* ERROR	요청을 처리하는중 문제가 발생한 상태를 나타냄.
* WARN	처리 가능한 문제이지만, 향후 시스템 에러의 원인이 될 수 있는 경고성 메시지를 나타냄.
* INFO	로그인, 상태변경과 같은 정보성 메시지를 나타냄.
* DEBUG	개발시 디버그 용도로 사용한 메시지를 나타냄.
*/

namespace core {
namespace logger {
class Logger
{
public:
	Logger() = default;
	virtual ~Logger() = default;
};
} // namespace logger
} // namespace core