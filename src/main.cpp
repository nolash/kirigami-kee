#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QDebug>
#include <QtQuick/qquickview.h>
#include <KLocalizedContext>
#include <KLocalizedString>
#include "settings.h"
#include "backend.h"
#include "credit.h"


int main(int argc, char *argv[]) {
	int r;

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
	CreditListModel credit_model;
	const Credit &item = Credit("foo", "bar");
	credit_model.addItem(item);
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
