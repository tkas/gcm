#include <templater/Library.h>
#include <templater/Templater.h>

using namespace GCM::templater;
using namespace GCM;

Library::Library() {
	REGFFUNC(nl2br);
	REGFFUNC(htmlspecialchars);
	REGFFUNC(length);
}

FFUNC(Library::nl2br) {
	return new Variable(var->toString()->replace("\n", "<br />\n"));
}

FFUNC(Library::htmlspecialchars) {
	String out = var->toString()
		->replace("&", "&amp;")
		->replace("\"", "&quot;")
		->replace("<", "&lt;")
		->replace(">", "&gt;");
	return new Variable(out);
}

FFUNC(Library::length) {
	if (var->getType() == T_STRING) {
		return new Variable((int)var->toString()->length());
	} else if (var->getType() == T_HASHTABLE) {
		return new Variable((int)var->toHashTable()->size());
	} else {
		return new Variable();	
	}
}
