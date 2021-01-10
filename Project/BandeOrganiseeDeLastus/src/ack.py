import threading
import re
import socket
import queue
from . import utils


def gestion_ack(sock, queue_received_ack, queue_done):

    last_ack = 1
    value_in_queue = 1
    sock.settimeout(0.0000001)
    # Recoit tout le temps les ACK
    while True:
        try:
            data, _ = utils.recvFromClient(sock)
            if re.match(r"ACK([0-9]){6}", data):
                current_sequence_ack = int(data[3:])
                last_ack = current_sequence_ack if current_sequence_ack > last_ack else last_ack
                # print("DEBUG - ack : ack recu : ", last_ack)
        except socket.timeout:
            # Met dans la queue le dernier ACK recu
            if last_ack >= value_in_queue:
                try:
                    queue_received_ack.get(block=False)
                except queue.Empty:
                    pass
                value_in_queue = last_ack
                queue_received_ack.put(last_ack)
            pass

        try:
            # interruption quand le sender dit que c'est fini
            if queue_done.get(block=False):
                break
        except queue.Empty:
            pass

    return
