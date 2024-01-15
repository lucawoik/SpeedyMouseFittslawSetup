from PyQt5.QtWidgets import QApplication, QMainWindow, QMessageBox

class MainWindow(QMainWindow):

    def __init__(self):
        super().__init__()
        QMessageBox.information(self, 'Fragebogen', "Vielen Dank für Ihre Teilnahme!")
        self.close()

app = QApplication([])
window = MainWindow()
# window.show()

app.exec_()