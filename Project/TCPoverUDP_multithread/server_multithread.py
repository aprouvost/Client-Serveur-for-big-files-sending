import socket
import common as Common
from time import perf_counter

import os
import time
import argparse
from threading import *
# import emoji


def recvFromClient(sock):
    data, addr = sock.recvfrom(Common.BUFFER)
    data = data.decode("utf-8")
    if data.endswith('\x00'):
        data = data[:-1]
    return data.encode("utf-8"), addr


def main(argv):
    print(os.getcwd())

    print("Warning, launching server")
    print("Creating control socket")

    # lance le handshake en thread, récupère la socket data créé avec le handshake

    # lance l'envoie du fichier sur la socket data  send_file()


def handshake(port):
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

    print("received message: %s" % data)
    print(f"Handshake {'done' if handshake_done else 'failed'}")

    return sock_data


def timer(delta_t):
    t = Timer(delta_t, timer)
    t.start()
    return True


def ack_management(seq, data, sock_data, addr, buf):
    while True:
        print("ACK management launched")
        current_ACK = 0
        ACK_rcv, addr = recvFromClient(sock_data)
        sequence_recue = int(ACK_rcv.decode("utf-8")[3:])
        timeout = False
        delta_t = 5
        if sequence_recue > current_ACK:
            # on update avec l'ACK reçu le plus récent
            current_ACK = sequence_recue
            print(ACK_rcv.decode("utf-8"))
            # kill les timers déjà en cours
            # j'ai aucune idée du delta temps qu'il faut laisser
            timeout = timer(delta_t)
        if timeout:
            # kill les timers déjà en cours
            data = buf[seq]
            retransmission(seq, data, sock_data, addr)
            timeout = timer(delta_t)


def retransmission(seq, data, sock_data, addr):
    num_sequence = str(seq).zfill(6)
    if sock_data.sendto(num_sequence.encode('utf-8') + data, addr):
        # kill le thread
        return True
    else:
        return False


def send_file(sock_data):
    # reçoit le nom du fichier recherché
    file_name_received, addr = recvFromClient(sock_data)
    file_name_received = file_name_received.decode('utf-8')
    print("Received file name: %s" % file_name_received)

    if file_name_received.endswith(Common.jpg) or file_name_received.endswith(Common.png):
        print("Sending requested image..... " + file_name_received)
    if file_name_received.endswith(Common.pdf):
        print("Sending requested pdf..... " + file_name_received)
    if file_name_received.endswith(Common.mp3):
        print("Sending requested music..... " + file_name_received)
    if file_name_received.endswith(Common.txt):
        print("Sending requested text..... " + file_name_received)

    # ----------------------------------------------------------------------
    # Ouverture du fichier et transformation en bytes
    # ----------------------------------------------------------------------

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

    # ----------------------------------------------------------------------
    # Les bytes sont envoyés dans la socket dans le format attendu
    # ----------------------------------------------------------------------

    # lance en // le thread de gestion des ACK  ack_managment()
    # -----> TO DO, LANCER ICI LE THREAD DE GESTION DES ACK
    # ack_management(seq, data, sock_data, addr)

    # compteur d'envois
    seq = 0
    print("Here we go ! Transmitting....")
    t1_start = perf_counter()
    for data in buf:

        num_sequence = str(seq).zfill(6)
        if sock_data.sendto(num_sequence.encode('utf-8') + data, addr):
            # incrémente le compteur total de paquets envoyés
            seq += 1
        else:
            num_sequence = str(seq)
            print("Warning, an error occured while sending paquet number " + num_sequence + "---> Retransmitting")
            # envoi à la retransmission dans un autre thread pour pas bloquer celui là?
            # Si ça retourne False c'est que ça a pas envoyé, faut gérer ça, peut etre relancer le thread
            retransmitted = retransmission(seq, data, sock_data, addr)
            seq += 1

    # TO DO : JOIN LES THREADS ICI

    t1_stop = perf_counter()
    # ----------------------------------------------------------------------
    # Chaine "FIN" envoyée, et calcul du débit
    # ----------------------------------------------------------------------
    sock_data.sendto(Common.FIN, addr)
    print("----> Sent with success !")
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
