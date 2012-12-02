#ifndef _GCM_TEMPLATER_VARIABLE_H
# define _GCM_TEMPLATER_VARIABLE_H

#include <GC.h>
#include <Object.h>
#include <stl/String.h>
#include <stl/List.h>
#include <stdint.h>
#include <utility>

namespace GCM {
	namespace templater {

class Variable;
class Templater;

typedef enum {
	T_NULL,
	T_STRING,
	T_INT,
	T_INT64,
	T_DOUBLE,
	T_HASHTABLE
} VarType;

class GCM_API HashTable: public Map<String, Variable> {
	friend class Templater;
	public:
		HashTable(): Map<String, Variable>(), pos(-1) { }

		virtual GC<Variable> assign(String key, GC<Variable> value) {
			Map<String, Variable>::iterator i = this->find(key);
			if (i == this->end()) {
				this->keyOrder.push_back(key);
			}
			(*this)[key] = value;
			return value;
		}

		int ordered_begin() {
			return 0;
		}

		int ordered_end() {
			return -1;
		}

	private:
		int pos;
		std::vector<String> keyOrder;
};

class GCM_API Variable: public Object {
	friend class Templater;
	protected:
		VarType type;
		union {
			int32_t asInt;
			int64_t asInt64;
			double asDouble;
		} value;
		GC<Object> otherValue;
		void destructor();

	public:
		Variable();
		Variable(String value);
		Variable(uint32_t value);
		Variable(int32_t value);
		Variable(uint64_t value);
		Variable(int64_t value);
		Variable(double value);
		Variable(GC<HashTable> value);
		~Variable();

		bool operator==(Variable &compare);
		bool operator>(Variable &compare);

		operator String();
		operator uint32_t();
		operator int32_t();
		operator uint64_t();
		operator int64_t();
		operator double();
		operator GC<HashTable>();
		operator bool();

		bool isNull();
		VarType getType();

		GC<Variable> access(String index);

		WeakGC<Variable> assign(String value);
		WeakGC<Variable> assign(uint32_t value);
		WeakGC<Variable> assign(int32_t value);
		WeakGC<Variable> assign(uint64_t value);
		WeakGC<Variable> assign(int64_t value);
		WeakGC<Variable> assign(double value);
		WeakGC<Variable> assign(GC<HashTable> value);
		WeakGC<Variable> assign(GC<Variable> value);

		WeakGC<Variable> assign(String key, String value);
		WeakGC<Variable> assign(String key, uint32_t value);
		WeakGC<Variable> assign(String key, int32_t value);
		WeakGC<Variable> assign(String key, uint64_t value);
		WeakGC<Variable> assign(String key, int64_t value);
		WeakGC<Variable> assign(String key, double value);
		WeakGC<Variable> assign(String key, GC<HashTable> value);
		WeakGC<Variable> assign(String key, GC<Variable> value);

		String toString();
		int32_t toInt();
		int64_t toInt64();
		double toDouble();
		GC<HashTable> toHashTable();
};

	}
}

#endif
