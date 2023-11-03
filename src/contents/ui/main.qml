import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.defalsify.kde.credittracker 1.0


Kirigami.ApplicationWindow {
	id: root
	title: i18nc("@title:window", "Bilateral credit tracker")

	globalDrawer: Kirigami.GlobalDrawer{
		isMenu: true
		actions: [
			Kirigami.Action {
				text: i18n("Lock")
				onTriggered: Backend.lock()
			},
			Kirigami.Action {
				text: i18n("Quit")
				icon.name: "gtk-quit"
				shortcut: StandardKey.Quit
				onTriggered: Qt.quit()
			}

		]
	}


	pageStack.initialPage: Kirigami.Page  {
		Controls.Label {
			id: splashlabel
			anchors.fill: parent
			text: i18n("Waiting for key unlock")
			Component.onCompleted: dialog.open()
		}
		Connections {
			target: Backend
			onKeyLock: dialog.open()
			onStateChanged: {splashlabel.text = Backend.fingerprint; passphrase.text = ""; pageStack.push(overview);}

		}
	}

	Kirigami.ScrollablePage {
		id: overview
		title: "credits"
		anchors.fill: parent
	}

	Controls.Dialog {
		id: dialog
		title: "unlock key"
		modal: true
		closePolicy: Controls.Popup.NoAutoClose
		standardButtons: Controls.Dialog.Ok | Controls.Dialog.Close
		anchors.centerIn: parent
		Kirigami.FormLayout {
			anchors.fill: parent
			Controls.TextField {
				id: passphrase
				echoMode: TextInput.Password
				Kirigami.FormData.label: "passphrase:"
			}

//				Controls.Button {
//					id: passphraseSend
//					text: "unlock"
//					onClicked: Backend.unlock(passphrase.text);
//				}

		}
		onAccepted: Backend.unlock(passphrase.text);
		onRejected: Qt.quit()
	}
}
