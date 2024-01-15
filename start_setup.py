import os
import sys


participant_id = int(sys.argv[1])
num_latency_levels = 5


while(True):
    
    os.system('python3 questionnaires/demographic_survey.py ' + str(participant_id))
    
    for trial in range(num_latency_levels):
        print("trial: ", trial)

        os.system('./fittslaw-app/SpeedyMouseFittslaw {} {} {}'.format(participant_id, trial, 0))
        
        os.system('python3 questionnaires/nasa-tlx.py {} {} {}'.format(participant_id, trial, 0))

    os.system('python3 notification.py')

    break


