#include <templater/Templater.h>
#include <templater/Variable.h>
#include <string.h>

using namespace GCM::templater;
using namespace GCM;

Templater::Templater(): Object(), variables(new Variable(new HashTable())) {
}

bool GCM_API Templater::lookAhead(const char *what) {
	return strncmp(data + pos, what, strlen(what)) == 0;
}

void GCM_API Templater::unget() {
	if (pos > 0) {
		if (data[pos] == '\n') {
			line--;
			for (size_t tempos = pos - 1; tempos > 0; --tempos) {
				if (data[tempos] == '\n') {
					column = pos - tempos;
					break;
				}

				if (tempos == 1) column = pos - 1;
			}
		}

		pos--;
	}
}

Templater::Token GCM_API Templater::getNextToken() {
	bool hasToken = false;
	Token out;

	out.line = line;

	size_t start = pos;
	for (; pos < tpl->length(); pos++) {
		const char ch = data[pos];
		if (ch == '\n') {
			line++;
			column = 1;
		} else {
			column++;
		}

		if (hasToken) {
			break;
		}

		//GCM::util::Log::d("Templater", "State %d, input %c", state, ch);

		switch (state) {
			case S_ERROR:
				out.type = Token::T_ERROR;
				hasToken = true;
				break;

			case S_TEXT:
				if (ch == '{') {
					state = S_BLOCK;
					if (start != pos) {
						out.type = Token::T_TEXT;
						out.content = String(this->tpl, start, pos - start);
						hasToken = true;
					}
				}
				break;

			case S_BLOCK:
				out.type = Token::T_BLOCK_START;
				state = S_BLOCK_INSIDE;
				hasToken = true;
				this->unget();
				break;

			case S_BLOCK_INSIDE:
				if (ch == '$') {
					state = S_VARIABLE_BEGIN;
					start = pos + 1;
				} else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
					state = S_IDENTIFIER;
					start = pos;
				} else if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
					// Skip whitespace
				} else if (ch == '|') {
					state = S_MODIFIER_BEGIN;
				} else if (ch == '}') {
					out.type = Token::T_BLOCK_END;
					hasToken = true;
					state = S_TEXT;
				} else {
					GCM::util::Log::e("Templater", "Parse error in template %s on line %d, column %d: Expected variable or identifier, got %c.", "unknown", line, column, ch);
					state = S_ERROR;
					out.type = Token::T_ERROR;
					hasToken = true;
				}
				break;

			case S_IDENTIFIER:
				if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
				} else if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '}') {
					hasToken = true;
					const char *identifier = data + start;
					size_t len = pos - start;
					if (strncmp(identifier, "if", len) == 0) {
						out.type = Token::T_IF;
					} else if (strncmp(identifier, "else", len) == 0) {
						out.type = Token::T_ELSE;
					} else if (strncmp(identifier, "endif", len) == 0) {
						out.type = Token::T_ENDIF;
					} else if (strncmp(identifier, "foreach", len) == 0) {
						out.type = Token::T_FOREACH;
					} else if (strncmp(identifier, "endforeach", len) == 0) {
						out.type = Token::T_ENDFOREACH;
					} else if (strncmp(identifier, "as", len) == 0) {
						out.type = Token::T_AS;
					} else {
						GCM::util::Log::e("Templater", "Parse error in template %s on line %d, column %d: Invalid identifier.", "unknown", line, column);
						out.type = Token::T_ERROR;
						state = S_ERROR;
					}
					hasToken = true;
					state = S_BLOCK_INSIDE;
					this->unget();
				} else {
					out.type = Token::T_ERROR;
					hasToken = true;
					GCM::util::Log::e("Templater", "Parse error in template %s on line %d, column %d: Invalid identifier.", "unknown", line, column);
					state = S_ERROR;
				}
				break;

			case S_VARIABLE_BEGIN:
				if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
					state = S_VARIABLE;
				} else {
					GCM::util::Log::e("Templater", "Parse error in template %s on line %d, column %d: Expected variable.", "unknown", line, column);
					out.type = Token::T_ERROR;
					hasToken = true;
					state = S_ERROR;
				}
				break;

			case S_VARIABLE:
				if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
				} else if (ch == '.') {
					state = S_VARIABLE_BEGIN;
				} else {
					out.type = Token::T_VARIABLE;
					out.content = String(tpl, start, pos - start);
					hasToken = true;
					state = S_BLOCK_INSIDE;
					this->unget();
				}
				break;

			case S_MODIFIER_BEGIN:
				if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
					start = pos;
					state = S_MODIFIER;
				} else {
					out.type = Token::T_ERROR;
					hasToken = true;
					GCM::util::Log::e("Templater", "Parse error in template %s on line %d, column %d: Invalid identifier.", "unknown", line, column);
					state = S_ERROR;
				}
				break;

			case S_MODIFIER:
				if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
				} else {
					out.type = Token::T_FUNCALL;
					out.content = String(tpl, start, pos - start);
					hasToken = true;
					state = S_BLOCK_INSIDE;
					this->unget();
				}
				break;
		}
	}

	if (!hasToken) {
		if (start != tpl->length()) {
			out.type = Token::T_TEXT;
			out.content = String(tpl, start, pos);
		} else {
			out.type = Token::T_EOF;
		}
	}

	//GCM::util::Log::d("Templater|Lexer", "Got token %s", Token::typeToString(out.type));

	return out;
}

