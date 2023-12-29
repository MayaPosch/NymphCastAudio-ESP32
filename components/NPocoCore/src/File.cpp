//
/*
	File.cpp
*/


#include "File.h"
#include "Path.h"
#include "DirectoryIterator.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "platforms/File_WINCE.cpp"
#else
#include "platforms/File_WIN32U.cpp"
#endif
#elif defined(POCO_VXWORKS)
#include "platforms/File_VX.cpp"
#elif defined(POCO_OS_FAMILY_UNIX)
#include "platforms/File_UNIX.cpp"
#elif defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/File_FreeRTOS.cpp"
#endif
#include "Thread.h"


namespace Poco {


File::File()
{
}


File::File(const std::string& path): FileImpl(path)
{
}


File::File(const char* path): FileImpl(std::string(path))
{
}


File::File(const Path& path): FileImpl(path.toString())
{
}


File::File(const File& file): FileImpl(file.getPathImpl())
{
}


File::~File()
{
}


File& File::operator = (const File& file)
{
	setPathImpl(file.getPathImpl());
	return *this;
}


File& File::operator = (const std::string& path)
{
	setPathImpl(path);
	return *this;
}


File& File::operator = (const char* path)
{
	poco_check_ptr (path);
	setPathImpl(path);
	return *this;
}


File& File::operator = (const Path& path)
{
	setPathImpl(path.toString());
	return *this;
}


void File::swap(File& file)
{
	swapImpl(file);
}


bool File::exists() const
{
	return existsImpl();
}


bool File::canRead() const
{
	return canReadImpl();
}


bool File::canWrite() const
{
	return canWriteImpl();
}


bool File::canExecute() const
{
	return canExecuteImpl();
}


bool File::isFile() const
{
	return isFileImpl();
}


bool File::isDirectory() const
{
	return isDirectoryImpl();
}


bool File::isLink() const
{
	return isLinkImpl();
}


bool File::isDevice() const
{
	return isDeviceImpl();
}


bool File::isHidden() const
{
	return isHiddenImpl();
}


Timestamp File::created() const
{
	return createdImpl();
}


Timestamp File::getLastModified() const
{
	return getLastModifiedImpl();
}


File& File::setLastModified(const Timestamp& ts)
{
	setLastModifiedImpl(ts);
	return *this;
}


File::FileSize File::getSize() const
{
	return getSizeImpl();
}


File& File::setSize(FileSizeImpl size)
{
	setSizeImpl(size);
	return *this;
}


File& File::setWriteable(bool flag)
{
	setWriteableImpl(flag);
	return *this;
}


File& File::setReadOnly(bool flag)
{
	setWriteableImpl(!flag);
	return *this;
}


File& File::setExecutable(bool flag)
{
	setExecutableImpl(flag);
	return *this;
}


bool File::copyTo(const std::string& path, int options) const {
	Path src(getPathImpl());
	Path dest(path);
	File destFile(path);
	if ((destFile.exists() && destFile.isDirectory()) || dest.isDirectory()) {
		dest.makeDirectory();
		dest.setFileName(src.getFileName());
	}
	
	if (isDirectory()) {
		return copyDirectory(dest.toString(), options);
	}
	else {
		return copyToImpl(dest.toString(), options);
	}
	
	return true;
}


bool File::copyDirectory(const std::string& path, int options) const {
	File target(path);
	target.createDirectories();

	Path src(getPathImpl());
	src.makeFile();
	DirectoryIterator it(src);
	DirectoryIterator end;
	for (; it != end; ++it) 	{
		if (!it->copyTo(path, options)) {
			return false;
		}
	}
	
	return true;
}


bool File::moveTo(const std::string& path, int options) {
	if (!copyTo(path, options)) { return false; }
	remove(true);
	setPathImpl(path);
	return true;
}


bool File::renameTo(const std::string& path, int options) {
	if (!renameToImpl(path, options)) { return false; }
	setPathImpl(path);
	return true;
}


bool File::linkTo(const std::string& path, LinkType type) const {
	return linkToImpl(path, type);
}


bool File::remove(bool recursive) {
	if (recursive && !isLink() && isDirectory()) {
		std::vector<File> files;
		list(files);
		for (auto& f: files) {
			f.remove(true);
		}

		// Note: On Windows, removing a directory may not succeed at first
		// try because deleting files is not a synchronous operation. Files
		// are merely marked as deleted, and actually removed at a later time.
		//
		// An alternate strategy would be moving files to a different directory
		// first (on the same drive, but outside the deleted tree), and marking
		// them as hidden, before deleting them, but this could lead to other issues.
		// So we simply retry after some time until we succeed, or give up.

		int retry = 8;
		long sleep = 10;
		while (retry > 0) {
			/* try {
				removeImpl();
				retry = 0;
			}
			catch (DirectoryNotEmptyException&) { */
			if (!removeImpl()) {
				if (--retry == 0) {
					//throw;
					return false;
				}
				
				Poco::Thread::sleep(sleep);
				sleep *= 2;
			}
			else {
				retry = 0;
			}
		}
	}
	else {
		return removeImpl();
	}
	
	return true;
}


bool File::createFile()
{
	return createFileImpl();
}


bool File::createDirectory()
{
	return createDirectoryImpl();
}


void File::createDirectories()
{
	if (!exists())
	{
		Path p(getPathImpl());
		p.makeDirectory();
		if (p.depth() > 1)
		{
			p.makeParent();
			File f(p);
			f.createDirectories();
		}
		//try
		//{
			createDirectoryImpl();
		//}
		//catch (FileExistsException&)
		//{
		//}
	}
}


void File::list(std::vector<std::string>& files) const
{
	files.clear();
	DirectoryIterator it(*this);
	DirectoryIterator end;
	while (it != end)
	{
		files.push_back(it.name());
		++it;
	}
}


File::FileSize File::totalSpace() const
{
	return totalSpaceImpl();
}


File::FileSize File::usableSpace() const
{
	return usableSpaceImpl();
}


File::FileSize File::freeSpace() const
{
	return freeSpaceImpl();
}


void File::list(std::vector<File>& files) const
{
	files.clear();
	DirectoryIterator it(*this);
	DirectoryIterator end;
	while (it != end)
	{
		files.push_back(*it);
		++it;
	}
}


void File::handleLastError(const std::string& /*path*/)
{
	//handleLastErrorImpl(path);
}


} // namespace Poco
