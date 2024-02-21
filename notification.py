from PyQt5.QtWidgets import QApplication, QMainWindow, QMessageBox
import sys

class MainWindow(QMainWindow):

    def __init__(self):
        super().__init__()
        reply = QMessageBox.information(self, 'Fragebogen', "Vielen Dank für Ihre Teilnahme!")
        if reply == QMessageBox.Ok:
            self.close()
            sys.exit()

app = QApplication([])
window = MainWindow()
# window.show()

app.exec_()