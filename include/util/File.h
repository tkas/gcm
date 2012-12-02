#ifndef _GCM_UTIL_FILE_H
# define _GCM_UTIL_FILE_H

#include <list>
#include <vector>
#include <stl/String.h>
#include <GC.h>
#include <Object.h>
#include <sys/stat.h>

namespace GCM {
	namespace io {

class File;

class FilenameFilter: public Object {
	public:
		virtual bool accept(File dir, String name) const = 0;
};

class FileFilter: public Object {
	public:
		virtual bool accept(File dir, File file) const = 0;
};

class sFileList: public std::vector<File>, public Object {
};

class FileList: public GC<sFileList> {
	public:
		FileList(): GC<sFileList>(new sFileList()) {}
		typedef std::vector<File>::iterator iterator;
		typedef std::vector<File>::const_iterator const_iterator;
		typedef std::vector<File>::const_reverse_iterator const_reverse_iterator;
		typedef std::vector<File>::reverse_iterator reverse_iterator;
};

class GCM_API sFile: public Object {
	public:
		typedef std::list<String> PartsList;

	private:
		PartsList mParts;
		struct stat buf;
		bool mAbsolute;

	public:
		sFile(String path);
		sFile(PartsList::iterator begin, PartsList::iterator end, bool absolute);
		sFile(File file);
		virtual File getParentFile();
		virtual String getParent();

		virtual bool exists();
		virtual bool isDirectory();
		virtual bool isFile();
		virtual bool isAbsolute();
		virtual bool isHidden();

		virtual String toString();
		virtual void append(String s);

		virtual String getName();
		virtual String getPath();

		virtual File getAbsoluteFile();
		virtual String getAbsolutePath();

		virtual String getBasename(const char *extension = NULL);
		virtual String getBasename(String extension);
		virtual String getDirname();

		virtual StringList list();
		virtual StringList list(GC<FilenameFilter> filter);
		virtual FileList listFiles();
		virtual FileList listFiles(GC<FileFilter> filter);

		virtual String read();

		virtual void mkdirs();
		virtual void mkdir();
};

class GCM_API File: public GC<sFile> {
	friend class sFile;

	public:
		static const char DIRECTORY_SEPARATOR;

		File(String path): GC<sFile>(new sFile(path)) {}
		File(sFile *file): GC<sFile>(file) {}
		File(): GC<sFile>(NULL) {}

	private:
		File(sFile::PartsList::iterator begin, sFile::PartsList::iterator end, bool absolute):
			GC<sFile>(new sFile(begin, end, absolute)) {}
};

	}
}


#endif
