#pragma once

// DB에 저장해야 하는 컨텐츠 인터페이스
class ContentDBSave
{
public:
	ContentDBSave() = default;
	virtual ~ContentDBSave() = default;

	virtual void DBSave() = 0;
};