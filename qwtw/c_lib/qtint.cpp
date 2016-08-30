
#include "qtint.h"
#include "sfigure.h"
#include "xqbytebuffer.h"
#include "xstdef.h"
#include "qtswidget.h"
#include "xmutils.h"
#include "sfigure.h"
#include <boost/thread.hpp>
#include <QApplication>


// ################################################################
// ################################################################
// ################################################################
// ################################################################

QWController* qwtController = 0;
QWTest* qwTest = 0;
QApplication* qApp123 = 0;
boost::thread* bt = 0;
LRBuf<BQInfo, 16>* bq = 0;


QWController::QWController() {
	ok = false;
	
}

int QWController::init(LRBuf<BQInfo, 16>* bq_) {
	bq = bq_;
	return 0;
}

#ifdef LIN_UX
	#include <signal.h>
#endif

void QWController::close() {
    //xm_printf("got QWController::close() \n");
    try {
	   send(0, 0, 0);  //    exit eberybody!
    } catch(...) {
	   xm_printf("QWController::close(): send(0, 0, 0) faild \n");
    }
    boost::this_thread::sleep(boost::posix_time::milliseconds(54));
	//bq->abort();
    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	if (bt != 0) {
        bt->try_join_for(boost::chrono::milliseconds(24));
		bt->interrupt();
#ifdef WIN32
		TerminateThread(bt->native_handle(), 0);
#else
        bt->try_join_for(boost::chrono::milliseconds(10));
    //	pthread_kill(bt->native_handle(), 9);
#endif

    }
}

static unsigned int trCounter = 0;

void QWController::send(unsigned char cmd, const char* b, int size, bool waitForReply) {
	BQInfo i;
	i.cmd = cmd;
	//strncpy(i.data, b, BQInfo::dsize - 1);
	//qmw->command(i);
	int bs = size;
	mxassert(size <= BQInfo::dsize, "");
	if (size >= BQInfo::dsize) {
		xm_printf("ERROR: QWController::send overflow\n");
		bs = BQInfo::dsize - 1;
	}
	if (bs > 0) {
		memcpy(i.data, b, bs);
	}
	bq->put(i, waitForReply);
	//bq->put(i);
}

void QWController::figure(int n) {
	char m[64]; XQByteBuffer b(m, false);
	b.putInt(n);
	send(1, m, b.getSize());
}

#ifdef USEMARBLE
void QWController::figure_topview(int n) {
	char m[64]; XQByteBuffer b(m, false);
	b.putInt(n);
	send(7, m, b.getSize());
}
#endif

void QWController::title(char* s) {
	char m[256]; XQByteBuffer b(m, 255);
	b.putString(s); 
	send(2, m, b.getSize());
}

void QWController::xlabel(char* s) {
	char m[256]; XQByteBuffer b(m, 255);
	b.putString(s); 
	send(3, m, b.getSize());
}

void QWController::ylabel(char* s) {
	char m[256]; XQByteBuffer b(m, 255);
	b.putString(s); 
	send(4, m, b.getSize());
}

void QWController::setmode(int mode) {
    char m[64]; XQByteBuffer b(m, false);
    b.putInt(mode);
    send(6, m, b.getSize());
}

void QWController::clear() {
	send(11, 0, 0);
}

void QWController::showMW() {
	send(12, 0, 0);
}


void QWController::setImportantStatus(int status) {
	char m[64]; XQByteBuffer b(m, false);
	b.putInt(status);
	send(10, m, b.getSize());
}

void QWController::plot(double* x, double* y, int size, char* name, const char* style, 
		  int lineWidth, int symSize, double* time) {
	char m[256];  int k = 0;
	XQByteBuffer b(m, false);
	b.putDPtr(x); b.putDPtr(y); b.putInt(size);
	b.putString(name);  b.putString(style);  b.putInt(lineWidth); b.putInt(symSize);
	b.putDPtr(time);
	send(5, m, b.getSize(), true);
}

// ################################################################
// ################################################################
// ################################################################
// ################################################################



// ################################################################
// ################################################################
// ################################################################
// ################################################################


QWTest::QWTest(LRBuf<BQInfo, 16>* bq_): QTSMainWidget(bq_) {
	pf = 0;
}

