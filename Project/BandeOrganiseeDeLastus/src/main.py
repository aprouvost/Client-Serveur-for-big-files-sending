import threading
import socket
import queue
from . import utils
from .sender import sender


def mainThread(public_port, RTT, window):
    ports_used = [public_port, ]
    clients = []  # (client_addr, queue)
    queue_ports = queue.Queue()

    sock_control = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP

    # Bind sur le port public
    sock_control.bind((utils.IP_ADDR, public_port))
    print("Listening")

    while True:
        # Ecoute tout le temps et creer un thread aux SYN
        print("DEBUG - main : port list : ", ports_used)
        data, addr = utils.recvFromClient(sock_control)
        # Debut du handshake
        if data == utils.SYN:
            print("SYN recu")
            queue_ack_handshake = queue.Queue()
            client = threading.Thread(target=sender,
                                      args=(sock_control, addr, ports_used, queue_ports, queue_ack_handshake, RTT, window))
            clients.append((addr, queue_ack_handshake))
            client.start()

            try:
                new_port = queue_ports.get(timeout=2)
                ports_used.append(new_port)
            except queue.Empty:
                print("Une erreur est survenue lors de la creation du client, pas de port attribue")
        if data == utils.ACK:
            for cl in clients:
                if addr == cl[0]:
                    cl[1].put(utils.ACK)
