import os
import subprocess
import sys
import random
import time

participant_id = int(sys.argv[1])

LATENCY_CLICK_LOW = 50
LATENCY_CLICK_HIGH = 150

LATENCY_MOVE_LOW = 50
LATENCY_MOVE_HIGH = 150

VARIANCE_CLICK_LOW = 0
VARIANCE_CLICK_HIGH = 50

VARIANCE_MOVE_LOW = 0
VARIANCE_MOVE_HIGH = 50

#leave out low variances for now
#[LATENCY_CLICK_LOW - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_LOW / 2), 0, 0],
#[LATENCY_CLICK_HIGH - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_LOW / 2), 0, 0],
#[LATENCY_CLICK_LOW - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_LOW - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_LOW + (VARIANCE_MOVE_LOW / 2)],
#[LATENCY_CLICK_HIGH - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_LOW / 2)],

latencies_template = []

# for i in range(0, 30):
#     latencies_template.append([150, 150, 150, 150])

""" 
latencies_template = [[0, 0, 0, 0]] """
latencies_template = [[LATENCY_CLICK_HIGH - (VARIANCE_CLICK_HIGH / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_HIGH / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_HIGH / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_HIGH / 2)]]

# Conditions: la=latency va=variancy lo=low hi= high 
# A(la/lo, va/lo), B(la/lo, la/ha), C(la/hi, va/lo), D(la/hi, va/,hi) 
""" latencies_template = [
    [LATENCY_CLICK_LOW - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_LOW - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_LOW + (VARIANCE_MOVE_LOW / 2)],
    [LATENCY_CLICK_LOW - (VARIANCE_CLICK_HIGH / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_HIGH/ 2), LATENCY_MOVE_LOW - (VARIANCE_MOVE_HIGH / 2), LATENCY_MOVE_LOW + (VARIANCE_MOVE_HIGH / 2)],
    [LATENCY_CLICK_HIGH - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_LOW / 2)],
    [LATENCY_CLICK_HIGH - (VARIANCE_CLICK_HIGH / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_HIGH / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_HIGH / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_HIGH / 2)]
    ] """

# B,C,A,D
""" latencies_template = [
    [LATENCY_CLICK_LOW - (VARIANCE_CLICK_HIGH / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_HIGH/ 2), LATENCY_MOVE_LOW - (VARIANCE_MOVE_HIGH / 2), LATENCY_MOVE_LOW + (VARIANCE_MOVE_HIGH / 2)],
    [LATENCY_CLICK_HIGH - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_LOW / 2)],
    [LATENCY_CLICK_LOW - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_LOW - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_LOW + (VARIANCE_MOVE_LOW / 2)],
    [LATENCY_CLICK_HIGH - (VARIANCE_CLICK_HIGH / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_HIGH / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_HIGH / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_HIGH / 2)]
    ] """

# C,D,B,A
""" latencies_template = [
    [LATENCY_CLICK_HIGH - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_LOW / 2)],
    [LATENCY_CLICK_HIGH - (VARIANCE_CLICK_HIGH / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_HIGH / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_HIGH / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_HIGH / 2)],
    [LATENCY_CLICK_LOW - (VARIANCE_CLICK_HIGH / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_HIGH/ 2), LATENCY_MOVE_LOW - (VARIANCE_MOVE_HIGH / 2), LATENCY_MOVE_LOW + (VARIANCE_MOVE_HIGH / 2)],
    [LATENCY_CLICK_LOW - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_LOW - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_LOW + (VARIANCE_MOVE_LOW / 2)]
    ] """

# D,A,C,B
""" latencies_template = [
    [LATENCY_CLICK_HIGH - (VARIANCE_CLICK_HIGH / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_HIGH / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_HIGH / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_HIGH / 2)],
    [LATENCY_CLICK_LOW - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_LOW - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_LOW + (VARIANCE_MOVE_LOW / 2)],
    [LATENCY_CLICK_HIGH - (VARIANCE_CLICK_LOW / 2), LATENCY_CLICK_HIGH + (VARIANCE_CLICK_LOW / 2), LATENCY_MOVE_HIGH - (VARIANCE_MOVE_LOW / 2), LATENCY_MOVE_HIGH + (VARIANCE_MOVE_LOW / 2)],
    [LATENCY_CLICK_LOW - (VARIANCE_CLICK_HIGH / 2), LATENCY_CLICK_LOW + (VARIANCE_CLICK_HIGH/ 2), LATENCY_MOVE_LOW - (VARIANCE_MOVE_HIGH / 2), LATENCY_MOVE_LOW + (VARIANCE_MOVE_HIGH / 2)] 
    ] """


while(True):
    trial = 0
    latencies = latencies_template.copy()
    random.shuffle(latencies)
    #latencies.insert(0, [0, 0, 0, 0])

    #print(latencies)
    
    #print(l for l in latencies[trial])
    #os.system('echo "{} {} {} {}"'.format(latencies[trial][0], latencies[trial][1], latencies[trial][2], latencies[trial][3])) 
    
    os.system('echo "0 0 0 0" > /tmp/delaydaemon')
    #os.system('python3 demographic_survey.py ' + str(participant_id))
    

    # delaydeamon starten: sudo ./DelayDaemon /dev/input/event4 0 0 0 0 /tmp/delaydaemon 
    for l in latencies:
        print('next trial: {} {} {} {}'.format(latencies[trial][0], latencies[trial][1], latencies[trial][2], latencies[trial][3]))
        os.system('echo "{} {} {} {}" > /tmp/delaydaemon'.format(int(latencies[trial][0]), int(latencies[trial][1]), int(latencies[trial][2]), int(latencies[trial][3]))) 
        # os.system('echo "0 1500 0 1500" > /tmp/delaydaemon') 
        os.system('./moving/FittsiM {} {} {} {} {} {}'.format(participant_id, trial, latencies[trial][0], latencies[trial][1], latencies[trial][2], latencies[trial][3]))
        os.system('echo "0 0 0 0" > /tmp/delaydaemon')
        #os.system('python3 nasa-tlx.py {} {} {} {} {} {}'.format(participant_id, trial, latencies[trial][0], latencies[trial][1], latencies[trial][2], latencies[trial][3]))
        trial = trial + 1

    break
    participant_id = participant_id + 1

    #subprocess.call("python3", "demographic_survey.py", participant_id)
    #os.system('python3 demographic_survey.py ' + participant_id)


