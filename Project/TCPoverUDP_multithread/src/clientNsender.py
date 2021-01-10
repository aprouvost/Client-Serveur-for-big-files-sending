import utils
import os
import time
import socket
# from Test_performances import Test_performances
import queue
import csv
import pandas
import threading
import queue
from clientNack import clientN_ack


def clientN_sender(sock, q, addr, default_RTT, win_size):
    print("sender thread")
    sock.settimeout(5)

    try:
        rcv_filename, _ = utils.recvFromClient(sock)
    except socket.timeout:
        pass
        print("No file was asked")
        utils.sendToClient(sock, addr, utils.FIN)
        exit(0)

    done = queue.Queue(maxsize=1)
    clientNack = threading.Thread(target=clientN_ack, args=(sock, q, done))
    clientNack.start()

    current_path = os.getcwd()
    filename = current_path + "/../../input/" + rcv_filename  # TODO : changer pour que ca soit que la current dir pour le rendu
    try:
        with open(filename, 'rb') as file:
            file = file.read()
            buf = [file[k * utils.BUFFER:(k + 1) * utils.BUFFER] for k in range((len(file) // utils.BUFFER) + 1)]
            file_size = os.stat(filename).st_size
            print("File found and opened successfully")
    except FileNotFoundError:
        print(" Hold on ! The file can't be found !")
        utils.sendToClient(sock, addr, utils.FIN)

    # Envoi de la data au client
    length_buf = len(buf)
    c_window = win_size
    last_ack = 1
    sequence = 1  # on commence a 1 sinon ca fait de la merde avec le client
    RTT = default_RTT
    time_start = time.perf_counter()
    # on envoie selon la congestion window
    tuples = []
    while True:
        # sequence = last_ack
        if c_window >= 1000:
            print("oops ca grandi trop")
            c_window = 500
        buf_window = buf[(sequence - 2):sequence + c_window]
        for data in buf_window:
            sequence_to_send = str(sequence).zfill(6).encode('utf-8')
            if data:
                sock.sendto(sequence_to_send + data, addr)
                sequence += 1

        # on regarde les ack
        try:
            last_ack = q.get(block=True, timeout=RTT)
            # print(last_ack)
            if last_ack == length_buf:
                print("Taille atteinte")
                break
            else:
                sequence = last_ack
                c_window += 1
                sequence_to_send = str(sequence).zfill(6).encode('utf-8')
                to_send = buf[(sequence - 2):sequence + 10]
                for data in to_send:
                    sock.sendto(sequence_to_send + data, addr)
                try:
                    last_ack = q.get(block=True, timeout=RTT)
                    # print(last_ack)
                    if last_ack == length_buf:
                        print("Taille atteinte")
                        break
                    else:
                        sequence = last_ack
                        c_window += 1
                except Exception as e:
                    raise e
            tuples.append((c_window, time.perf_counter()))
            if sequence > 670000:
                print(sequence, "/", length_buf)

        except Exception as e:
            print("rtt - Congestion detected, window size : " + str(c_window))
            tuples.append((c_window, time.perf_counter()))
            sequence = last_ack
            if c_window != 1:
                c_window = int(c_window / 2)
        # print(last_ack,"/",sequence)
        # print(c_window)

    time_end = time.perf_counter()
    print(" File sent with success ")

    time_run = time_end - time_start
    debit = file_size / time_run
    time.sleep(0.1)
    utils.sendToClient(sock, addr, utils.FIN)
    print("Debit : {:.4f} Mo/s".format(debit * 0.000001))

    # Test_performances(tuples)

    # Average window computing
    tuples = pandas.DataFrame(tuples)
    # check if csv output file already exists
    try:
        # if exists
        tuples.to_csv('average_window_output.csv', mode='a', header=False)
    except IOError:
        # if doesn't exist
        print("Creating output for average window file")
        tuples.to_csv('average_window_output.csv', mode='w', header=False)

    # Average debit computing
    deb = [debit * 0.000001]
    data = [(deb, default_RTT)]
    out = pandas.DataFrame(data)
    # check if csv output file already exists
    try:
        # if exists
        out.to_csv('average_debit_output.csv', mode='a', header=False)
    except IOError:
        # if doesn't exist
        print("Creating output for average debit file")
        out.to_csv('average_debit_output_2.5Mo_window-130.csv', mode='w', header=False)

    time.sleep(1)
    print("exit")
    done.put(True)
    clientNack.join()
    return
