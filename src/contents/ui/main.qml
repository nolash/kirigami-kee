import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.defalsify.kde.credittracker 1.0

Kirigami.ApplicationWindow {
	id: root
	title: i18nc("@title:window", "Bilateral credit tracker")

	pageStack.initialPage: Kirigami.Page  {
		Controls.Dialog {
			id: dialog
			title: "unlock key"
			modal: true
			standardButtons: Controls.Dialog.Ok
			Kirigami.FormLayout {
				anchors.fill: parent
				Controls.TextField {
					id: passphrase
					Kirigami.FormData.label: "passphrase:"
				}

//				Controls.Button {
//					id: passphraseSend
//					text: "unlock"
//					onClicked: Backend.unlock(passphrase.text);
//				}

			}
			onAccepted: Backend.unlock(passphrase.text);
		}
		Controls.Label {
			id: ddd
			anchors.centerIn: parent
			text: i18n("Not logged in")
			Connections {
				target: Backend
				onStateChanged: ddd.text = Backend.fingerprint
			}
			Component.onCompleted: dialog.open()
		}
		Connections {
			target: Backend
			onKeyLock: dialog.open()
		}
	}
}
