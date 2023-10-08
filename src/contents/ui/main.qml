import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.defalsify.kde.credittracker 1.0

Kirigami.ApplicationWindow {
	id: root
	title: i18nc("@title:window", "Bilateral credit tracker")

	pageStack.initialPage: Kirigami.Page  {
		Controls.Label {
			anchors.centerIn: parent
			text: i18n("Not logged in")
			Component.onCompleted: Backend.update(42)
		}	
	}
}
