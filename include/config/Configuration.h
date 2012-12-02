#ifndef _CONFIGURATION_H
# define _CONFIGURATION_H 1

#include <map>
#include <string>
#include <Object.h>
#include <stl/String.h>

namespace GCM {
	namespace config {

class GCM_API Configuration: public Object {
	typedef enum {
		VAL_STRING,
		VAL_INT,
		VAL_DOUBLE
	} tValue;

	typedef union {
		long asInt;
		char *asString;
		double asDouble;
	} uValue;

	typedef struct {
		tValue type;
		uValue value;
	} sValue;

	std::map<std::string, sValue> data;
	std::string fileName;

	public:
		Configuration();

		virtual void load(const char *filename);
		virtual void write(const char *filename);
		virtual void write();

		virtual void set(const char *name, long value);
		virtual void set(const char *name, int value);
		virtual void set(const char *name, const char *value);
		virtual void set(const char *name, double value);
		virtual void set(const char *name, String value);
		
		virtual void unset(const char *name);

		virtual bool keyExists(const char *name);

		virtual long getInt(const char *name, long defaultValue);
		virtual const char *getString(const char *name, const char *defaultValue);
		virtual double getDouble(const char *name, double defaultValue);

		virtual const char *getFileName();
		virtual void setFileName(const char *fileName);

		virtual ~Configuration();
};

	}
}

#endif
