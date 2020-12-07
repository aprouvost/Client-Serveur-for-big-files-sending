import utils
import os
import time
import _queue
import threading
from clientNack import clientN_ack
import socket


def clientN_sender(sock, queue, lock):
    utils.init()
    print("sender thread")
    try:
        rcv_filename, addr = utils.recvFromClient(sock)
    except socket.timeout:
        print("No file demanding")
        exit(0)
    current_path = os.getcwd()
    filename = current_path + "\\..\\..\\input\\" + rcv_filename  # TODO : changer pour que ca soit que la current dir pour le rendu
    try:
        with open(filename, 'rb') as file:
            file = file.read()
            buf = [file[k * utils.BUFFER:(k + 1) * utils.BUFFER] for k in range((len(file) // utils.BUFFER) + 1)]
            file_size = os.stat(filename).st_size
            print("File found and opened successfully")
    except FileNotFoundError:
        print(" Hold on ! The file can't be found !")
        # TODO : Est ce qu'on envoie au client qu'on a pas trouver le fichier avec un FIN ?
        utils.sendToClient(sock, addr, utils.FIN)

    # Envoi de la data au client
    length_buf = len(buf)
    c_window = 10
    last_ack = -1
    sequence = 1  # on commence a 1 sinon ca fait de la merde avec le client
    RTT = 0.06
    time_start = time.perf_counter()
    # on envoie selon la congestion window
    while True:
        buf_window = buf[(sequence - 1):(sequence - 1) + c_window]
        for data in buf_window:
            sequence = buf.index(data) + 1
            sequence = str(sequence).zfill(6).encode('utf-8')
            sock.sendto(sequence + data, addr)

        # on regarde les ack
        try:
            last_ack = queue.get(block=True, timeout=RTT)
            if last_ack == length_buf:
                print("Taille atteinte")
                break
            else:
                sequence = last_ack
                c_window += 1
        except:
            print("Congestion detected")
            sequence = last_ack
            c_window = int(c_window / 2)

    time_end = time.perf_counter()
    print(" File sent with success ")

    time_run = time_end - time_start
    debit = file_size / time_run
    time.sleep(0.1)
    utils.sendToClient(sock, addr, utils.FIN)
    print("Debit : {:.4f} Mo/s".format(debit * 0.000001))
