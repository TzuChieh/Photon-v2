#include "Common/LogSender.h"

namespace ph
{

const LogSender& LogSender::DEFAULT()
{
	static const LogSender defaultSender("PR");
	return defaultSender;
}

LogSender::LogSender(const std::string& senderName) :
	m_senderName(senderName)
{}

const std::string& LogSender::getSenderName() const
{
	return m_senderName;
}

}// end namespace ph