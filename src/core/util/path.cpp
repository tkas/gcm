/**
 * GeoGet httpd server
 * @author Michal Kuchta <niximor@gmail.com>
 *
 * Path modification class
 */

#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
# include <unistd.h>
#endif
#include <limits.h>
#include <string>
#include <list>
#include <stdio.h>

#include <util/path.h>
#include <gcm.h>

using namespace GCM::util;

/**
 * Default constructor. Creates / path.
 */
GCM_API path::path(): root_parts(0), total_length(0), buffer(NULL), statBufValid(false) {
	this->add(DIRECTORY_SEPARATOR);
	this->root_parts = this->parts.size();
}

#if !defined(PATH_MAX) && defined(MAX_PATH)
# define PATH_MAX MAX_PATH
#endif

/**
 * Constructor.
 */
GCM_API path::path(const char *root): root_parts(0), total_length(0), buffer(NULL), statBufValid(false) {
	// If relative path, first add current working directory.
	if (!root || (root[0] != '/' && root[0] != '\\' && (root[0] == '\0' || (strncmp(root + 1, ":\\", 2) != 0 && strncmp(root + 1, ":/", 2) != 0)))) {
		char cwd[PATH_MAX];
		if (getcwd(cwd, PATH_MAX) != NULL) {
			this->add(cwd);
		}
	}

	this->add(root);
	this->root_parts = this->parts.size();
}

/**
 * Copy constructor.
 */
GCM_API path::path(const path &p): root_parts(p.root_parts), total_length(p.total_length), parts(p.parts), buffer(NULL), statBufValid(false) {
}

/**
 * Destructor.
 */
GCM_API path::~path() {
	if (buffer != NULL) {
		free(buffer);
	}
}

/**
 * Add part to path, normalize the path and don't allow to leave defined
 * root directory.
 * @return bool true on success, false when there was error in path (overflow
 *   or anything else).
 */
bool GCM_API path::add(const char *new_parts) {
	// Nothing to parse...
	if (new_parts == NULL || new_parts[0] == '\0') {
		return true;
	}

	this->statBufValid = false;

	char *part_begin = (char *)new_parts;

#ifdef _WIN32
	// If the parts begins with drive specification, use it only if no root
	// is specified.

	//printf("new_parts: %s\n", new_parts);
	//printf("existing_parts: %s\n", this->c_str());

	if (
		this->root_parts == 0 &&
		new_parts[0] != '\0' && (
			strncmp(new_parts + 1, ":/", 2) == 0 ||
			strncmp(new_parts + 1, ":\\", 2) == 0
		)
	) {
		// Create first root part from the drive letter.
		std::string part(new_parts, 2);
		this->root_parts = 1;
		this->parts.push_back(part);
		this->total_length += part.length() + 1;
		new_parts += 2;
		part_begin = (char *)new_parts;
		//printf("New parts shift... result: %s\n", part_begin);
	}
#endif

	size_t len = strlen(new_parts);

	//printf("Middle status (%d): %s\n", this->parts.size(), this->c_str());

	// Start from root, so cut out from the array everything that does not
	// belong to the root.
	if (new_parts[0] == '/' || new_parts[0] == '\\') {
		size_t root_count = this->root_parts;
		std::list<std::string>::iterator i = this->parts.begin();
		while (root_count > 0 && i != this->parts.end()) {
			i++;
			root_count--;
		}

		// Now, remove everything to the end.
		this->parts.erase(i, this->parts.end());
	}

	// While not end or NULL character.
	while (part_begin != NULL && *part_begin != '\0') {
		// Parse the path...
		std::string part;

		// We need to modify the pointer content, so we need to retype this to
		// non-const. But no modification is done to the string.
		char *split = strpbrk(part_begin, "/\\");
		size_t part_len;
		if (!split) {
			// Everything to the end...
			part_len = new_parts + len - part_begin;
		} else {
			part_len = split - part_begin;
		}

		part.assign(part_begin, part_len);

		if (part == "" || part == ".") {
			// Do not do anything with empty path (double slash...)
			// Do not do anything with one dot (current directory) too.
		} else if (part == "..") {
			// Go one directory up, if we can.
			if (this->parts.size() > this->root_parts) {
				// Cut out last node.
				this->parts.erase(--(this->parts.end()));

			} else {
				// Can't go up any more, we are at root.
				return false;
			}
		} else {
			//printf("Pushing back %s\n", part.c_str());
			this->parts.push_back(part);
			this->total_length += part.length() + 1;
		}

		if (!split) {
			break;
		}
		part_begin = split + 1;
	}

	//printf("After add(%d): %s\n", this->parts.size(), this->c_str());

	return true;
}

