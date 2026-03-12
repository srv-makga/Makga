module;

export module makga.lib.conversion_string;

import <string>;

namespace makga::lib {

// @brief 멀티바이트와 유니코드 문자열 간 변환을 처리하는 클래스
// @details 내부적으로 멀티바이트·유니코드 문자열을 모두 저장하여, 매번 변환 없이 바로 사용 가능
export class ConversionString
{
public:
	ConversionString(const std::string& str);
	ConversionString(std::string&& str);

	ConversionString(const std::wstring& str);
	ConversionString(std::wstring&& str);
	~ConversionString() = default;

	void operator=(const std::string& str);
	void operator=(std::string&& str);

	void operator=(const std::wstring& str);
	void operator=(std::wstring&& str);

	// @brief 비-const: string_ 반환, const: wstring_ 반환
	const std::string&  operator()();
	const std::wstring& operator()() const;

	void operator()(std::string& str);
	void operator()(std::wstring& str) const;

	operator std::string()  const;
	operator std::wstring() const;

	const std::string&  GetString()  const;
	const std::wstring& GetWString() const;

private:
	std::string  string_;
	std::wstring wstring_;
};

} // namespace makga::lib