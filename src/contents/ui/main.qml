import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import org.kde.kirigami 2.20 as Kirigami
import org.defalsify.kde.credittracker 1.0


Kirigami.ApplicationWindow {
	id: root
	title: i18nc("@title:window", "Bilateral credit tracker")

	controlsVisible: true

	globalDrawer: Kirigami.GlobalDrawer{
		isMenu: true
		actions: [
			Kirigami.Action {
				text: i18n("Lock")
				icon.name: "lock"
				onTriggered: Backend.lock()
			},
			Kirigami.Action {
				text: i18n("Import from QR")
				icon.name: "view-barcode-qr"
				onTriggered: function () {
					pageStack.push(qrscanner);
					camera.start();
					shutter.start();
				}
			},
			Kirigami.Action {
				text: i18n("Quit")
				icon.name: "gtk-quit"
				shortcut: StandardKey.Quit
				onTriggered: Qt.quit()
			}

		]
	}


	pageStack.initialPage: Kirigami.ScrollablePage {
		id: overview
		Controls.Label {
			id: splashlabel
			anchors.fill: parent
			text: i18n("Waiting for key unlock")
			Component.onCompleted: dialog.open()
		}
		Connections {
			target: Backend
			function onKeyLock() {
				dialog.open();
			}
			function onStateChanged() {
				if (Backend.fingerprint != "") {
					splashlabel.text = Backend.fingerprint;
					passphrase.text = "";
					creditList.visible = true
				}
			}
		}
		Kirigami.CardsListView {
			model: creditModel
			id: creditList
			delegate: creditListDelegate
		}
		header: Kirigami.NavigationTabBar {
			actions: [
				Kirigami.Action {
					text: "back"
					visible: pageStack.depth > 1
					onTriggered: {
						console.log("back");
						pageStack.pop();
					}
				}
			]
		}
	}

	Kirigami.ScrollablePage {
		id: creditItem
		visible: false
		Controls.Label {
			id: creditItemTitle
			text: ""
		}
		Connections {
			target: pageStack
			function onPagePushed() {
				console.log("page pushed");
			}
		}	
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
		}
		onAccepted: Backend.unlock(passphrase.text);
		onRejected: Qt.quit()
	}

	Component {
		id: creditListDelegate
		Kirigami.AbstractCard {
			contentItem: Item {
				implicitWidth: creditListDelegateLayout.implicitWidth
				implicitHeight: creditListDelegateLayout.implicitHeight
				GridLayout {
					id: creditListDelegateLayout
					anchors {
						left: parent.left
						top: parent.top
						right: parent.right
					}
					rowSpacing: Kirigami.Units.largeSpacing
					columnSpacing: Kirigami.Units.largeSpacing
					columns: overview.wideScreen ? 4 : 2

					Kirigami.Heading {
						Layout.fillHeight: true
						level: 1
						text: "head"
					}


					ColumnLayout {
						Kirigami.Heading {
							Layout.fillWidth: true
							level: 2
							text: name
						}
						Kirigami.Separator {
							Layout.fillWidth: true
							visible: name.length > 0

						}
						Controls.Label {
							Layout.fillWidth: true
							text: description
							wrapMode: Text.WordWrap
							visible: description.length > 0
						}
						Controls.Button {
							text: i18n("View")
							Layout.columnSpan: 2
							onClicked: {
								console.log("foo");
								creditItemTitle.text = name
								if (!creditItem.visible) {
									pageStack.push(creditItem);
								}
							}
						}
					}
				}
			}
		}
	}

	Kirigami.Page {
		id: qrscanner
		visible: false
		Column {
			Controls.ComboBox {
				id: cameraSelect
				model: QtMultimedia.availableCameras
				delegate: Controls.ItemDelegate {
					text: modelData.displayName
				}
				editable: false
				displayText: QtMultimedia.availableCameras[cameraSelect.currentIndex].displayName;
				onActivated: function(i) {
					console.log(QtMultimedia.availableCameras[i].deviceId);
					camera.deviceId = QtMultimedia.availableCameras[i].deviceId;	
				}
			}

			VideoOutput {
				id: viewfinder
				source: camera
				x: 100
				y: 100
			}

			Camera {
				id: camera
				position: Camera.FrontFace
				captureMode: Camera.CaptureStillImage
				videoRecorder {
					resolution: "640x480"
					frameRate: 30
				}
				imageCapture {
					id: camcap
					onImageSaved: function (reqid, img) {
						Backend.image_catch(img);
					}
				}
			}
		}
		actions.contextualActions: [
			Kirigami.Action {
				icon.name: "gtk-close"
				text: "close"
				onTriggered: function() {
					camera.stop();
					shutter.stop();
					pageStack.pop();
				}
			}
		]
	}

	Item {
		Timer {
			id: shutter
			interval: 1000 
			repeat: true
			running: false
			triggeredOnStart: true
			onTriggered: function() {
				if (camera.imageCapture.ready) {
					console.log("shot");
					camera.imageCapture.capture();
				}
			}
		}
		Connections {
			target: Backend
			function onCommandProcessStart() {
				if (!shutter.running) {
					return;
				}
				shutter.stop();
				pageStack.pop();
				pageStack.push(commandConfirm);
			}
		}
	}

	Kirigami.Page {
		id: commandConfirm
		visible: false
		ColumnLayout {
			Controls.Label {
				id: commandConfirmPrepareState
				text: "processing..."
			}
			Text {
				id: commandPreview
				text: ""
				textFormat: Text.MarkdownText
			}
			RowLayout {
				Controls.Button {
					id: commandConfirmActionOk
					visible: false
					text: i18n("yay")
					onClicked: function() {
						pageStack.pop();
						commandPreview.text = "";
						commandConfirmActionOk.visible = false;
						Backend.acceptCurrentCommand();
					}
				}
				Controls.Button {
					id: commandConfirmActionAbort
					text: i18n("cancel")
					onClicked: function() {
						pageStack.pop();
						commandPreview.text = "";
						commandConfirmActionOk.visible = false;
					}

				}
			}
		}
		Connections {
			target: Backend
			function onCommandProcessView(r, s) {
				camera.stop();
				commandConfirmPrepareState.text = "parsed";
				commandPreview.text = s;
				commandConfirmActionOk.visible = true;
			}
		}
	}	
}
