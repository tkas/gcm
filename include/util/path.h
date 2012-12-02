/**
 * GeoGet httpd server
 * @author Michal Kuchta <niximor@gmail.com>
 *
 * Path manipulation
 */

#ifndef _PATH_H
#define _PATH_H

#include <gcm.h>
#include <list>
#include <string>

#include <sys/stat.h>

#ifndef _WIN32
#	define DIRECTORY_SEPARATOR "/"
#	define CDIRECTORY_SEPARATOR '/'
#else
#	define DIRECTORY_SEPARATOR "\\"
#	define CDIRECTORY_SEPARATOR '\\'
#endif

namespace GCM {
	namespace util {

/**
 * Path manipulation.
 */
class GCM_API path {
	private:
		/**
		 * Number of parts that belongs to the root.
		 */
		size_t root_parts;

		/**
		 * Total length of the path.
		 */
		size_t total_length;

		/**
		 * Parts of the path.
		 */
		std::list<std::string> parts;

		/**
		 * Buffer where is stored path returned by c_str().
		 */
		char *buffer;

		/**
		 * Stat buffer.
		 */
		struct stat buf;
		bool statBufValid;

	public:
		/**
		 * Create empty path.
		 */
		path();

		/**
		 * Create path with given root as string.
		 */
		path(const char *root);

		/**
		 * Copy constructor.
		 */
		path(const path &p);

		/**
		 * Destructor.
		 */
		virtual ~path();

		/**
		 * Add portion of path.
		 */
		bool add(const char *parts);

		/**
		 * Set current path as root.
		 */
		void jail();

		/**
		 * Return path length.
		 */
		size_t length();

		/**
		 * Convert the path representation to string.
		 */
		const char *c_str(char divider = CDIRECTORY_SEPARATOR);

		/**
		 * Return true, if current path exists.
		 */
		bool exists();

		/**
		 * Return true, if current path is directory.
		 */
		bool is_dir();

		/**
		 * Return true, if current path is file.
		 */
		bool is_file();

		/**
		 * Return base (file) name of current path.
		 */
		const char *basename(const char *extension = NULL);

		/**
		 * Return directory name of current path.
		 */
		const char *dirname();
};

	}
}

#endif
