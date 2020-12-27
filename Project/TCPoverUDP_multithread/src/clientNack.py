import utils
import re
import socket
import queue


def clientN_ack(sock, q, done):
    print("Ack thread")

    last_sequence_ack = 1
    value_in_queue = 1

    # set time out
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
                q.put(last_sequence_ack)
        try:
            if done.get(block=False):
                break
        except queue.Empty:
            continue

    print("ACK finished")
    exit(0)

