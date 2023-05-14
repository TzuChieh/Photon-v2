#pragma once

#include <DataIO/FileSystem/Path.h>
#include <Utility/TUniquePtrVector.h>
#include <Utility/TSpan.h>

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <cstddef>

namespace ph::editor
{

/*! @brief Information for a filesystem directory.
*/
class FileSystemDirectoryEntry final
{
	friend class FileSystemExplorer;

private:
	// A dummy struct to prevent the entry from being constructed by others (ctors are public for
	// `unique_ptr` to access them)
	struct CtorAccessToken
	{};

public:
	FileSystemDirectoryEntry(FileSystemDirectoryEntry* parent, Path directoryPath, CtorAccessToken);

	bool haveChildren() const;
	FileSystemDirectoryEntry* getParent() const;
	FileSystemDirectoryEntry* getChild(std::size_t childIndex) const;
	std::size_t numChildren() const;
	const Path& getDirectoryPath() const;
	const std::string& getDirectoryName() const;

private:
	inline FileSystemDirectoryEntry(FileSystemDirectoryEntry&& other) = default;
	inline FileSystemDirectoryEntry& operator = (FileSystemDirectoryEntry&& rhs) = default;

	void populateChildren();
	void removeChildren();

	FileSystemDirectoryEntry* m_parent;
	TUniquePtrVector<FileSystemDirectoryEntry> m_children;
	Path m_directoryPath;
	std::string m_directoryName;
	bool m_hasBeenPopulated;
};

class FileSystemExplorer final
{
public:
	FileSystemExplorer();

	std::optional<std::size_t> addRootPath(const Path& path);
	void setCurrentRootPath(std::size_t rootPathIndex);
	TSpanView<Path> getRootPaths() const;
	const Path& getCurrentRootPath() const;

	FileSystemDirectoryEntry* getCurrentDirectoryEntry();

	/*! @brief Expand the entry by linking it with potential child entries.
	Expanding an already-expanded entry has no effect.
	*/
	void expand(FileSystemDirectoryEntry* directoryEntry);

	/*! @brief Collapse the entry by removing all potential child entries.
	Collapsing an already-collapsed entry has no effect.
	*/
	void collapse(FileSystemDirectoryEntry* directoryEntry);

	std::vector<Path> makeItemListing(
		FileSystemDirectoryEntry* directoryEntry,
		bool withDirectories = true) const;

private:
	std::optional<std::size_t> findRootPathIndex(const Path& rootPath) const;

	std::vector<Path> m_rootPaths;
	TUniquePtrVector<FileSystemDirectoryEntry> m_rootDirectoryEntries;
	std::size_t m_currentRootIndex;
	FileSystemDirectoryEntry* m_currentDirectoryEntry;
};

}// end namespace ph::editor
