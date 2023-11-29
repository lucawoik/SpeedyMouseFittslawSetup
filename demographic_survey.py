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
        for age in list(range(10, 90)):
            age_nums.append(str(age))
        #print(age_nums)
        self.ui.comboBox_age.addItems(age_nums)

    def on_finished(self):
        #print("Age:", self.ui.comboBox_age.currentText())
        #print("Gender:", self.get_selected_gender())
        #print("Constraints:", self.get_selected_constraints())
        #print("Action games:", self.get_selected_answer_action_games())
        #print("Strategy games:", self.get_selected_answer_strategy_games())
        #print("Agree:", self.ui.checkbox_agree.isChecked())

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

    def get_selected_constraints(self):
        if self.ui.radio_button_constraints_no.isChecked():
            return False
        elif self.ui.radio_button_constraints_yes.isChecked():
            return True
        else:
            # TODO: Handling nothing selected
            return False
    
    def get_selected_color_blindness(self):
        if self.ui.radio_button_color_blindness_no.isChecked():
            return False
        elif self.ui.radio_button_color_blindness_yes.isChecked():
            return True
        else:
            # TODO: Handling nothing selected
            return False

    def get_selected_answer_action_games(self):
        if self.ui.radio_button_action_games_never.isChecked():
            return 'never'
        elif self.ui.radio_button_action_games_rarely.isChecked():
            return 'rarely'
        elif self.ui.radio_button_action_games_sometimes.isChecked():
            return 'sometimes'
        elif self.ui.radio_button_action_games_often.isChecked():
            return 'often'
        else:
            # TODO: Error handling nothing selected
            return 'no_answer'

    def get_selected_answer_strategy_games(self):
        if self.ui.radio_button_strategy_games_never.isChecked():
            return 'never'
        elif self.ui.radio_button_strategy_games_rarely.isChecked():
            return 'rarely'
        elif self.ui.radio_button_strategy_games_sometimes.isChecked():
            return 'sometimes'
        elif self.ui.radio_button_strategy_games_often.isChecked():
            return 'often'
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
                             'action_games',
                             'strategy_games'])
            writer.writerow([self.id_prob,
                             self.ui.checkbox_agree.isChecked(),
                             self.ui.comboBox_age.currentText(),
                             self.get_selected_gender(),
                             self.get_selected_hand(),
                             self.get_selected_color_blindness(),
                             self.get_selected_constraints(),
                             self.get_selected_answer_action_games(),
                             self.get_selected_answer_strategy_games()])


def main():
    app = QtWidgets.QApplication(sys.argv)
    demographicSurvey = DemographicSurvey()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
