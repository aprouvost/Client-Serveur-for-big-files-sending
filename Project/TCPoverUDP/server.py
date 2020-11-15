import socket
import Project.TCPoverUDP.common as Common
import os

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

# si serveur reçoit demande d'image, l'envoi au client
while True:
    data, addr = sock_data.recvfrom(Common.BUFFER)
    print("received message: %s" % data)
    print(addr)
    if data == Common.bonjour:
        print("Sending bye")
        sock_data.sendto(Common.aurevoir, addr)
    if data == Common.sendimg:
        file_name = input(
            "File requested, which File do you want to send ? (plz enter the extension : ex .jpg) :").encode('utf-8')
        f = open(file_name, mode="rb").read()
        buf = [f[k * Common.BUFFER:(k + 1) * Common.BUFFER] for k in range((len(f) // Common.BUFFER) + 1)]
        # envoi le nom du fichier
        sock_data.sendto(file_name, addr)
        # envoie la taille du fichier
        sock_data.sendto(str(len(buf)).encode('utf-8'), addr)
        for data in buf:
            print(data)
            if sock_data.sendto(data, addr):
                print("sending ...")
                # attend confirmation d'un ACK de la part du client
                rcv, addr = sock_control.recvfrom(Common.BUFFER)
                if rcv == Common.ACK:
                    continue
        f.close()
