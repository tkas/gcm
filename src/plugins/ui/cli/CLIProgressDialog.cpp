#include "CLIProgressDialog.h"
#include <stdio.h>
#include <util/Log.h>
#include <unistd.h>

#ifdef _WIN32
# include <windows.h>
#endif

CLIProgressDialog::CLIProgressDialog(): Dialog(), visible(false) {
}

#ifdef _WIN32
void clrline(short int srow, short int erow) {
	int numSpaces; // Holds the number of spaces to output.
	COORD new_window; //Stores the Top, Left coordinates of new window.
	DWORD CharsWritten;

	// Set the starting row to clear, column always 0.
	//X and Y values are in a user-defined C structure
	new_window.X = 0;
	new_window.Y = srow - 1;

	// Calculate the number of spaces to output.
	numSpaces = (erow - srow) * 80 + 80;

	// Clear lines.
	// Attribute byte set to Light Gray (0x07) on Black (0x00)
	FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x00 | 0x07,
		numSpaces, new_window, &CharsWritten);
	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', numSpaces, new_window,
		&CharsWritten);

	// Position current cursor position at starting row, column 0.
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), new_window);
}

COORD getcursorpos() {
	COORD out;
	CONSOLE_SCREEN_BUFFER_INFO bi;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bi)) {
		out = bi.dwCursorPosition;
	}
	return out;
}

#endif

void CLIProgressDialog::dismiss() {
	if (this->visible) {
#ifndef _WIN32
		// Clear both lines for percent updates.
		if (this->getMaxProgress() > 0) {
			printf("\033[2K\033[1A\033[2K");
		}

		// Clear the line.
		printf("\033[2K");
#else
		COORD coord = getcursorpos();
		if (this->getMaxProgress() > 0) {
			// Clear two lines
			clrline(coord.Y - 1, coord.Y);
		} else {
			// Clear just one line
			clrline(coord.Y, coord.Y);
		}
#endif

		// Report done.
		printf("\r%s ... Done\n", this->getMessage()->c_str());
		fflush(stdout);
	} else {
		printf(" ...done.\n");
	}
}

void CLIProgressDialog::show() {
#ifndef _WIN32
	if (isatty(fileno(stdout))) {
#else
	if (GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)) == FILE_TYPE_CHAR) {
#endif
		this->visible = true;
	} else {
		printf("%s", this->getMessage()->c_str());
	}

	this->update();
}

void CLIProgressDialog::update() {
	if (!this->visible) return;

	if (this->getMaxProgress() > 0) {
		float percent = this->getPercent();
		// Print the message and percentage indicator.
#ifndef _WIN32
		printf("\033[1A\033[2K\r%s\n\033[2K\r %3.0f%% [",
			this->getMessage()->c_str(),
			percent);
#else
		// Clear last two lines and redraw
		COORD coord = getcursorpos();
		clrline(coord.Y - 1, coord.Y);
		printf("%s\n %3.0f%% [", this->getMessage()->c_str(), percent);
#endif

		char spaces[51] =
			"                                                  ";
		char progress[51] =
			"==================================================";

		printf("%s", progress + 50 - (int)(percent / 2));
		printf(">");
		printf("%s] ", spaces + (int)(percent / 2));

	} else {
		// Print the rotor when we don't have progress indicator.
		int rotor = 0;
		char rot[9] = "-\\|/-\\|/";
#ifndef _WIN32
		printf("\033[2K\r %c %s",
			rot[rotor],
			this->getMessage()->c_str());
#else
		COORD coord = getcursorpos();
		clrline(coord.Y, coord.Y);
		printf(" %c %s", rot[rotor], this->getMessage()->c_str());
#endif
		rotor = (rotor + 1) % 8;
	}

	fflush(stdout);
}
