#pragma once

#include "core_header.h"

/* �α� ����
* FATAL	���� �ɰ��� ������ �߻��� ���¸� ��Ÿ��. �ý��������� �ɰ��� ������ �߻��ؼ� ���ø����̼� �۵��� �Ұ����� ��찡 �ش��ϴµ�, �Ϲ������δ� ���ø����̼ǿ����� ����� ���� ����.
* ERROR	��û�� ó���ϴ��� ������ �߻��� ���¸� ��Ÿ��.
* WARN	ó�� ������ ����������, ���� �ý��� ������ ������ �� �� �ִ� ��� �޽����� ��Ÿ��.
* INFO	�α���, ���º���� ���� ������ �޽����� ��Ÿ��.
* DEBUG	���߽� ����� �뵵�� ����� �޽����� ��Ÿ��.
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