const char GCM_API *Templater::Token::typeToString(Templater::Token::TokenType type) {
	switch (type) {
		case T_ERROR: return "T_ERROR";
		case T_TEXT: return "T_TEXT";
		case T_ECHO: return "T_ECHO";
		case T_BLOCK_START: return "T_BLOCK_START";
		case T_VARIABLE: return "T_VARIABLE";
		case T_BLOCK_END: return "T_BLOCK_END";
		case T_IF: return "T_IF";
		case T_ELSE: return "T_ELSE";
		case T_ENDIF: return "T_ENDIF";
		case T_FOREACH: return "T_FOREACH";
		case T_AS: return "T_AS";
		case T_ENDFOREACH: return "T_ENDFOREACH";
		case T_EOF: return "T_EOF";
		case T_FUNCALL: return "T_FUNCALL";
		default: return "";
	}
}

/**
 * Compile the template to opcode
 */
bool GCM_API Templater::compile(String tpl) {
	this->tpl = tpl;
	this->data = tpl->c_str();
	this->line = 1;
	this->column = 0;
	this->state = S_TEXT;
	this->pos = 0;
	this->tempvar = 0;

	tok = this->getNextToken();
	if (!this->body()) {
		GCM::util::Log::e("Templater", "Syntax error in %s on line %d. Unexpected %s.", "unknown", tok.line, Token::typeToString(tok.type));
		return false;
	} else {
		return true;
	}
}

bool GCM_API Templater::body() {
	while (tok.type != Token::T_EOF && tok.type != Token::T_ERROR) {
		//GCM::util::Log::d("Templater", "body");
		switch (tok.type) {
			case Token::T_TEXT: {
				String name = this->mkTempVar();
				this->operations.push_back(Operation(OP_ASSIGN, name, tok.content));
				this->operations.push_back(Operation(OP_ECHO, name));
				tok = this->getNextToken();
				break;
			}

			case Token::T_BLOCK_START: {
				tok = this->getNextToken();
				if (!this->blockStart()) return false;
				break;
			}

			default: {
				return false;
			}
		}

	}

	return tok.type == Token::T_EOF;
}

bool GCM_API Templater::blockStart() {
	//GCM::util::Log::d("Templater", "blockStart");
	switch (tok.type) {
		case Token::T_VARIABLE:
			return this->echoVar();

		case Token::T_IF:
			tok = this->getNextToken();
			return this->ifBegin();

		case Token::T_FOREACH:
			tok = this->getNextToken();
			return this->foreachBegin();

		default:
			return true;
	}
}

