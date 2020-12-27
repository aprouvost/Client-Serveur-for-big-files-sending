import utils
import random
import threading
import queue
import socket
from clientNack import clientN_ack
from clientNsender import clientN_sender
from Test_performances import Test_performances


def controlConnexion(sock, addr, ports, queue_ports):
    unique = False
    while not unique:
        clientN_port = random.randint(1000, 9999)
        if clientN_port not in ports:
            queue_ports.put(clientN_port)
            unique = True

    print("entering in")

    # creation des threads pour l'envoi de data et les ACK
    sock_data = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock_data.bind((utils.IP_ADDR, clientN_port))
    q = queue.Queue(maxsize=1)
    done = queue.Queue(maxsize=1)
    clientNsender = threading.Thread(target=clientN_sender, args=(sock_data, q, addr))
    clientNack = threading.Thread(target=clientN_ack, args=(sock_data, q, done))

    clientNsender.start()
    clientNack.start()

    # Fin du handshake
    #  envoyer le SYN-ACK0000
    utils.sendToClient(sock, addr, utils.SYN_ACK + str(clientN_port))

    # attendre le ACK et tuer le thread
    # TODO : /!\ il est lu dans le main thread
    # data = ""
    # while data != utils.ACK:
    #     data, _ = utils.recvFromClient(sock)

    clientNsender.join()
    if not clientNsender.is_alive():
        done.put(True)

    clientNack.join()
    return

