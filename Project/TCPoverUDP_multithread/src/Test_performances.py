import utils
import threading
# from clientNack import clientN_ack
# from clientNsender import clientN_sender

import datetime
import random
import matplotlib.pyplot as plt


def Test_performances(tuples):
    # tuples associates the window size to the last update's time
    x = []
    y = []
    for i in tuples:
        x.append(i[0])  # window size
        y.append(i[1])  # time

    # plot
    plt.plot(x, y)
    # beautify the x-labels
    plt.gcf().autofmt_xdate()

    plt.show()
    return
