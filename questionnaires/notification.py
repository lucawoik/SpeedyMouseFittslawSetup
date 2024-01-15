from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton, QMessageBox, QVBoxLayout, QWidget, QLabel, QSpacerItem, QSizePolicy
from PyQt5.QtGui import QFont
from PyQt5.QtCore import Qt

class MainWindow(QMainWindow):

    def __init__(self):
        super().__init__()

        self.label = QLabel("Bitte füllen Sie den Fragebogen aus und klicken Sie dann auf 'Weiter'.")
        self.button = QPushButton("Weiter", self)
        self.button.clicked.connect(self.on_button_clicked)

        font_big = QFont("Arial", 20)
        font_mid = QFont("Arial", 14)

        self.label.setFont(font_big)
        self.button.setFont(font_mid)

        self.layout = QVBoxLayout()
        self.layout.setContentsMargins(0, 400, 0, 400)
        self.layout.addWidget(self.label, 0, Qt.AlignCenter)
        self.layout.addWidget(self.button, 0, Qt.AlignCenter)

        self.central_widget = QWidget()
        self.central_widget.setLayout(self.layout)        

        self.setCentralWidget(self.central_widget)
        self.showFullScreen()
        

    def on_button_clicked(self):
        reply = QMessageBox.question(self, 'Fragebogen', "Sind Sie fertig mit dem Fragebogen?", QMessageBox.Yes | QMessageBox.No, QMessageBox.No)

        if reply == QMessageBox.Yes:
            QMessageBox.information(self, 'Fragebogen', "Vielen Dank für Ihre Teilnahme!")
            self.close()


app = QApplication([])
window = MainWindow()
window.show()

app.exec_()