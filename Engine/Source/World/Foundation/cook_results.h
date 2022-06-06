#pragma once

namespace ph
{

class PreCookResourceCollectionProxy;
class ResourceCookReport;

class PreCookResults final
{
public:
	PreCookResults(
		ResourceCookReport&             cookReport,
		PreCookResourceCollectionProxy& resourceCollection);

	ResourceCookReport& report();
	PreCookResourceCollectionProxy& resources();

private:
	ResourceCookReport&             m_cookReport;
	PreCookResourceCollectionProxy& m_resourceCollection;
};

class CookResults final
{
public:

private:
};

// In-header Implementations:

inline ResourceCookReport& PreCookResults::report()
{
	return m_cookReport;
}

inline PreCookResourceCollectionProxy& PreCookResults::resources()
{
	return m_resourceCollection;
}

}// end namespace ph
