import socket

#import export as export

import Project.TCPoverUDP.common as Common
import os
import time

# import emoji


print(os.getcwd())

buf = 1024
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
data, addr = sock_control.recvfrom(Common.BUFFER)
if data == Common.SYN:
    sock_control.sendto(Common.SYNACK + b" " + str(Common.UDP_PORT_DATA).encode('utf-8'), addr)
    print(f"Watining for {Common.ACK}")
    data, addr = sock_control.recvfrom(Common.BUFFER)
    if data == Common.ACK:
        handshake_done = True

    else:
        print(f"Expected : {Common.ACK}, received : {data}")
else:
    print(f"Expected : {Common.SYN}, received : {data}")

print(f"Handshake {'done' if handshake_done else 'failed'}")

while True:
    data, addr = sock_data.recvfrom(Common.BUFFER)
    print("received message: %s" % data)
    print(addr)
    if data == Common.bonjour:
        print("Sending bye")
        sock_data.sendto(Common.aurevoir, addr)
    if data == Common.sendimg:
        valid = False
        while not valid:
            file_name = input(
                "File requested, which File do you want to send ? (plz enter the extension : ex .jpg) :").encode(
                'utf-8')
            try:
                f = open(file_name, mode="rb").read()
                buf = [f[k * Common.BUFFER:(k + 1) * Common.BUFFER] for k in range((len(f) // Common.BUFFER) + 1)]
                sock_data.sendto(file_name, addr)
                sock_data.sendto(str(len(buf)).encode('utf-8'), addr)
                valid = True
            except FileNotFoundError:
                print(" Hold on ! The file can't be found !")

        print("Here we go ! Transmitting....")
        for data in buf:
            transmitted = False
            checked = time.time()
            current = time.time()
            while transmitted == False:

                if sock_data.sendto(data, addr):
                    # attend confirmation d'un ACK de la part du client

                    while (current - checked) < 0.0000005:
                        current = time.time()
                        rcv, addr = sock_control.recvfrom(Common.BUFFER)
                        if rcv == Common.ACK:
                            transmitted = True
                            break
                        else:
                            print("Mhm this is confusing. No clue what just happened. Received : " + str(rcv))

                    if transmitted == False:
                        print("Timeout, packet lost. Retransmitting....")

        print("Sent with success ! :white_check_mark:")
