#include <ui/Dialog.h>
#include <stl/String.h>

using namespace GCM::ui;
using namespace GCM;

Dialog::Dialog(): Object(), mTitle(""), mMessage(""), maxProgress(0), progress(0), cancellable(false), cancelled(false) {
}

void GCM_API Dialog::setRequireInteraction(bool requireInteraction) {
	mRequireInteraction = requireInteraction;
	this->update();
}

bool GCM_API Dialog::getRequireInteraction() {
	return mRequireInteraction;
}

void GCM_API Dialog::setTitle(const char *title) {
	mTitle = title;
	this->update();
}

void GCM_API Dialog::setTitle(String title) {
	mTitle = title;
	this->update();
}

GCM::String GCM_API Dialog::getTitle() {
	return mTitle;
}

void GCM_API Dialog::setMessage(const char *message) {
	mMessage = message;
	this->update();
}

void GCM_API Dialog::setMessage(String message) {
	mMessage = message;
	this->update();
}

GCM::String GCM_API Dialog::getMessage() {
	return mMessage;
}

void GCM_API Dialog::dismiss() {
}

void GCM_API Dialog::update() {
}

void GCM_API Dialog::setProgress(int progress) {
	this->progress = progress;
	this->update();
}

void GCM_API Dialog::setMaxProgress(int progress) {
	this->maxProgress = progress;
	this->update();
}

int GCM_API Dialog::getMaxProgress() {
	return this->maxProgress;
}

int GCM_API Dialog::getProgress() {
	return this->progress;
}

float GCM_API Dialog::getPercent() {
	if (this->maxProgress <= 0) {
		return 0;
	} else {
		float out = this->progress * 100 / this->maxProgress;
		if (out < 0) return 0;
		if (out > 100) return 100;
		return out;
	}
}

GCM_API Dialog::~Dialog() {

}

bool GCM_API Dialog::isCancellable() {
	return this->cancellable;
}

void GCM_API Dialog::setCancellable(bool cancellable) {
	this->cancellable = cancellable;
}

void GCM_API Dialog::setCancelled(bool cancelled) {
	if (cancelled && !this->cancelled) {
		this->events.trigger("cancelled", this);
	}

	this->cancelled = cancelled;
}

bool GCM_API Dialog::isCancelled() {
	return this->cancelled;
}