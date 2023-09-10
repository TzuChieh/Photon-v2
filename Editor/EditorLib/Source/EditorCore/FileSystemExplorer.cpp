#include "EditorCore/FileSystemExplorer.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <DataIO/FileSystem/Filesystem.h>

#include <utility>
#include <filesystem>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(FileSystemExplorer, EditorCore);

FileSystemDirectoryEntry::FileSystemDirectoryEntry(
	FileSystemDirectoryEntry* const parent, 
	Path directoryPath, 
	CtorAccessToken)

	: m_parent(parent)
	, m_children()
	, m_directoryPath(std::move(directoryPath))
	, m_directoryName()
	, m_hasBeenPopulated(false)
{
	m_directoryName = m_directoryPath.getTrailingElement().toString();
}

FileSystemDirectoryEntry* FileSystemDirectoryEntry::getParent() const
{
	return m_parent;
}

bool FileSystemDirectoryEntry::haveChildren() const
{
	return !m_children.isEmpty();
}

FileSystemDirectoryEntry* FileSystemDirectoryEntry::getChild(const std::size_t childIndex) const
{
	return m_children.get(childIndex);
}

std::size_t FileSystemDirectoryEntry::numChildren() const
{
	return m_children.size();
}

const Path& FileSystemDirectoryEntry::getDirectoryPath() const
{
	return m_directoryPath;
}

const std::string& FileSystemDirectoryEntry::getDirectoryName() const
{
	return m_directoryName;
}

void FileSystemDirectoryEntry::populateChildren()
{
	// Using a separate flag instead of checking `haveChildren()` to avoid having to iterate the
	// directory repeatedly (consider a directory with no child directory while having many files).
	if(m_hasBeenPopulated)
	{
		return;
	}

	for(const auto& stdPath : std::filesystem::directory_iterator(m_directoryPath.toStdPath()))
	{
		// Skip any non-directory item
		const Path childPath(stdPath);
		if(!Filesystem::hasDirectory(childPath))
		{
			continue;
		}

		m_children.add(std::make_unique<FileSystemDirectoryEntry>(
			this, childPath, CtorAccessToken()));
	}

	m_hasBeenPopulated = true;
}

void FileSystemDirectoryEntry::removeChildren()
{
	m_children.removeAll();
	m_hasBeenPopulated = false;
}

FileSystemExplorer::FileSystemExplorer()
	: m_rootPaths()
	, m_rootDirectoryEntries()
	, m_currentRootIndex(0)
	, m_currentDirectoryEntry(nullptr)
{
	// Always has default root path and directory entry
	const Path defaultRootPath = Path("./");
	setCurrentRootPath(*addRootPath(defaultRootPath));
}
	
std::optional<std::size_t> FileSystemExplorer::addRootPath(const Path& path)
{
	// Not adding the same root path twice
	if(findRootPathIndex(path))
	{
		return std::nullopt;
	}

	if(!Filesystem::hasDirectory(path))
	{
		PH_LOG_WARNING(FileSystemExplorer,
			"cannot add non-directory root path {}", path);
		return std::nullopt;
	}

	m_rootPaths.push_back(path);
	m_rootDirectoryEntries.add(std::make_unique<FileSystemDirectoryEntry>(
		nullptr, path, FileSystemDirectoryEntry::CtorAccessToken()));
	return m_rootPaths.size() - 1;
}

void FileSystemExplorer::setCurrentRootPath(const std::size_t rootPathIndex)
{
	PH_ASSERT_LT(rootPathIndex, m_rootPaths.size());
	PH_ASSERT_EQ(m_rootPaths.size(), m_rootDirectoryEntries.size());

	m_currentRootIndex = rootPathIndex;
	m_currentDirectoryEntry = m_rootDirectoryEntries.get(rootPathIndex);
}

void FileSystemExplorer::expand(FileSystemDirectoryEntry* const directoryEntry)
{
	PH_ASSERT(directoryEntry);
	directoryEntry->populateChildren();
}

void FileSystemExplorer::collapse(FileSystemDirectoryEntry* const directoryEntry)
{
	PH_ASSERT(directoryEntry);
	directoryEntry->removeChildren();
}

std::vector<Path> FileSystemExplorer::makeItemListing(
	FileSystemDirectoryEntry* const directoryEntry,
	const bool withDirectories) const
{
	if(!directoryEntry)
	{
		return {};
	}

	std::vector<Path> itemSubpaths;
	itemSubpaths.reserve(128);
	for(const auto& stdPath : std::filesystem::directory_iterator(directoryEntry->getDirectoryPath().toStdPath()))
	{
		const Path itemPath(stdPath);

		// Potentially skip directory item
		if(!withDirectories && Filesystem::hasDirectory(itemPath))
		{
			continue;
		}

		itemSubpaths.push_back(itemPath.getTrailingElement());
	}
	return itemSubpaths;
}

TSpanView<Path> FileSystemExplorer::getRootPaths() const
{
	return m_rootPaths;
}

const Path& FileSystemExplorer::getCurrentRootPath() const
{
	PH_ASSERT_LT(m_currentRootIndex, m_rootPaths.size());
	return m_rootPaths[m_currentRootIndex];
}

FileSystemDirectoryEntry* FileSystemExplorer::getCurrentDirectoryEntry()
{
	PH_ASSERT(m_currentDirectoryEntry);
	return m_currentDirectoryEntry;
}

std::optional<std::size_t> FileSystemExplorer::findRootPathIndex(const Path& rootPath) const
{
	for(std::size_t i = 0; i < m_rootPaths.size(); ++i)
	{
		if(m_rootPaths[i] == rootPath)
		{
			return i;
		}
	}
	return std::nullopt;
}

}// end namespace ph::editor
