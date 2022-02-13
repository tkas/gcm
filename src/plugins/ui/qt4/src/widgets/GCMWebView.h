#ifndef GCMWEBVIEW_H
#define GCMWEBVIEW_H

//#include <QWebView>
#include <QtWebKitWidgets/qwebview.h>
#include <QWidget>
#include <util/Log.h>

class GCMWebView : public QWebView
{
public:
	GCMWebView(QWidget *parent = 0);

protected:
	virtual void javascriptConsoleMessage( const QString & message, int lineNumber, const QString & sourceID ) {
		GCM::util::Log::e("Qt4GUI|WebView", "%s (in %s on line %d).", message.toUtf8().data(), sourceID.toUtf8().data(), lineNumber);
	}
};

#endif // GCMWEBVIEW_H
