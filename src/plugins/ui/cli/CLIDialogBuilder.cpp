#include "CLIDialogBuilder.h"
#include "CLIDialog.h"
#include "CLIProgressDialog.h"

ptrDialog CLIDialogBuilder::buildStartupDialog() {
	return ptrDialog(new CLIDialog());
}

ptrDialog CLIDialogBuilder::buildProgressDialog() {
	return ptrDialog(new CLIProgressDialog());
}

ptrDialog CLIDialogBuilder::buildAlertDialog() {
	return ptrDialog(new CLIDialog());
}

CLIDialogBuilder::~CLIDialogBuilder() {
}
