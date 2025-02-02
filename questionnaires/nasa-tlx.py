#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import csv
from PyQt5 import QtWidgets, uic
import os

UI_FILE = 'questionnaires/nasa-tlx.ui'
LOG_DIRECTORY = 'log/'


class NasaTLX(QtWidgets.QWizard):

    def __init__(self):
        super().__init__()

        # Collect and storeArguments passed to the script
        self.id_prob = sys.argv[1]
        self.id_trial = sys.argv[2]
        self.latency = sys.argv[3]

        self.init_ui()

    def init_ui(self):
        self.ui = uic.loadUi(UI_FILE, self)

        # Stylesheet for custom slider design
        self.ui.setStyleSheet("""
                .QWizard {
                    background-color: #ffffff;
                }
                .QSlider {
                    min-height: 68px;
                    max-height: 68px;
                    padding-left: 10px;
                    padding-right: 10px;
                }
                
                .QSlider::groove:horizontal {
                    height: 0px;
                    background: #000000;
                }
                
                .QSlider::handle:horizontal {
                    background: #22B14C;
                    border: 5px solid #B5E61D;
                    width: 10px;
                    height: 50px;
                    margin: -10px -10px;
                }
                """)

        self.showFullScreen()

        # Only show a back button on page 2 (ID 1)
        if QtWidgets.QWizard.currentId(self) != 1:
            self.button(QtWidgets.QWizard.BackButton).hide()
        else:
            self.button(QtWidgets.QWizard.BackButton).show()

        self.button(QtWidgets.QWizard.FinishButton).clicked.connect(self.on_finished)

        self.show()

    def on_finished(self):
        self.write_to_csv()

    def write_to_csv(self):
        if not os.path.exists(LOG_DIRECTORY):
            os.makedirs(LOG_DIRECTORY)

        file_exists = os.path.exists(LOG_DIRECTORY + 'participant_' + str(self.id_prob) + '_tlx.csv')

        with open(LOG_DIRECTORY + 'participant_' + str(self.id_prob) + '_tlx.csv', mode='a+') as file:
            writer = csv.writer(file, delimiter=',')

            if not file_exists:
                writer.writerow(['participant_id',
                                 'trial',
                                 'latency',
                                 'mental_demand',
                                 'physical_demand',
                                 'temporal_demand',
                                 'performance',
                                 'effort',
                                 'frustration'])

            writer.writerow([self.id_prob,
                             self.id_trial,
                             self.latency,
                             self.ui.slider_mental.value() / 5,  # Divide by 5 to get from 0-100 to 0-20
                             self.ui.slider_physical.value() / 5,
                             self.ui.slider_temporal.value() / 5,
                             self.ui.slider_performance.value() / 5,
                             self.ui.slider_effort.value() / 5,
                             self.ui.slider_frustration.value() / 5])


def main():
    app = QtWidgets.QApplication(sys.argv)
    nasa_tlx = NasaTLX()
    # nasa_tlx.write_to_csv()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()