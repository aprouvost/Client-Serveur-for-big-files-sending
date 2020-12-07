import utils
import re
import socket


def clientN_ack(sock, queue, lock):
    utils.init()
    print("Ack thread")

    last_sequence_ack = 1
    value_in_queue = 1

    #set time out
    sock.settimeout(0.0000005)

    while True:
        try:
            data, _ = utils.recvFromClient(sock)
            # print(data)
            if re.match(r"ACK([0-9]){6}", data):
                current_sequence_ack = int(data[3:])
                last_sequence_ack = current_sequence_ack if current_sequence_ack > last_sequence_ack else last_sequence_ack
                # print(f"Received ACK : {last_sequence_ack}")
        except socket.timeout:
            # envoyer au thread send la seq ack via une queue
            if last_sequence_ack != value_in_queue:
                value_in_queue = last_sequence_ack
                queue.put(last_sequence_ack)

        #Met à jour la variable globale pour récupérer le plus grand ACK

        # lock.acquire()
        # utils.last_ack = last_sequence_ack
        # lock.release()
    # exit(0)
