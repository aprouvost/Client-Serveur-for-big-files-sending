from . import utils
import os
import time
import socket
import threading
import queue
from .clientNack import clientN_ack


def clientN_sender(sock, q, addr, default_RTT, win_size):
    sock.settimeout(15)

    try:
        rcv_filename, _ = utils.recvFromClient(sock)
    except socket.timeout:
        pass
        print("Timeout, no file was requested")
        utils.sendToClient(sock, addr, utils.FIN)
        exit(0)

    done = queue.Queue(maxsize=1)
    clientNack = threading.Thread(target=clientN_ack, args=(sock, q, done))
    clientNack.start()
    try:
        with open(rcv_filename, 'rb') as file:
            file = file.read()
            buf = [file[k * utils.BUFFER:(k + 1) * utils.BUFFER] for k in range((len(file) // utils.BUFFER) + 1)]
            file_size = os.stat(rcv_filename).st_size
            #print("File found and opened successfully")
    except FileNotFoundError:
        print(" Hold on ! The file can't be found !")
        utils.sendToClient(sock, addr, utils.FIN)
        return

    length_buf = len(buf)
    c_window = win_size
    last_ack = 1
    sequence = 1
    RTT = default_RTT
    time_start = time.perf_counter()

    while True:
        buf_window = buf[(sequence - 1):sequence + c_window]
        for data in buf_window:
            sequence = buf.index(data) + 1
            sequence = str(sequence).zfill(6).encode('utf-8')
            sock.sendto(sequence + data, addr)

        try:
            last_ack = q.get(block=True, timeout=RTT)
            if last_ack == length_buf:
                #print("Taille atteinte")
                break
            elif int(sequence) - last_ack > c_window:
                #print("Congestion detected, window size : " + str(c_window))
                sequence = last_ack + 1
                if c_window != 1:
                    c_window = int(c_window / 2)
            else:
                sequence = last_ack + 1
                c_window += 1

        except Exception as e:
            #print("Congestion detected, window size : " + str(c_window))
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

    #time.sleep(1)
    done.put(True)
    clientNack.join()
    return
