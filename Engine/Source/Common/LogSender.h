#pragma once

#include <string>

namespace ph
{

class LogSender
{
public:
	static const LogSender& DEFAULT();

public:
	explicit LogSender(const std::string& senderName);

	const std::string& getSenderName() const;

private:
	std::string m_senderName;
};

}// end namespace ph