/**
 * Sets current directory as new root.
 */
void GCM_API path::jail() {
	this->root_parts = this->parts.size();
}

/**
 * Return string representation of path. The returned buffer is valid only
 * until next call to c_str().
 * @param divider Divider to use to delimiter path names. Can be generally
 *   / or \, acording to the platform.
 * @return null terminated string containing full path to the file.
 */
const char GCM_API *path::c_str(char divider) {
	if (this->buffer != NULL) {
		free(buffer);
	}

	this->buffer = (char *)malloc((this->length() + 1) * sizeof(char));
	if (!this->buffer) return NULL;

	// No parts, so it is root.
	std::list<std::string>::iterator i = this->parts.begin();
	if (i == this->parts.end()) {
		this->buffer[0] = divider;
		this->buffer[1] = '\0';
		return this->buffer;
	}

	size_t pos = 0;
	while (i != this->parts.end()) {
#ifndef _WIN32
		// Add first slash.
		this->buffer[pos] = divider;
		pos++;
#else
		// For Win32, we need to count with drive letter in the first part.
		if (i != this->parts.begin() || i->length() != 2 || i->data()[1] != ':') {
			this->buffer[pos] = divider;
			pos++;
		}
#endif

		//printf("serialize part %s\n", i->c_str());

		memcpy(this->buffer + pos, i->data(), i->length());
		pos += i->length();
		i++;
	}

	this->buffer[pos] = '\0';
	return this->buffer;
}

size_t GCM_API path::length() {
	if (this->total_length < 1) {
		return 1;
	} else {
		return this->total_length;
	}
}

/**
 * Return true, if given path exists.
 */
bool GCM_API path::exists() {
	if (!this->statBufValid) {
		if (stat(this->c_str(), &this->buf) < 0) {
			return false;
		} else {
			this->statBufValid = true;
			return true;
		}
	} else {
		return true;
	}
}

#ifndef S_ISDIR
# define S_ISDIR(m) (((m) & _S_IFDIR) > 0)
#endif

#ifndef S_ISREG
# define S_ISREG(m) (((m) & _S_IFREG) > 0)
#endif

/**
 * Return true, if given path is directory.
 */
bool GCM_API path::is_dir() {
	if (this->exists()) {
		return S_ISDIR(this->buf.st_mode);
	} else {
		return false;
	}
}

/**
 * Return true, if given path is regular file.
 */
bool GCM_API path::is_file() {
	if (this->exists()) {
		return S_ISREG(this->buf.st_mode);
	} else {
		return false;
	}
}

/**
 * Return file part of the path.
 * @param extension Optional extension. If file has this extension,
 *   it will be cut off the returned filename.
 */
const char GCM_API *path::basename(const char *extension) {
	if (this->parts.size() == 0) {
		return NULL;
	}

	if (this->buffer) {
		free(this->buffer);
	}

	std::string &last = this->parts.back();
	this->buffer = (char *)malloc((last.length() + 1) * sizeof(char));
	memcpy(this->buffer, last.c_str(), last.length() + 1);

	// Cut off extension.
	if (extension) {
		size_t len = last.length();
		size_t elen = strlen(extension);
		if (strcmp(extension, this->buffer + len - elen) == 0) {
			this->buffer[len - elen] = '\0';
		}
	}

	return this->buffer;
}

/**
 * Return directory part of the path
 */
const char GCM_API *path::dirname() {
	// Convert to string, and remove last slash.
	this->c_str();

	size_t len = this->length();
	for (size_t x = len - 1; x < len; --x) {
		if (this->buffer[x] == CDIRECTORY_SEPARATOR) {
			this->buffer[x] = '\0';
			break;
		}
	}

	return this->buffer;
}

