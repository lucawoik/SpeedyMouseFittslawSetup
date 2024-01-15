import os
import sys
import random


participant_id = int(sys.argv[1])
# latencies_template = [[0, 0, 0, 0]]
num_latency_levels = 2


while(True):
    # latencies = latencies_template.copy()
    # random.shuffle(latencies)
    # trial = 0

    
    os.system('python3 questionnaires/demographic_survey.py ' + str(participant_id))
    
    for trial in range(num_latency_levels):
        print("trial: ", trial)

        os.system('./fittslaw-app/SpeedyMouseFittslaw {} {} {} {} {} {}'.format(participant_id, trial, 0, 0, 0, 0))
        
        os.system('python3 questionnaires/nasa-tlx.py {} {} {} {} {} {}'.format(participant_id, trial, 0, 0, 0, 0))
        # trial += 1

    os.system('python3 notification.py')

    break


