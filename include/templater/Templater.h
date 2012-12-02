#ifndef _GCM_TEMPLATER_H
# define _GCM_TEMPLATER_H

#include "Object.h"
#include "stl/String.h"
#include "stl/List.h"
#include "GC.h"
#include <map>

#include "Variable.h"

namespace GCM {
	namespace templater {

class GCM_API Templater: public Object {
	friend class TemplaterInitializer;

	public:
		typedef GC<Variable> (*fFormatFunc)(GC<Variable> value);
		typedef std::map<String, fFormatFunc> FormatFunctions;

		typedef enum {
			/**
			 * Echo variable to output. Uses only first operand, which contains
			 * variable containing text to be printed.
			 */
			OP_ECHO,

			/**
			 * Assign value to variable.
			 * First operand contains variable containing new variable name
			 * Second operand contains variable to be assigned to newly
			 * created variable.
			 */
			OP_ASSIGN,

			/**
			 * Begining of the IF statement.
			 * Uses first operand, which contains variable containing either
			 * 0 for false or anything else for true.
			 * Jump contains address after the else or after the endif.
			 */
			OP_IF_BEGIN,

			/**
			 * Else part of the if.
			 * Jump contains address after the else section.
			 */
			OP_IF_ELSE,

			/**
			 * Begining of foreach loop.
			 * First operand contains variable which contains array | hashtable.
			 * Second operand contains name of variable to assign values to.
			 * Jump contains address after the foreach end.
			 */
			OP_FOREACH_BEGIN,

			/**
			 * End of foreach loop.
			 * Jump contains address of the foreach begin.
			 */
			OP_FOREACH_END,

			/**
			 * Operation means calling conversion function.
			 * First operand contains variable to be converted.
			 * Second operand contains pointer to the conversion function
			 * Third parameter contains name of target variable.
			 * that should be assigned.
			 */
			OP_CALL_CONVFUNC
		} OperationCode;

		struct Operation {
			Operation(OperationCode opcode, String varg1, String varg2, String varg3): opcode(opcode), varg1(varg1), varg2(varg2), varg3(varg3), jump(-1) {}
			Operation(OperationCode opcode, String varg1, String varg2): opcode(opcode), varg1(varg1), varg2(varg2), jump(-1) {}
			Operation(OperationCode opcode, String varg1): opcode(opcode), varg1(varg1), jump(-1) {}
			Operation(OperationCode opcode): opcode(opcode), jump(-1) {}

			OperationCode opcode;

			String varg1;
			String varg2;
			String varg3;
			int jump;
		};

		typedef std::vector<Operation> Operations;

		enum State {
			S_ERROR,
			S_TEXT,
			S_BLOCK,
			S_BLOCK_INSIDE,
			S_VARIABLE_BEGIN,
			S_VARIABLE,
			S_IDENTIFIER,
			S_MODIFIER_BEGIN,
			S_MODIFIER
		};
		State state;

		class GCM_API Token {
			public:
				enum TokenType {
					T_ERROR,
					T_TEXT,
					T_ECHO,
					T_BLOCK_START,
					T_VARIABLE,
					T_BLOCK_END,
					T_IF,
					T_ELSE,
					T_ENDIF,
					T_FOREACH,
					T_AS,
					T_ENDFOREACH,
					T_EOF,
					T_FUNCALL,
				};

				int line;
				TokenType type;
				String content;

				static const char *typeToString(TokenType type);
		};

	protected:
		GC<Variable> variables;
		Operations operations;

		String tpl;
		const char *data;
		int line;
		int column;
		size_t pos;
		int tempvar;
		Token tok;

		void unget();
		bool lookAhead(const char *what);
		Token getNextToken();

		/**
		 * Compile the template to opcode
		 */
		bool compile(String tpl);
		bool body();
		bool blockStart();
		bool ifBegin();
		bool echoVar();
		bool foreachBegin();

		String mkTempVar();
		String handleVariable();
		String handleExpression();
		GC<Variable> resolveVariable(String name);

		/**
		 * Execute the opcode
		 */
		String execute();

		static FormatFunctions &getFF();

	public:
		Templater();
		void assign(String name, GC<Variable> value);
		static void registerFormatFunc(String funcName, fFormatFunc callback);
		String eval(String tpl);
		String debugVar(GC<Variable> var, bool print = true, int level = 0);
};

	}
}

#endif
