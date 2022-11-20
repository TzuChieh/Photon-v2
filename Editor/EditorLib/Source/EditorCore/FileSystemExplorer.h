#pragma once

#include <DataIO/FileSystem/Path.h>
#include <Utility/TUniquePtrVector.h>

#include <vector>
#include <span>
#include <memory>
#include <string>

namespace ph::editor
{

class FileSystemDirectoryEntry final
{
public:
	explicit FileSystemDirectoryEntry(Path directoryPath);

	bool isExpanded() const;
	std::span<FileSystemDirectoryEntry> getChildren();
	const Path& getDirectoryName() const;

private:
	TUniquePtrVector<FileSystemDirectoryEntry> m_children;
	Path m_directoryPath;
	std::string m_directoryName;
};

class FileSystemExplorer final
{
public:
	std::size_t addRootPath(Path path);
	void expandRootPath(std::size_t rootIndex);
	std::span<const Path> getRootPaths() const;
	std::span<FileSystemDirectoryEntry> getRootEntries();

private:
	std::vector<Path> m_rootPaths;
	std::vector<FileSystemDirectoryEntry> m_rootDirectoryEntries;
	/*std::vector<Path> 
	std::size_t m_currentRootIndex;*/
};

}// end namespace ph::editor
