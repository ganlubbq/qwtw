
/**

	2D and 3D line views container.
	

	\file sfigure.cpp
	\author   Igor Sandler
	\date    Jul 2009
	\version 1.0
	
*/



#include "sfigure.h"
#include "figure2.h"
#include <sstream>
#include "justaplot.h"
#include "xstdef.h"
#include <QApplication>
#include "qdesktopwidget.h"
//#include "topviewplot.h"

#ifdef USEMARBLE
#include "marbleview.h"
#endif
#ifdef USE_QT3D
#include "qt-3d.h"
#endif

XQPlots::XQPlots(QWidget * parent1): QDialog(parent1) {
	parent = parent1;
	cf = 0;  //cf3 = 0;
	markersAreVisible = false;
	//currentFigureMode = 2;  //  
	clearingAllFigures = false;
	currentImportanceMode = true;
	//memset(ecefOrigin, 0, 3*sizeof(double));  //  this means 'not set'
	ui.setupUi(this);

	/*QStandardItem *i0 = pim.invisibleRootItem();
	i0->insertColumn(0, new QStandardItem("key"));

	for (int i = 0; i < 4; ++i) {
		QStandardItem *item = new QStandardItem(QString("item %0").arg(i));
		i0->appendRow(item);
		i0 = item;
	}
	*/
	ui.tv->setModel(&pim);
	connect(ui.tv, SIGNAL(clicked(QModelIndex)), this, SLOT(onTvItemClicked(QModelIndex)));

	connect(ui.tbClosePlots, SIGNAL(clicked(bool)), this, SLOT(onCloseAllPlots(bool)));
	connect(ui.tbShowEverything, SIGNAL(clicked(bool)), this, SLOT(onShowAllPlots(bool)));

}

XQPlots::~XQPlots() {
	onExit();
}

void XQPlots::setmode(int mode_) {
    //currentFigureMode = mode_;
}

void XQPlots::setImportant(bool i) {
	currentImportanceMode = i;
}

void XQPlots::clear() {
	clearFigures();
}

void XQPlots::showMainWindow() {
	show();

	activateWindow();
	raise();
	//showMaximized();
	showNormal();
}

JustAplot* XQPlots::figure(std::string name_, int type) {
	std::map<std::string,  JustAplot*>::iterator it = figures.find(name_);

	if (it != figures.end()) {
		cf = it->second;
		cf->activateWindow();
		cf->raise();
		//cf->showMaximized();
		cf->showNormal();

	}   else  {

		switch(type) {
		case 1:
			cf	= new Figure2(name_, this, parent);
			break;
#ifdef USEMARBLE
		case 2: {
			MarView* tvp = new MarView(name_, this, parent);
			tvp->mvInit();
			cf = tvp;
			}
			break;
#endif
#ifdef USE_QT3D
		case 3: {
			Q3DView* q3 = new Q3DView(name_, this, parent);
			q3->q3Init();
			cf = q3;
		}
			break;
#endif
		}
		bool test = true;
		test = connect(cf, SIGNAL(exiting (const std::string&)), this, SLOT(onFigureClosed(const std::string&)));
		test = connect(cf, SIGNAL(onSelection(const std::string&)), this, SLOT(onSelection(const std::string&)));
		test = connect(cf, SIGNAL(onPicker(const std::string&, double, double)), this, SLOT(onPicker(const std::string&, double, double)));

		figures.insert(make_pair(name_, cf));
		cf->show();

		// add to tree view:
		//ui.tv->iie
		QStandardItem *i0 = pim.invisibleRootItem();
		QList<QStandardItem *> raw; 
		raw.append(new QStandardItem(cf->key.c_str()));
		raw.append(new QStandardItem(cf->name.c_str()));

		i0->appendRow(raw);
	}
	return cf;
}
void XQPlots::onTvItemClicked(QModelIndex mi) {
	QStandardItem *i = pim.itemFromIndex(mi);
	JustAplot* p = getPlotByName(i->text().toUtf8().toStdString());
	if (p != 0) {
		p->activateWindow();
		p->raise();
		//p->showMaximized();
		p->showNormal();
	}

}
JustAplot* XQPlots::getPlotByName(std::string s) {
	JustAplot* ret = 0;

	std::map<std::string, JustAplot*>::iterator it;
	for (it = figures.begin(); it != figures.end(); it++) {
		if (it->second->name.compare(s) == 0) {
			ret = it->second;
			break;
		}
	}
	return ret;
}

