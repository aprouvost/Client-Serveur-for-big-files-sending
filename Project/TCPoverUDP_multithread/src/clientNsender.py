import utils
import os
import time
import socket
import threading
import queue
import psutil
from clientNack import clientN_ack


def read_in_chunks(f, size=128):
    while True:
        chunk = f.read(size)
        if not chunk:
            break
        yield chunk


def clientN_sender(sock, q, addr):
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
        # with open(filename, 'rb') as file:
        # file = file.read()
        with open(filename) as file:
            print("File found and opened successfully")
            file_size = os.stat(filename).st_size
            time_start = time.perf_counter()
            current = 0
            delta = 1200000000  # à fixer pour voir des chunk de combien on envoi
            while current < file_size:
                # check de la taille de la RAM utilisée à chaque itération
                # TODO : enlever pour le rendu !
                #print('RAM used : ')
                #print(psutil.virtual_memory().percent)

                # si on arrive sur la fin du fichier
                if current + delta > file_size:
                    delta = file_size - current
                chunk = file.seek(delta, current)
                current = file.tell()
                buf = [chunk[k * utils.BUFFER:(k + 1) * utils.BUFFER] for k in range((len(chunk) // utils.BUFFER) + 1)]

                # Envoi de la data au client
                length_buf = len(buf)
                c_window = 20
                last_ack = 1
                sequence = 1
                RTT = 0.06
                # time_start = time.perf_counter()
                # on envoie selon la congestion window
                while True:
                    buf_window = buf[(sequence - 1):sequence + c_window]
                    for data in buf_window:
                        sequence = buf.index(data) + 1
                        sequence = str(sequence).zfill(6).encode('utf-8')
                        sock.sendto(sequence + data, addr)

                    # on regarde les ack
                    try:
                        last_ack = q.get(block=True, timeout=RTT)
                        # print(last_ack)
                        if last_ack == length_buf:
                            print("Taille atteinte")
                            break
                        else:
                            sequence = last_ack + 1
                            c_window += 1
                    except:
                        print("Congestion detected, window size : " + str(c_window))
                        sequence = last_ack
                        if c_window != 1:
                            c_window = int(c_window / 2)
                        if c_window == 0:
                            c_window = 1

    except FileNotFoundError:
        print(" Hold on ! The file can't be found !")
        utils.sendToClient(sock, addr, utils.FIN)

    time_end = time.perf_counter()
    print(" File sent with success ")

    time_run = time_end - time_start
    debit = file_size / time_run
    utils.sendToClient(sock, addr, utils.FIN)
    print("Debit : {:.4f} Mo/s".format(debit * 0.000001))
    time.sleep(1)
    print("exit")
    done.put(True)
    clientNack.join()
    exit(0)
