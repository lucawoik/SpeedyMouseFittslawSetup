#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import csv
from PyQt5 import QtWidgets, uic
import os

UI_FILE = 'questionnaires/demographic_survey.ui'
LOG_DIRECTORY = 'log/'


class DemographicSurvey(QtWidgets.QDialog):

    #id_prob = -1

    def __init__(self):
        super().__init__()

        # Collect and storeArguments passed to the script
        self.id_prob = sys.argv[1]

        self.init_ui()

    def init_ui(self):
        self.ui = uic.loadUi(UI_FILE, self)
        self.showFullScreen()

        self.init_age_combobox()

        self.ui.button_box.accepted.connect(self.on_finished)

        self.show()

    def init_age_combobox(self):
        age_nums = ["Keine Angabe"]
        for age in list(range(18, 90)):
            age_nums.append(str(age))
        self.ui.comboBox_age.addItems(age_nums)

    def on_finished(self):
        self.write_to_csv()

    def get_selected_gender(self):
        if self.ui.radio_button_gender_male.isChecked():
            return 'male'
        elif self.ui.radio_button_gender_female.isChecked():
            return 'female'
        elif self.ui.radio_button_gender_diverse.isChecked():
            return 'diverse'
        elif self.ui.radio_button_gender_no_answer.isChecked():
            return 'no_answer'
        else:
            # TODO: Error handling no gender selected
            return 'no_answer'

    def get_selected_hand(self):
        if self.ui.radio_button_hand_left.isChecked():
            return "left"
        elif self.ui.radio_button_hand_right.isChecked():
            return 'right'
        else:
            # TODO: Handling nothing selected
            return 'no_answer'
    
    def get_selected_color_blindness(self):
        if self.ui.radio_button_color_blindness_no.isChecked():
            return False
        elif self.ui.radio_button_color_blindness_yes.isChecked():
            return True
        else:
            # TODO: Handling nothing selected
            return False

    def get_selected_answer_computer(self):
        if self.ui.radio_button_computer_never.isChecked():
            return 'never'
        elif self.ui.radio_button_computer_rarely.isChecked():
            return 'rarely'
        elif self.ui.radio_button_computer_sometimes.isChecked():
            return 'sometimes'
        elif self.ui.radio_button_computer_often.isChecked():
            return 'often'
        else:
            # TODO: Error handling nothing selected
            return 'no_answer'

    def get_selected_answer_input_device(self):
        if self.ui.radio_button_input_device_mouse.isChecked():
            return 'Mouse'
        elif self.ui.radio_button_input_device_touchpad.isChecked():
            return 'Touchpad'
        elif self.ui.radio_button_input_device_touchscreen.isChecked():
            return 'Touchscreen'
        else:
            # TODO: Error handling nothing selected
            return 'no_answer'

# todo: hier auch einfügen
    def write_to_csv(self):
        if not os.path.exists(LOG_DIRECTORY):
            os.makedirs(LOG_DIRECTORY)

        with open(LOG_DIRECTORY + 'demographic_participant_' + str(self.id_prob) + '.csv', mode='w') as file:
            writer = csv.writer(file, delimiter=',')

            writer.writerow(['participant_id',
                             'agreed',
                             'age',
                             'gender',
                             'hand',
                             'color_blindness',
                             'constraints',
                             'computer',
                             'input_device'])
            writer.writerow([self.id_prob,
                             self.ui.checkbox_agree.isChecked(),
                             self.ui.comboBox_age.currentText(),
                             self.get_selected_gender(),
                             self.get_selected_hand(),
                             self.get_selected_color_blindness(),
                             self.get_selected_answer_computer(),
                             self.get_selected_answer_input_device()])


def main():
    app = QtWidgets.QApplication(sys.argv)
    demographic_survey = DemographicSurvey()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()