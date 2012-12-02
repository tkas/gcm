#include <ui/Command.h>

using namespace GCM;
using namespace GCM::ui;

static GCM::GC<Command> SEPARATOR(new Separator());

Command::Command(GCM::String name, GCM::String id): Object(), name(name), id(id), help("") {}
Command::Command(GCM::String name, GCM::String id, GCM::String help): Object(), name(name), id(id), help(help) {};

bool Command::isSubmenu() {
	return false;
}

void Command::setName(GCM::String name) {
	this->name = name;
}

void Command::setId(GCM::String id) {
	this->id = id;
}

void Command::setHelp(GCM::String help) {
	this->help = help;
}

void Command::setSelectedCaches(GCM::GC<GCM::geolib::GeocacheList> selection) {
	this->selectedCaches = selection;
}

GCM::String Command::getName() {
	return this->name;
}

GCM::String Command::getId() {
	return this->id;
}

GCM::String Command::getHelp() {
	return this->help;
}

GCM::GC<GCM::geolib::GeocacheList> Command::getSelectedCaches() {
	return this->selectedCaches;
}

CommandMenu::CommandMenu(String name, String id): Command(name, id), subcommands(new List<Command>()) {
}

CommandMenu::CommandMenu(String name, String id, String help): Command(name, id, help), subcommands(new List<Command>()) {
}

bool CommandMenu::isSubmenu() {
	return true;
}

GCM::GC<Command> CommandMenu::addCommand(GC<Command> cmd) {
	this->subcommands->push_back(cmd);
	return cmd;
}

GCM::GC< List<Command> > CommandMenu::getCommands() {
	return this->subcommands;
}

void CommandMenu::triggered() {
}
