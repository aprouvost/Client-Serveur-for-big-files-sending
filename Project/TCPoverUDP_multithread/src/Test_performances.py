import argparse
import queue

import utils
import threading
# from clientNack import clientN_ack
# from clientNsender import clientN_sender

import datetime
import random
import matplotlib.pyplot as plt
import csv
import pandas as pd

import main


def Test_performances(tuples):
    # tuples associates the window size to the last update's time
    x = []
    y = []
    for i in tuples:
        x.append(i[0])  # window size
        y.append(i[1])  # time

    # plot
    #plt.plot(y, x)
    # beautify the x-labels
    #plt.gcf().autofmt_xdate()

    #plt.show()
    return


# Fais une moyenne sur N iterations pour débir moyen
def get_average_deb(csv_debit, RTT):
    # Read the csv file:
    df = pd.read_csv(csv_debit)
    # computes mean value

    val = df[1].values.mean()
    return val


def computing_average():
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Server public port", type=int)
    args = parser.parse_args()
    default_RTT = 0.06
    iterations = int(input("Combien d'itérations voulez vous effectuer pour moyenner les valeurs ? : "))
    for i in iterations:
        if args.port:
            ports_used = []
            q_ports = queue.Queue()
            ports = [args.port]
            main.mainThread(args.port, q_ports, ports, default_RTT)
    deb_moyen = get_average_deb('average_debit_output.csv')
    print("Debit moyen : " + str(deb_moyen))
    return


# Faire varier RTT
def var_RTT():
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Server public port", type=int)
    args = parser.parse_args()

    val_RTT = 0.0
    while val_RTT < 1.5:
        if args.port:
            ports_used = []
            q_ports = queue.Queue()
            ports = [args.port]
            main.mainThread(args.port, q_ports, ports, val_RTT)
        val_RTT += 0.01

    deb_moyen = get_average_deb('average_debit_output.csv')
    print("Debit moyen : " + str(deb_moyen))
    return


# Faire varier window
def var_Window():
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Server public port", type=int)
    args = parser.parse_args()
    default_RTT = 0.06
    win_start = int(input("A quelle taille souhaitez vous démarer le test de la fenetre ? : "))
    default_RTT = int(input("quelle valeur de RTT ? : "))
    win_size = win_start
    while win_size < win_end:
        if args.port:
            ports_used = []
            q_ports = queue.Queue()
            ports = [args.port]
            main.mainThread(args.port, q_ports, ports, default_RTT, win_size)
        #win_size += 1
    deb_moyen = get_average_deb('average_debit_output_selon_RTT.csv', RTT)
    print("Debit moyen : " + str(deb_moyen))
    return

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Server public port", type=int)
    args = parser.parse_args()
    default_RTT = 0.06
    # iterations = int(input("Combien d'itérations voulez vous effectuer pour moyenner les valeurs ? : "))
    # win_size = int(input("Quelle taille de window au démarrage ? : "))
    win_size = 20
    # default_RTT = float(input("Quelle taille de RTT ? : "))
    # if win_size == 0:
    #     win_size = 1
    # for i in range(iterations):
    if args.port:
        ports_used = []
        q_ports = queue.Queue()
        ports = [args.port]
        main.mainThread(args.port, q_ports, ports, default_RTT, win_size)
    deb_moyen = get_average_deb('average_debit_output.csv')
    print("Debit moyen : " + str(deb_moyen))
