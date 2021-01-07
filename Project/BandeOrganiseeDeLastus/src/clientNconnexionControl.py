from . import utils
import random
import threading
import queue
import socket
from .clientNsender import clientN_sender


def controlConnexion(sock, addr, ports, queue_ports, default_RTT, win_size):
    unique = False
    while not unique:
        clientN_port = random.randint(1000, 9999)
        if clientN_port not in ports:
            queue_ports.put(clientN_port)
            unique = True

    # creation des threads pour l'envoi de data et les ACK
    sock_data = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock_data.bind((utils.IP_ADDR, clientN_port))
    q = queue.Queue(maxsize=1)
    clientNsender = threading.Thread(target=clientN_sender, args=(sock_data, q, addr, default_RTT, win_size))

    clientNsender.start()


    # Fin du handshake
    # Envoi le SYN-ACK0000
    utils.sendToClient(sock, addr, utils.SYN_ACK + str(clientN_port))

    # attendre le ACK et tuer le thread
    # TODO : /!\ il est lu dans le main thread
    # data = ""
    # while data != utils.ACK:
    #     data, _ = utils.recvFromClient(sock)

    clientNsender.join()
    return
