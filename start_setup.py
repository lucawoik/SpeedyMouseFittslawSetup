import os
import sys
import random


participant_id = int(sys.argv[1])
latencies_template = [[0, 0, 0, 0]]


while(True):
    trial = 0
    latencies = latencies_template.copy()
    random.shuffle(latencies)
    
    os.system('python3 questionnaires/demographic_survey.py ' + str(participant_id))
    
    for l in latencies:

        os.system('./fittslaw-app/SpeedyMouseFittslaw /dev/input/event4 {} {} {} {} {} {}'.format(participant_id, trial, latencies[trial][0], latencies[trial][1], latencies[trial][2], latencies[trial][3]))
        # os.system('python3 notification.py')
        #os.system('python3 nasa-tlx.py {} {} {} {} {} {}'.format(participant_id, trial, latencies[trial][0], latencies[trial][1], latencies[trial][2], latencies[trial][3]))
        trial = trial + 1

    break


