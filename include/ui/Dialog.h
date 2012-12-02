#ifndef _GCM_UI_DIALOG_H
# define _GCM_UI_DIALOG_H 1

#include <gcm.h>
#include <Object.h>
#include <stl/String.h>
#include <Events.h>

namespace GCM {
	namespace ui {

class GCM_API Dialog: public Object {
	private:
		bool mRequireInteraction;
		String mTitle;
		String mMessage;
		int maxProgress;
		int progress;

		bool cancellable;
		bool cancelled;

	public:
		Events events;

		Dialog();

		virtual void setRequireInteraction(bool requireInteraction);
		virtual bool getRequireInteraction();

		virtual void setTitle(const char *title);
		virtual void setTitle(String title);
		virtual String getTitle();

		virtual void setMessage(const char *message);
		virtual void setMessage(String message);
		virtual String getMessage();

		virtual void setProgress(int progress);
		virtual void setMaxProgress(int maxProgress);
		virtual int getMaxProgress();
		virtual int getProgress();
		virtual float getPercent();

		virtual bool isCancellable();
		virtual void setCancellable(bool cancellable);
		virtual void setCancelled(bool cancelled);
		virtual bool isCancelled();

		virtual void show() = 0;
		virtual void update();
		virtual void dismiss();
		virtual ~Dialog();
};

	} // namespace ui
} // namespace GCM

#endif
