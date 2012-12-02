#include <util/File.h>
#include <util/Log.h>
#include <stl/String.h>

#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#endif

#ifndef _WIN32
# include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <gcm.h>

#ifndef _WIN32
# include <dirent.h>
#endif

using namespace GCM::io;
using namespace GCM;

#ifndef _WIN32
const char File::DIRECTORY_SEPARATOR = '/';
#else
const char File::DIRECTORY_SEPARATOR = '\\';
#endif

GCM_API sFile::sFile(String path): mAbsolute(false) {
	this->append(path);
}

GCM_API sFile::sFile(PartsList::iterator begin, PartsList::iterator end, bool absolute): mAbsolute(absolute) {
	// This is in fact very memory efficient, because no
	// string is actually copied. Only references to that string.
	this->mParts.insert(
		this->mParts.begin(),
		begin,
		end
	);
}

GCM_API sFile::sFile(File file): mAbsolute(file->isAbsolute()) {
	this->mParts.insert(
		this->mParts.begin(),
		file->mParts.begin(),
		file->mParts.end()
	);
}

void GCM_API sFile::append(String path) {
	const char *s = path->c_str();
	size_t len = path->length();
	size_t lastPart = 0;

	//GCM::util::Log::d("GCM|File", "Appending %s to file %s.", path->c_str(), this->toString()->c_str());

	if (len == 0) return;

	for (size_t i = 0; i < len; i++) {
#ifdef _WIN32
		if (i == 0 && len > 1) {
			if (strncmp(":/", s + 1, 2) == 0 || strncmp(":\\", s + 1, 2) == 0) {
				this->mParts.clear();
				this->mAbsolute = true;
			}
		}
#endif

		if (s[i] == '\\' || s[i] == '/') {
			// This applies to windows too, because paths begining with /
			// applies to current drive root.
			if (i == 0) {
				//GCM::util::Log::d("GCM|File", "Absolute path.");

				// If this is absolute path...
				this->mParts.clear();
				this->mAbsolute = true;
				lastPart = i + 1;
				continue;
			}

			if (lastPart != i) {
				std::string part = path->substr(lastPart, i - lastPart);
				if (part.size() > 0) {
					if (part != ".") {
						if (part != ".." || mParts.size() == 0) {
							mParts.push_back(part);
						} else {
							mParts.pop_back();
						}
					}
				}
				lastPart = i + 1;
			}
		}
	}

	if (lastPart != len - 1) {
		std::string part = path->substr(lastPart, len - lastPart);

		if (part.size() == 0) return;
		if (part == ".") return;

		if (part != ".." || mParts.size() == 0) {
			mParts.push_back(part);
		} else {
			mParts.pop_back();
		}
	}

	//GCM::util::Log::d("GCM|File", "Result: %s", this->toString()->c_str());
}

File GCM_API sFile::getParentFile() {
	// Cut off last part.
	PartsList::iterator i = mParts.end();
	i--;

	return new sFile(mParts.begin(), i, this->mAbsolute);
}

