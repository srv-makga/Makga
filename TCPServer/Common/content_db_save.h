#pragma once

// DB�� �����ؾ� �ϴ� ������ �������̽�
class ContentDBSave
{
public:
	ContentDBSave() = default;
	virtual ~ContentDBSave() = default;

	virtual void DBSave() = 0;
};