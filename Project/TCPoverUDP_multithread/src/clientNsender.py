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
    last_ack = 1
    sequence = 1
    try:
        with open(filename, 'rb') as file:
            print("File found and opened successfully")
            file_size = os.stat(filename).st_size
            time_start = time.perf_counter()
            current = 0
            delta = 120000
            iteration = 0
            # decal = iteration*delta
            decal = 0

            while current < file_size:

                file.seek(sequence*utils.BUFFER, 0)
                chunk = file.read(delta)

                buf = [chunk[k * utils.BUFFER:(k + 1) * utils.BUFFER] for k in range((len(chunk) // utils.BUFFER) + 1)]

                # Envoi de la data au client
                length_buf = len(buf)
                c_window = win_size
                RTT = default_RTT

                # on envoie selon la congestion window
                while True:
                    buf_window = buf[(decal - sequence - 1):(decal - sequence) + c_window]
                    for data in buf_window:
                        sequence = decal + buf.index(data) + 1
                        sequence_to_send = str(sequence).zfill(6).encode('utf-8')
                        sock.sendto(sequence_to_send + data, addr)

                    # on regarde les ack
                    try:
                        last_ack = q.get(block=True, timeout=RTT)
                        # print(last_ack)
                        if last_ack == length_buf:
                            print("Taille buffer atteinte")
                            break
                        elif int(sequence) - last_ack > c_window:
                            print("Congestion detected, window size : " + str(c_window))
                            sequence = last_ack + 1
                            if c_window != 1:
                                c_window = int(c_window / 2)
                        else:
                            sequence = last_ack + 1
                            c_window += 1
                        tuples.append((c_window, time.perf_counter()))

        except Exception as e:
            print("Congestion detected, window size : " + str(c_window))
            tuples.append((c_window, time.perf_counter()))
            sequence = last_ack + 1
            if c_window != 1:
                c_window = int(c_window / 2)

    time_end = time.perf_counter()
    print(" File sent with success ")

    time_run = time_end - time_start
    debit = file_size / time_run
    time.sleep(0.1)
    utils.sendToClient(sock, addr, utils.FIN)
    print("Debit : {:.4f} Mo/s".format(debit * 0.000001))

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
