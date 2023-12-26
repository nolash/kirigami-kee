#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QDebug>
#include <QLocalServer>
#include <QtQuick/qquickview.h>
#include <KLocalizedContext>
#include <KLocalizedString>
#include "settings.h"
#include "backend.h"
#include "credit.h"
#include "db.h"
#include "rpc.h"


int main(int argc, char *argv[]) {
	int r;
	int c;
	char buf[1024];
	char *k;
	size_t kl;
	char *v;
	size_t vl;
	Import *im;
	
	std::string s;
	QString sq;
	Db *db;

	Settings settings = Settings();
	r = settings.init();
	if (r) {
		return r;
	}
	//settings.set(SETTINGS_LOCKTIME, "2");

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);
	KLocalizedString::setApplicationDomain("credittracker");
	QCoreApplication::setOrganizationName(QStringLiteral("defalsify"));
	QCoreApplication::setOrganizationDomain(QStringLiteral("defalsify.org"));
	QCoreApplication::setApplicationName(QStringLiteral("Bilateral Credit Tr4ck3R"));

	QQmlApplicationEngine engine;

	Backend backend;

	RpcSocket *rpc;
	
	rpc = new RpcSocket(&backend);
	rpc->setSocketOptions(QLocalServer::UserAccessOption);
	s = settings.get(SETTINGS_RUN) + "/sock.ipc";
	rpc->listen(QString::fromStdString(s));

	s = settings.get(SETTINGS_DATA);
	db = new Db(s);
	r = db->connect();
	if (r) {
		return 1;
	}

	CreditListModel credit_model;

	c = 0;
	while (1) {
		r = db->next(DbKeyCreditItem, &k, &kl, &v, &vl);
		if (r) {
			break;
		}
		memcpy(buf+c, v, vl);
		c += vl;
	}

	im = new Import(buf, c); // improve load buffered direct from file
	while (1) {
		const Credit &item = Credit(im);
		if (im->done()) {
			break;
		}
		credit_model.addItem(item);
	}

	//qmlRegisterType<List>("org.defalsify.kde.credittracker", 1, 0, "List");
	qmlRegisterSingletonInstance<Backend>("org.defalsify.kde.credittracker", 1, 0, "Backend", &backend);
	r = backend.init(&settings);
	if (r) {
		return 1;
	}

	engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
	engine.rootContext()->setContextProperty(QStringLiteral("creditModel"), &credit_model);
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	if (engine.rootObjects().isEmpty()) {
		return -1;
	}

	return app.exec();
}