void XQPlots::onCloseAllPlots(bool checked) {
	clear();
}

void XQPlots::onShowAllPlots(bool checked) {
	int n = figures.size();
	if (n < 1) return;
	double side = sqrt(n);
	int w = ceil(side);
	int h = floor(side);
	if (w*h < n) {
		h++;
	}
	mxat(w*h >= n);
	QRect rec = QApplication::desktop()->screenGeometry();
	int height = rec.height();
	int width = rec.width() - 48;
	int hh = floor(height / h);
	int ww = floor(width / w);
	int i = 0, j = 0;
	QRect g, g1, g2;
	int dx, dy;

	for (FSet::iterator it = figures.begin(); it != figures.end(); it++) {
		g1 = it->second->frameGeometry();
		g2 = it->second->geometry();
		dx = g1.width() - g2.width();
		dy = g1.height() - g2.height();

		
		it->second->showNormal();
		it->second->resize(ww - dx, hh - dy);
		it->second->move(i*ww, j*hh);

		/*g.setTop(j*hh); 
		g.setBottom((j + 1)*hh);
		g.setLeft(i*ww);
		g.setRight((i + 1)*ww);
		*/
		
		//it->second->setGeometry(g);
		
		it->second->show();
		it->second->raise();
		
		i++;
		if (i >= w) {
			i = 0;
			j++;
		}
	}

}

void XQPlots::drawMarker(const std::string& key_, double X, double Y, int type) {
	//if (!markersAreVisible) markersAreVisible = true;
	std::map<std::string,  JustAplot*>::iterator it = figures.find(key_);
	if (it != figures.end()) {
		it->second->drawMarker(X, Y, type);
	}
}

void XQPlots::setAllMarkersVisible(bool visible) {
	std::map<std::string, JustAplot*>::iterator it;
	for (it = figures.begin(); it != figures.end(); it++) {
		it->second->makeMarkersVisible(visible);
	}
}

void XQPlots::drawAllMarkers(double t) {
	//if (!markersAreVisible) markersAreVisible = true;
    std::map<std::string, JustAplot*>::iterator it;
    for(it = figures.begin(); it != figures.end(); it++) {
	   it->second->drawMarker(t);
    }
	for (it = figures.begin(); it != figures.end(); it++) {
		it->second->replot();
	}
}

void XQPlots::clipAll(double t1, double t2) {
	std::map<std::string, JustAplot*>::iterator it;
	for (it = figures.begin(); it != figures.end(); it++) {
		it->second->onClip(t1, t2);
	}
}

JustAplot* XQPlots::figure(int n, int type) {
	char buf[32];
	std::string fName;
	if (n == 0) { //  this means 'create new'
		for (int i = 500; i < 1500; i++) {
			snprintf(buf, 32, "%d", i);
			fName.assign(buf);
			if (figures.find(fName) == figures.end()) { //   new value
				n = i;
				break;
			}
		}
	}
	
	snprintf(buf, 32, "%d", n);
	fName.assign(buf);
	return figure(fName, type);
}

void XQPlots::title(const std::string& s) {
	if (cf == 0) {
		return;
	}
	cf->title(s);

	//  change title in the tv:
	QList<QStandardItem *> si = pim.findItems(cf->key.c_str());
	if (si.size() > 0) {
		si.at(0)->setText(s.c_str());
	}
}
void XQPlots::footer(const std::string& s) {
	if (cf == 0) {
		return;
	}
	cf->footer(s);
}