void QWTest::onInfo(BQInfo x) {
    //xm_printf("QWTest::onInfo() !!!!!!!!!!! cmd = %d\n", x.cmd);
	if (pf == 0) {
		//pf = new XQPlots(this);
		pf = new XQPlots();
	}
	int n, mode;
	XQByteBuffer b(x.data, false);
	char str1[256];
	char style[8];
	double* xx;
	double* yy;
	double* time = 0;
	int size, linewidth, symsize;

	switch (x.cmd) {
	case 0: 
        //xm_printf("got exit command!\n");
		stopQueueThread();
		//QApplication::quit();
		QApplication::exit();
		break;
	case 1: // figure
		n = b.getInt();
		pf->figure(n, 1);
		break;
	case 2: //  title
		b.getString(str1);
		pf->title(str1);
		break;
	case 3: //  xlabel
		b.getString(str1);
		pf->xlabel(str1);
		break;
	case 4: //  ylabel
		b.getString(str1);
		pf->ylabel(str1);
		break;
	case 5: // plot
		xx = b.getDPtr(); yy = b.getDPtr();
		size = b.getInt(); b.getString(str1); b.getString(style);

		linewidth = b.getInt(); symsize = b.getInt();
		time = b.getDPtr();
		pf->plot(xx, yy, size, str1, style, linewidth, symsize, time);
		sendReply(); //    notify 'host' that it can return from function and free the memory
		break;
	case 6:
	   mode = b.getInt();
	   pf->setmode(mode);
	   break;

#ifdef USEMARBLE
	case 7: // top view figure
		n = b.getInt();
		pf->figure(n, 2);
		break;
#endif
	case 10:  //   important status
		pf->setImportant(b.getInt() != 0);
		break;

	case 11:  //  close all plots
		pf->clear();
		break;
	case 12:  //  show main window
		pf->showMainWindow();
		break;

	default: xm_printf("QWTest::onInfo(): uncnown  command #%d\n", x.cmd);  break;
	};
}

void startQT_2();
void startQT_1() {
	//bt = new boost::thread(boost::bind(startQT_2, (void*)0));
	bt = new boost::thread(startQT_2);
	boost::this_thread::sleep(boost::posix_time::milliseconds(34));
}

void startQT_2() {
	mxassert(qApp123 == 0, "qtsMain::runQT error #1 ");
	//  create arguments for QApplication:
	static char argv0[256];
	getExeFilePath(argv0, 255);

	//#ifdef _DEBUG
	if (argv0[0] == 0) {
		xm_printf("QTSMain::runQT() error #74635 \n");
		return;
	}
	else {
		xm_printf("QTSMain::runQT() start [%s]\n", argv0);
	}
	//#endif

	int argc = 1;
	static char* argv[2];
	argv[0] = argv0;

	// http://habrahabr.ru/post/188816/ :
	QStringList paths = QCoreApplication::libraryPaths();
	paths.append(".");
	paths.append("imageformats");
	paths.append("platforms");
	paths.append("sqldrivers");

	//paths.append("C:\\ProgramData\\qwtw");
#ifdef WIN32
	std::string qwtwSysPath = getCommonAppDataPath().append("\\qwtw");
#else
	std::string qwtwSysPath = getCommonAppDataPath().append("/qwtw");
#endif
	paths.append(qwtwSysPath.c_str());
	QCoreApplication::setLibraryPaths(paths);

	qApp123 = new QApplication(argc, argv);
	qApp123->setQuitOnLastWindowClosed(false);
	//Q_INIT_RESOURCE(plib1);
	//qtsMainWidget->setHidden(true);
	
	//#ifdef _DEBUG
	//qts2.start();
	if (qwTest == 0) {
		qwTest = new QWTest(bq);
	}
	qwTest->start();
	
    //xm_printf("\tQApplication created; starting QT \n");
	//#endif
	qApp123->exec();

    //xm_printf("\tQApplication exec finished \n");
}

void startQWT() {
	int ok;
	bq = new LRBuf<BQInfo, 16>();
	//xm_printf("qwtw: 'startQWT' starting \n");
	if (qwtController == 0) {
		qwtController = new QWController();
	}
	qwtController->init(bq);

	qRegisterMetaType<BQInfo>();
	startQT_1();
}

void stopQWT() {
	if (qwtController != 0) {
		qwtController->close();
	}
}

class DLLStarter {
public:
    DLLStarter() {
        startQWT();
    }
    ~DLLStarter() {
#ifdef WIN32
		stopQWT();
#endif
    }

};
static DLLStarter ourDllStarter;


