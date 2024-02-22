import os
import sys


participant_id = int(sys.argv[1])
event_handler = sys.argv[2]
order_levels = sys.argv[3]
num_latency_levels = 7
trial = 0


while(True):

    os.system('python3 questionnaires/demographic_survey.py ' + str(participant_id))
    
    for level in order_levels:
        os.system('./fittslaw-app/SpeedyMouseFittslaw {} {} {} {} {} {}'.format(event_handler, level, participant_id, trial, 0, 0))
        os.system('python3 questionnaires/nasa-tlx.py {} {} {}'.format(participant_id, trial, 0))
        trial += 1
    os.system('python3 notification.py')
    break