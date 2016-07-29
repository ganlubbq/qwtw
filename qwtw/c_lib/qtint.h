

#pragma once

#include <QObject>
//#include "queue_block.h"
#include "lrbuf.h"
#include "qtswidget.h"

class QTSMainWidget;

class QWController {
public:
	QWController();
	/// @return 0 if all is OK
	int init(LRBuf<BQInfo, 16>* bq_);
	void figure(int n);  // 1
	void title(char* s); // 2
	void xlabel(char* s); // 3
	void ylabel(char* s); // 4
	void setmode(int mode); // 6
	void clear(); // 11
	void showMW();
	void setImportantStatus(int status); // 10
#ifdef USEMARBLE
	void figure_topview(int n);  // 7    (top view)
#endif
	void plot(double* x, double* y, int size, char* name, const char* style, 
		  int lineWidth, int symSize, double* time = 0); // 5
	void close(); // NOT WORKING
	
private:
	bool ok;
	//QueueBlock<BQInfo, 16>* bq;
	LRBuf<BQInfo, 16>* bq;
	void send(unsigned char cmd, const char* b, int size, bool waitForReply = false);
};

extern QWController* qwtController;

class XQPlots;
class QWTest : public QTSMainWidget {
	Q_OBJECT
public:
	QWTest(LRBuf<BQInfo, 16>* bq_);
protected:
	void onInfo(BQInfo x);
private:
	XQPlots* pf;
};









