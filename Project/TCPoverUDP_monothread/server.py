import socket
import common as Common
# import export as export
from time import perf_counter

import os
import time
import argparse


# import emoji


def recvFromClient(sock):
    data, addr = sock.recvfrom(Common.BUFFER)
    data = data.decode("utf-8")
    if data.endswith('\x00'):
        data = data[:-1]
    return data.encode("utf-8"), addr


def congestion(buff, sock_data, addr):
    # on transmet paquet à paquet
    for data in buff:
        if sock_data.sendto(data, addr):
            # attend confirmation d'un ACK de la part du client
            dat, addr = recvFromClient(sock_data)
            if Common.ACK in dat:
                continue
            else:
                print("Warning, didn't receive an ACK, received something else")


    else:
        print("Warning, an error occured while sending paquet during congestion handling ")
    pass


def main(argv):
    print(os.getcwd())

    buf = 1024
    print("Warning, launching server")
    print("Creating control socket")

    sock_control = socket.socket(socket.AF_INET,  # Internet
                                 socket.SOCK_DGRAM)  # UDP
    sock_control.bind((Common.UDP_IP, Common.UDP_PORT_CONTROL))

    print("creating data socket ")

    sock_data = socket.socket(socket.AF_INET,  # Internet
                              socket.SOCK_DGRAM)  # UDP
    sock_data.bind((Common.UDP_IP, Common.UDP_PORT_DATA))

    # handshake
    handshake_done = False

    data, addr = recvFromClient(sock_control)
    if data == Common.SYN:
        sock_control.sendto(Common.SYNACK + str(Common.UDP_PORT_DATA).encode('utf-8'), addr)
        print(f"Watining for {Common.ACK}")
        data, addr = recvFromClient(sock_control)
        if data == Common.ACK:
            handshake_done = True

        else:
            print(f"Expected : {Common.ACK}, received : {data}")
    else:
        print(f"Expected : {Common.SYN}, received : {data}")

    print(f"Handshake {'done' if handshake_done else 'failed'}")

    # reçoit le nom du fichier recherché
    file_name_received, addr = recvFromClient(sock_data)
    file_name_received = file_name_received.decode('utf-8')
    print("received message: %s" % data)
    print(addr)
    print(file_name_received)

    while True:

        rep = int(input("Voulez vous continuer à transmettre ? Oui : 0 , non : 1 "))
        if rep == 1:
            break

        if file_name_received.endswith(Common.jpg) or file_name_received.endswith(Common.png):
            print("Sending requested image..... " + file_name_received)
        if file_name_received.endswith(Common.pdf):
            print("Sending requested pdf..... " + file_name_received)
        if file_name_received.endswith(Common.mp3):
            print("Sending requested music..... " + file_name_received)
        if file_name_received.endswith(Common.txt):
            print("Sending requested text..... " + file_name_received)

        valid = False

        while not valid:
            path = os.getcwd()
            file_name = path + "/../input/" + file_name_received
            print(file_name)
            try:
                f = open(file_name, mode="rb").read()
                buf = [f[k * Common.BUFFER:(k + 1) * Common.BUFFER] for k in range((len(f) // Common.BUFFER) + 1)]
                # sock_data.sendto(file_name.encode('utf-8'), addr)
                # sock_data.sendto(str(len(buf)).encode('utf-8'), addr)
                file_stats = os.stat(file_name)
                size = file_stats.st_size
                print("File found")
                valid = True
            except FileNotFoundError:
                print(" Hold on ! The file can't be found !")

        # où on en est à l'envoie
        seq = 0
        # où on est en reception
        rec = 0
        window_size = 3
        # nombre de paquets recus/ envoyés dans la fenetre
        cont_envoye = 0

        print("Here we go ! Transmitting....")
        t1_start = perf_counter()
        for data in buf:

            # storing the received ACK
            stored = []

            # si on est encore dans le window size: on envoie
            if cont < window_size:
                print("Sending in the window")
                num_sequence = str(seq).zfill(6)
                if sock_data.sendto(num_sequence.encode('utf-8') + data, addr):
                    # incrémente le compteur total de paquets envoyés
                    seq += 1
                    # incrémente le nombre de paquets envoyé dans la fenetre
                    cont_envoye += 1
                    continue
                else:
                    num_sequence = str(seq)
                    print("Warning, an error occured while sending paquet number " + num_sequence)

            # si on est en dehors de la window size : on check les ACK des séquences et on retransmet
            # éventuellement
            else:
                print("checking ACKS")
                checked = perf_counter()
                current = perf_counter()

                nbr_ACK = 0
                # laisse délais pour checker les ACKs
                while (current - checked) < 0.000005:
                    current = perf_counter()
                    rcv, dat = recvFromClient(sock_data)
                    sequence_recue = rcv.decode("utf-8")[3:]
                    print("received : " + sequence_recue)
                    sequence_recue = int(sequence_recue)
                    if sequence_recue not in stored :
                        stored.append(sequence_recue)
                        nbr_ACK += 1

            # on check le nombre d'ACK reçus après le délais
            # si a pas tout reçu
            if nbr_ACK < window_size:
                print("Timeout , entering congestion mode")
                
                #regarde à partir d'où doit retransmettre
                stored.sort()
                congestion(buf[rec:], sock_data, addr)
                cont = 0
                rec = seq
                window_transmitted = True
            else:
                window_transmitted = True

        sock_data.sendto(Common.FIN, addr)
        print("Sent with success ! :white_check_mark:")
        t1_stop = perf_counter()
        total_time = t1_stop - t1_start
        print(size)
        print(total_time)

        debit = size / total_time
        print("The current debit is {:.2f} Mo/s".format(debit * 0.000001))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Server port")
    args = parser.parse_args()

    if args.port:
        main(args.port)
