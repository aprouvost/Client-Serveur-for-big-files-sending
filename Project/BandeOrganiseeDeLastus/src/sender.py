import threading
import random
import socket
import queue
import os
import time
from . import utils
from .ack import gestion_ack


def sender(sock_control, addr, used_ports, queue_ports, queue_ack_handshake, RTT, window):
    queue_done_theard_ack = queue.Queue()
    queue_received_ack = queue.Queue()

    # Tirage d'un port aleatoire
    print("DEBUG - sender : port list : ", used_ports)
    unique = False
    while not unique:
        clientN_port = random.randint(1000, 9999)
        if clientN_port not in used_ports:
            queue_ports.put(clientN_port)
            unique = True

    # Creation de la socket
    sock_data = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock_data.bind((utils.IP_ADDR, clientN_port))

    # Envoi du SYN-ACK
    print("Envoi du SYN-ACK", clientN_port)
    utils.sendToClient(sock_control, addr, utils.SYN_ACK + str(clientN_port))

    # Attendre le ACK
    # Si pas de ACK -> kill -> envoie de terminer au ACK
    # Sion on envoie le fichier
    try:
        queue_ack_handshake.get(timeout=10)
        print("ACK recu")
    except queue.Empty:
        # Si pas de ack du ahndshake pas la peine de continuer, on arrete les thread
        print("Timeout, handshake fail")
        return

    # On attend le nom du fichier
    sock_data.settimeout(10)
    # si pas de fichier demander on ferme la connexion et on fini les threads
    try:
        rcv_filename, _ = utils.recvFromClient(sock_data)
        print("DEBUG - sender : file asked : ", rcv_filename)
    except socket.timeout:
        utils.sendToClient(sock_data, addr, utils.FIN)
        print("Pas de fichier demander")
        return

    ack_manager = threading.Thread(target=gestion_ack,
                                   args=(sock_data, queue_received_ack, queue_done_theard_ack))
    ack_manager.start()

    # Lecture du fichier par morceau
    try:
        with open(rcv_filename, "rb") as file:
            file_size = os.stat(rcv_filename).st_size
            time_start = time.perf_counter()
            size_sent = 0
            sequence = 1
            last_ack = 0
            while size_sent < file_size:
                file_chunk = file.read(utils.FILE_BUFFERED)
                buf = [file_chunk[k * utils.BUFFER:(k + 1) * utils.BUFFER] for k in range((len(file_chunk) // utils.BUFFER) + 1)]
                buf_length = len(buf)

                # envoie de la data
                for count, data in enumerate(buf):
                    sock_data.sendto(str(sequence).zfill(6).encode('utf-8') + data, addr)
                    sequence += 1
                    if count % window == 0:
                        # print("DEBUG - sender : check des ack")
                        # on check les ack
                        try:
                            last_ack = queue_received_ack.get(block=True, timeout=RTT)
                            # print("DEBUG - sender : last ack : ", last_ack)
                            sequence = last_ack + 1
                        except queue.Empty:
                            sequence = last_ack
                            if window != 1:
                                window = int(window/2)
                if last_ack == buf_length:
                    break

                # print("DEBUG - sender : sequence : ", sequence)
                file.seek(sequence * utils.BUFFER, 0)
                size_sent = file.tell()

    except FileNotFoundError:
        print(" Hold on ! The file can't be found !")
        utils.sendToClient(sock_data, addr, utils.FIN)
        queue_done_theard_ack.put(True)
        ack_manager.join()
        return

    time_end = time.perf_counter()
    print(" File sent with success ")
    time_run = time_end - time_start
    debit = file_size / time_run
    time.sleep(0.1)
    utils.sendToClient(sock_data, addr, utils.FIN)
    print("Debit : {:.4f} Mo/s".format(debit * 0.000001))
    time.sleep(1)
    print("exit")

    # Envoie de terminer au ACK et kill

    ack_manager.join()
    return
