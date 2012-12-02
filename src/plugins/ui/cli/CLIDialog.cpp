#include "CLIDialog.h"
#include <util/Log.h>

CLIDialog::CLIDialog(): Dialog(), visible(false) {
}

void CLIDialog::show() {
	this->visible = true;
	this->update();
}

void CLIDialog::update() {
	if (this->visible) {
		if (this->getMessage()->length() > 0) {
			GCM::util::Log::d("CLI", this->getMessage()->c_str());
		}
	}
}

CLIDialog::~CLIDialog() {}