bool GCM_API Templater::echoVar() {
	//GCM::util::Log::d("Templater", "echoVar");
	if (tok.type == Token::T_VARIABLE) {
		String var = this->handleVariable();

		if (tok.type == Token::T_BLOCK_END) {
			tok = this->getNextToken();
			this->operations.push_back(Operation(OP_ECHO, var));
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool GCM_API Templater::ifBegin() {
	//GCM::util::Log::d("Templater", "ifBegin");
	// {if

	// <epx>
	String var = this->handleExpression();
	if (var == "") return false;

	// }
	if (tok.type != Token::T_BLOCK_END) return false;
	tok = this->getNextToken();

	size_t ptr = this->operations.size();
	this->operations.push_back(Operation(OP_IF_BEGIN, var));
	this->body();
	// {else
	if (tok.type == Token::T_ELSE) {
		tok = this->getNextToken();
		// }
		if (tok.type != Token::T_BLOCK_END) return false;

		// ptr points to if start
		this->operations[ptr].jump = this->operations.size() + 1;
		ptr = this->operations.size();
		this->operations.push_back(Operation(OP_IF_ELSE));

		tok = this->getNextToken();
		this->body();

		if (tok.type != Token::T_ENDIF) return false;

		tok = this->getNextToken();
		if (tok.type != Token::T_BLOCK_END) return false;

		tok = this->getNextToken();
		this->operations[ptr].jump = this->operations.size();
		return true;
	} else if (tok.type == Token::T_ENDIF) {
		tok = this->getNextToken();
		if (tok.type != Token::T_BLOCK_END) return false;

		tok = this->getNextToken();
		this->operations[ptr].jump = this->operations.size();
		return true;
	} else {
		return false;
	}
}

bool GCM_API Templater::foreachBegin() {
	//GCM::util::Log::d("Templater", "foreachBegin");

	if (tok.type != Token::T_VARIABLE) return false;

	String varName = this->handleVariable();
	if (tok.type != Token::T_AS) return false;

	tok = this->getNextToken();
	if (tok.type != Token::T_VARIABLE) return false;

	String targetName = tok.content;

	tok = this->getNextToken();
	if (tok.type != Token::T_BLOCK_END) return false;

	size_t ptr = this->operations.size();
	this->operations.push_back(Operation(OP_FOREACH_BEGIN, varName, targetName));
	tok = this->getNextToken();
	this->body();

	if (tok.type != Token::T_ENDFOREACH) return false;
	tok = this->getNextToken();

	if (tok.type != Token::T_BLOCK_END) return false;

	tok = this->getNextToken();
	this->operations.push_back(Operation(OP_FOREACH_END));
	this->operations[ptr].jump = this->operations.size();
	this->operations[this->operations.size() - 1].jump = ptr;
	return true;
}


String GCM_API Templater::mkTempVar() {
	return String::format("0%d", tempvar++);
}

String GCM_API Templater::handleVariable() {
	//GCM::util::Log::d("Templater", "handleVariable");
	String varName = tok.content;
	String tempVar = varName;

	tok = this->getNextToken();
	while (tok.type == Token::T_FUNCALL) {
		// Modifier
		tempVar = this->mkTempVar();
		this->operations.push_back(Operation(OP_CALL_CONVFUNC, varName, tok.content, tempVar));

		varName = tempVar;
		tok = this->getNextToken();
	}

	return tempVar;
}

String GCM_API Templater::handleExpression() {
	//GCM::util::Log::d("Templater", "handleExpression");
	// For now, expressions are not supported.
	if (tok.type == Token::T_VARIABLE) {
		return this->handleVariable();
	} else {
		return String();
	}
}

GC<Variable> GCM_API Templater::resolveVariable(String name) {
	const char *data = name->c_str();
	int size = name->length();

	GC<Variable> out = this->variables;

	int start = 0;
	for (int i = start; i < size; i++) {
		const char ch = data[i];
		if (ch == '.' || i == size - 1) {
			if (i == size - 1) i++;
			String identifier = String(name, start, i - start);
			//GCM::util::Log::d("Templater", "Resolving %s", identifier->c_str());
			out = out->access(identifier);
			start = i + 1;
		}
	}

	return out;
}

/**
 * Execute the opcode
 */
String GCM_API Templater::execute() {
	int ip = 0; // Instruction pointer
	String out;

	while (ip < (int)this->operations.size() && ip >= 0) {
		Operation &op = this->operations[ip];
		ip++;

		//GCM::util::Log::d("Templater|Opcode", "opcode = %d, varg1 = %s, varg2 = %s, varg3 = %s", op.opcode, op.varg1->c_str(), op.varg2->c_str(), op.varg3->c_str());

		switch (op.opcode) {
			case OP_ECHO:
				//GCM::util::Log::d("Templater", "Append %s", this->resolveVariable(op.varg1)->toString()->c_str());
				out->append(this->resolveVariable(op.varg1)->toString());
				break;

			case OP_ASSIGN: {
				GC<Variable> var = this->resolveVariable(op.varg1);
				var->assign(op.varg2);
				break;
			}

			case OP_IF_BEGIN:
				if (this->resolveVariable(op.varg1)->toInt() == 0) {
					ip = op.jump;
				}
				break;

			case OP_IF_ELSE:
				ip = op.jump;
				break;

			case OP_FOREACH_BEGIN: {
				GC<Variable> var = this->resolveVariable(op.varg1);
				if (var->getType() == T_HASHTABLE) {
					//GCM::util::Log::d("Templater", "Loop iteration.");
					GCM::GC<HashTable> ht = var->toHashTable();

					if (ht->pos < 0) {
						ht->pos = ht->ordered_begin();
					} else {
						ht->pos++;
					}

					if (ht->pos >= (int)ht->keyOrder.size()) {
						ht->pos = -1;
						ip = op.jump;
					} else {
						this->variables->assign(op.varg2, ht->access(ht->keyOrder[ht->pos]));
					}
				} else {
					//GCM::util::Log::e("Templater", "Can loop only through hash tables.");
					ip = op.jump;
				}
				break;
			}

			case OP_FOREACH_END:
				ip = op.jump;
				break;

			case OP_CALL_CONVFUNC: {
				// Try to locate the conversion function stored in arg 2.
				FormatFunctions::iterator i = getFF().find(op.varg2);
				if (i != getFF().end()) {
					this->variables->assign(op.varg3, i->second(this->resolveVariable(op.varg1)));
				} else {
					GCM::util::Log::d("Templater", "Undefined function %s.", op.varg2->c_str());
					this->variables->assign(op.varg3, String(""));
					//ip = -1; // Kill kill kill!
				}
				break;
			}

			default:
				GCM::util::Log::e("Templater", "Invalid opcode %d.", op.opcode);
				ip = -1;
				break;
		}
	}

	return out;
}

void GCM_API Templater::assign(String name, GC<Variable> value) {
	this->variables->assign(name, value);
}

void GCM_API Templater::registerFormatFunc(String funcName, fFormatFunc callback) {
	getFF()[funcName] = callback;
}

String GCM_API Templater::eval(String tpl) {
	if (this->compile(tpl)) {
		return this->execute();
	} else {
		return "";
	}
}

String GCM_API Templater::debugVar(GC<Variable> var, bool print, int level) {
	String out;

	if (var.getObj() == NULL) {
		var = this->variables;
	}

	switch (var->type) {
		case T_STRING:
			out->append("(string)'");
			out->append(var->toString());
			out->append("'");
			break;

		case T_INT:
			out->append("(int)");
			out->append(String::format("%d", var->toInt()));
			break;

		case T_INT64:
			out->append("(int64)");
			out->append(String::format("%ld", var->toInt64()));
			break;

		case T_DOUBLE:
			out->append("(double)");
			out->append(String::format("%lf", var->toDouble()));
			break;

		case T_HASHTABLE: {
			out->append("htable {\n");

			HashTable::iterator i = var->toHashTable()->begin();
			while (i != var->toHashTable()->end()) {
				out->append(4*level, ' ');
				out->append("'");
				out->append(i->first);
				out->append("' => ");
				out->append(this->debugVar(i->second, false, level + 1));
				out->append("\n");
				i++;
			}

			out->append("}");
			break;
		}

		case T_NULL:
			out->append("NULL");
			break;
	}

	if (print) {
		GCM::util::Log::d("VariableDump", "%s", out->c_str());
	}

	return out;
}

Templater::FormatFunctions GCM_API &Templater::getFF() {
	static FormatFunctions ff;
	return ff;
}