String GCM_API sFile::toString() {
	String s;

	if (mParts.size() == 0) {
		if (this->mAbsolute) s = String(1, File::DIRECTORY_SEPARATOR);
		else s = ".";
	} else {
		PartsList::iterator i = mParts.begin();
		while (i != mParts.end()) {
#ifdef _WIN32
			if (i != mParts.begin()) {
#else
			if (i != mParts.begin() || this->isAbsolute()) {
#endif
				s->push_back(File::DIRECTORY_SEPARATOR);
			}
			s->append((*i)->c_str());
			i++;
		}
	}
	return s;
}

bool GCM_API sFile::exists() {
	String s = this->toString();
	const char *path = s->c_str();
	if (stat(path, &this->buf) < 0) {
		return false;
	} else{
		return true;
	}
}

#ifndef S_ISDIR
# define S_ISDIR(m) (((m) & _S_IFDIR) > 0)
#endif

#ifndef S_ISREG
# define S_ISREG(m) (((m) & _S_IFREG) > 0)
#endif

bool GCM_API sFile::isDirectory() {
	if (this->exists()) {
		return S_ISDIR(this->buf.st_mode);
	} else {
		return false;
	}
}

bool GCM_API sFile::isFile() {
	if (this->exists()) {
		return S_ISREG(this->buf.st_mode);
	} else {
		return false;
	}
}

bool GCM_API sFile::isHidden() {
	return !this->mParts.empty() && this->mParts.back()->size() > 0 && this->mParts.back()->at(0) == '.';
}

String GCM_API sFile::getName() {
	if (this->mParts.size() > 0) {
		return this->mParts.back();
	} else {
		return "";
	}
}

String GCM_API sFile::getPath() {
	return this->toString();
}

String GCM_API sFile::getParent() {
	return this->getParentFile()->toString();
}

bool GCM_API sFile::isAbsolute() {
	return this->mAbsolute;
}

#if !defined(PATH_MAX) && defined(MAX_PATH)
# define PATH_MAX MAX_PATH
#endif

File GCM_API sFile::getAbsoluteFile() {
	if (!this->isAbsolute()) {
		//GCM::util::Log::d("GCM|File", "Path %s is not absolute, work with CWD.", this->toString()->c_str());

		char cwd[PATH_MAX];
		getcwd(cwd, PATH_MAX);
		sFile *f = new sFile(cwd);

		f->mParts.insert(
			f->mParts.end(),
			this->mParts.begin(),
			this->mParts.end()
		);

		return f;
	} else {
		return this;
	}
}

String GCM_API sFile::getAbsolutePath() {
	return this->getAbsoluteFile()->toString();
}

String GCM_API sFile::getBasename(String extension) {
	return this->getBasename(extension->c_str());
}

String GCM_API sFile::getBasename(const char *extension) {
	if (this->mParts.size() > 0) {
		if (extension && extension[0] != '\0') {
			String file = this->mParts.back();
			if (file->endsWith(extension)) {
				return file->substr(0, file->size() - strlen(extension));
			} else {
				return file;
			}
		} else {
			return this->mParts.back();
		}
	} else {
		return ".";
	}
}

String GCM_API sFile::getDirname() {
	if (this->isDirectory()) return this->toString();
	else return this->getParent();
}

void GCM_API sFile::mkdirs() {
	File f = this->getAbsoluteFile();
	PartsList::iterator i = f->mParts.begin();
	PartsList::iterator last = f->mParts.end();

	sFile tempPath("/");

	while (i != last) {
		tempPath.append(*i);

		if (!tempPath.exists()) {
			tempPath.mkdir();
		}

		i++;
	}
}

void GCM_API sFile::mkdir() {
	String path = this->getPath();
	GCM::util::Log::d("File", "Create path %s", path->c_str());

	// TODO: Throw GCM::io::IOException when mkdir failed.

#ifndef _WIN32
	::mkdir(path->c_str(), 0777);
#else
	::CreateDirectory(path->c_str(), NULL);
#endif
}

class GCM_API AcceptAll: public FilenameFilter {
	public:
		virtual bool accept(File dir, String file) const {
			return file->at(0) != '.';
			(void)dir;
		}
};

class GCM_API AcceptAllFiles: public FileFilter {
	public:
		virtual bool accept(File dir, File file) const {
			return !file->isHidden();
			(void)dir;
		}
};

File GCM_API buildFile(File dir, String file) {
	File out(dir->toString());
	out->append(file);
	return out;
}

String GCM_API buildString(File dir, String file) {
	return file;
	(void)dir;
}

template <class TList, class TInstance, class TFilter>
TList _intListFiles(File fPtr, GC<TFilter> filter, TInstance (*builder)(File,String)) {
	if (fPtr->isDirectory()) {
		TList out;

#ifndef _WIN32
		DIR *dp;
		struct dirent *dirp;

		String path = fPtr->toString();

		dp = opendir(path->c_str());
		if (dp == NULL) {
			GCM::util::Log::e("GCM", "Unable to open directory %s (%s).", path->c_str(), strerror(errno));
			return out;
		}

		while ((dirp = readdir(dp)) != NULL) {
			String file = String(dirp->d_name);
			if (file->equals(".") || file->equals("..")) continue;

			TInstance iFile = builder(fPtr, file);
			//GCM::util::Log::d("GCM|File", "Found file %s. Constructed iFile %s.", file->c_str(), iFile->toString()->c_str());

			if (filter->accept(fPtr, iFile)) {
				//GCM::util::Log::d("GCM|File", "Filter passed.");
				out->push_back(iFile);
			}
		}

		closedir(dp);
#else
		WIN32_FIND_DATA dirp;
		String path = fPtr->toString();
		path->append("/*");

		HANDLE dp = FindFirstFile(path->c_str(), &dirp);
		if (dp == INVALID_HANDLE_VALUE) {
			GCM::util::Log::e("GCM", "Unable to open directory %s.", path->c_str());
			return out;
		}

		do {
			String file = String(dirp.cFileName);
			if (file->equals(".") || file->equals("..")) continue;
			TInstance iFile = builder(fPtr, file);
			if (filter->accept(fPtr, iFile)) {
				out->push_back(iFile);
			}
		} while (FindNextFile(dp, &dirp) != 0);

		FindClose(dp);
#endif

		return out;
	} else {
		return TList();
	}
}


StringList GCM_API sFile::list() {
	return this->list(new AcceptAll());
}

StringList GCM_API sFile::list(GC<FilenameFilter> filter) {
	return _intListFiles<StringList, String, FilenameFilter>(File(this), filter, buildString);
}


FileList GCM_API sFile::listFiles() {
	return this->listFiles(new AcceptAllFiles());
}

FileList GCM_API sFile::listFiles(GC<FileFilter> filter) {
	return _intListFiles<FileList, File, FileFilter>(File(this), filter, buildFile);
}

String GCM_API sFile::read() {
	String out;

	if (this->exists() && !this->isDirectory()) {
		FILE *f = fopen(this->getAbsolutePath()->c_str(), "r");
		char buffer[4096];

		while (f && !feof(f)) {
			size_t readed = fread(buffer, sizeof(char), 4096, f);
			out->append(buffer, readed);
		}

		if (f) {
			fclose(f);
		} else {
			GCM::util::Log::e("GCM|File", "Unable to open file %s: %s", this->getAbsolutePath()->c_str(), strerror(errno));
		}
	} else {
		GCM::util::Log::e("GCM|File", "File %s does not exists.", this->getAbsolutePath()->c_str());
	}
	return out;
}
