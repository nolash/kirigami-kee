#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QDebug>
#include <KLocalizedContext>
#include <KLocalizedString>
#include "settings.h"
#include "backend.h"
#include "gpg.h"

int main(int argc, char *argv[]) {
	int r;

	Settings settings = Settings();

	GpgStore gpg;
	r = gpg.check(settings.get(SettingsType::DATA));
	if (r) {
		return 1;		
	}

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);
	KLocalizedString::setApplicationDomain("credittracker");
	QCoreApplication::setOrganizationName(QStringLiteral("defalsify"));
	QCoreApplication::setOrganizationDomain(QStringLiteral("defalsify.org"));
	QCoreApplication::setApplicationName(QStringLiteral("Bilateral Credit Tr4ck3R"));

	QQmlApplicationEngine engine;

	Backend backend;
	qmlRegisterSingletonInstance<Backend>("org.defalsify.kde.credittracker", 1, 0, "Backend", &backend);

	engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	if (engine.rootObjects().isEmpty()) {
		return -1;
	}

	return app.exec();
}