/*
void XQPlots::setaxesequal() {
	if (cf == 0) {
		return;
	}
	//cf->setAxesEqual();
}
*/
void XQPlots::xlabel(const std::string& s) {
	if (cf == 0) {
		return;
	}
	cf->xlabel(s);
}

void XQPlots::ylabel(const std::string& s) {
	if (cf == 0) {
		return;
	}
	cf->ylabel(s);
}

void  XQPlots::plot(double* x, double* y, int size, const char* name, 
					     const char* style, int lineWidth, int symSize,
						double* time) {
	mxassert((x != 0) && (y != 0) && (size > 0) && (name != 0) && (style != 0), "");
	if (cf == 0) {
		figure(1);
	}

	//int mode = currentFigureMode;
	int mode = 2;
	if(time != 0) {
	    mode = 3;
	} 

	//it will be deleted in 'cf' destructor
	LineItemInfo* i = new LineItemInfo(x, y, size, name, mode, time);
	i->style = style; 
	i->lineWidth = lineWidth;
	i->symSize = symSize;
	i->important = currentImportanceMode;

	cf->addLine(i);
}

void  XQPlots::plot(double* x, double* y, double* z, int size, const char* name,
	const char* style, int lineWidth, int symSize,
	double* time) {
	mxassert((x != 0) && (y != 0) && (z != 0) && (size > 0) && (name != 0) && (style != 0), "");
	if (cf == 0) {
		figure(0, 3);
	} else { ///  check:
		mxat(cf->type == 3);
		if (cf->type != 3) {
			return;
		}
	}

	//it will be deleted in 'cf' destructor
	LineItemInfo* i = new LineItemInfo(x, y, z, size, name, time);
	i->style = style;
	i->lineWidth = lineWidth;
	i->symSize = symSize;
	i->important = currentImportanceMode;

	cf->addLine(i);
}


void XQPlots::onExit() {
	clearFigures();
}

void XQPlots::clearFigures() {
	clearingAllFigures = true;
	std::map<std::string,  JustAplot*>::iterator it = figures.begin();
	while (it != figures.end()) {
		cf = it->second;
		cf->close();
		delete cf; 
		cf = 0;
		it++;
	}
	figures.clear();
	clearingAllFigures = false;

	pim.clear();
}

void XQPlots::onSelection(const std::string& key) {
	std::map<std::string,  JustAplot*>::iterator it = figures.find(key);
	if (it != figures.end()) {
		cf = it->second;
		emit selection(key);
	} else {  //  error?

	}
}

void XQPlots::onPicker(const std::string& key_, double X, double Y) {
	emit picker(key_, X, Y);
}

void XQPlots::onFigureClosed(const std::string& key) {
	if (clearingAllFigures) {
		return;
	}
	JustAplot* f = 0;
	std::map<std::string,  JustAplot*>::iterator it = figures.find(key);
	if (it != figures.end()) {
		f = it->second;  //  DO WE NEED THIS???

		//  remove from tv:
		QList<QStandardItem *> si = pim.findItems(f->name.c_str());
		if (si.size() != 0) {
			QModelIndex mi = si.at(0)->index();
			pim.removeRow(mi.row());
		}

		emit figureClosed(key);
		
		// !!!!!!!!!!!!!!delete cf;    because of setAttribute(Qt::WA_DeleteOnClose);
		disconnect(f, 0, 0, 0);
		figures.erase(it);

		

	}   else  {
		//   error? 
	}

	if (f == cf) { // we need new 'cf'
		//   set cf   to the new value:
		it = figures.begin();
		if (it != figures.end()) {
			cf = it->second;
		} else { 
			cf = 0;
		}
	}
}

//void XQPlots::setEcefOrigin(double* origin) {
//	memcpy(ecefOrigin, origin, 3*sizeof(double));
//}

