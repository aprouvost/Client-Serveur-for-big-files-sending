#! /usr/bin/python3

import socket
from . import utils
import argparse
import threading
import queue
from .clientNconnexionControl import controlConnexion


def mainThread(public_port, queue_ports, ports_list, default_RTT, win_size):
    # Socket creation on public port

    print("Creating control socket")

    # récupère les ports utilisés

    sock_control = socket.socket(socket.AF_INET,
                                 socket.SOCK_DGRAM)  # UDP

    sock_control.bind((utils.IP_ADDR, public_port))

    while True:

        data, addr = utils.recvFromClient(sock_control)
        # debut du handshake
        if data == utils.SYN:
            thread = threading.Thread(target=controlConnexion,
                                      args=(sock_control, addr, ports_list, queue_ports, default_RTT, win_size))
            thread.start()
            item = queue_ports.get()
            ports_list.append(item)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Server public port", type=int)
    args = parser.parse_args()

    RTT = 0.06
    window = 100

    if args.port:
        ports_used = []
        q_ports = queue.Queue()
        ports = [args.port]
        mainThread(args.port, q_ports, ports, RTT, window)
