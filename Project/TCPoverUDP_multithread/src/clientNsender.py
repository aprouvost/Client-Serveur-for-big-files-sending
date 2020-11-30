import utils
import os
import time
import _queue
import threading
from clientNack import clientN_ack


def clientN_sender(sock, queue, lock):
    utils.init()
    print("sender thread")
    rcv_filename, addr = utils.recvFromClient(sock)
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

    # Envoi de la data au client
    sequence = 1  # on commence a 1 sinon ca fait de la merde avec le client
    time_start = time.perf_counter()
    file_send = False
    length_buf = len(buf)
    # clientNack = threading.Thread(target=clientN_ack, args=(sock, queue, lock))
    # clientNack.start()
    while not file_send:

        try:
            last_ack = queue.get(block=False)
            print(last_ack)
            if last_ack == length_buf:
                print(" Taille atteinte")
                file_send = True
            else:
                for elem in range(0, last_ack):
                    buf[elem] = 0
        except _queue.Empty:
            for data in buf:
                if data == 0:
                    continue
                else:
                    sequence = str(sequence).zfill(6).encode('utf-8')
                    sock.sendto(sequence + data, addr)
                    sequence = buf.index(data) + 1

        # clientNack.join()
        # lock.acquire()
            # if not clientNack.is_alive():
            #     clientNack.start()
        # lock.release()

    print(" File sent with success ")

    # TODO : interuption en fonction de la sequence ack (avec une queue je pense)
    time_end = time.perf_counter()
    time_run = time_end - time_start
    debit = file_size / time_run
    time.sleep(0.1)
    utils.sendToClient(sock, addr, utils.FIN)
    print("Debit : {:.4f} Mo/s".format(debit * 0.000001))
