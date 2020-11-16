import socket
import Project.TCPoverUDP.common as Common
import os
import io
import sys
import PIL.Image as Image
# import emoji
import time

os.environ['DJANGO_SETTINGS_MODULE'] = 'mysite.settings'

sock = socket.socket(socket.AF_INET,  # Internet
                     socket.SOCK_DGRAM)  # UDP

# Handshake
handshake_done = False
sock.sendto(Common.SYN, (Common.UDP_IP, Common.UDP_PORT_CONTROL))  # Send SYN
data, addr = sock.recvfrom(Common.BUFFER)
if data == Common.SYNACK + b" " + str(Common.UDP_PORT_DATA).encode('utf-8'):
    print(data)
    port_data = int(str(data).split(" ")[1][:-1])
    sock.sendto(Common.ACK, (Common.UDP_IP, Common.UDP_PORT_CONTROL))
    print(f"Received  {data}, sent  {Common.ACK}")
    handshake_done = True

else:
    print(f"Expected : {Common.SYNACK}, received : {data}")

print(f"Handshake {'done' if handshake_done else 'failed'}")

if handshake_done:
    while True:
        message = input("Enter a string : ").encode('utf-8')
        sock.sendto(message, (Common.UDP_IP, port_data))

        if message == Common.bonjour:
            data, addr = sock.recvfrom(Common.BUFFER)
            print(f"Server say : {data}")

        # client demande image au serveur
        if message == Common.sendimg:
            print("Waiting for server approval ... :thumbs_up: ")
            cop = "copy_"
            file_name_received, addr = sock.recvfrom(Common.BUFFER)
            file_size, addr = sock.recvfrom(Common.BUFFER)
            file_name_received = cop + file_name_received.decode("utf-8")
            extension = file_name_received.split(".")[1]
            print(extension)
            file_size = int(file_size.decode("utf-8"))
            buf = bytearray()

            print("Receiving.... Hold on ! ( And pray because this is UDP ) ")

            for data in range(file_size):
                # receiving data on data socket
                img_data, addr = sock.recvfrom(Common.BUFFER)
                # buf.append(int(img_data.decode("utf-16")))
                buf += img_data
                # sending ACK for received packet to server on control socket
                sock.sendto(Common.ACK, (Common.UDP_IP, Common.UDP_PORT_CONTROL))

        if extension == Common.jpg or extension == Common.png:
            image = Image.open(io.BytesIO(buf))
            image.save(file_name_received)
            print("Image eceived ! :heart_eyes: ")
        if extension == Common.pdf:
            outfile = open(file_name_received, 'wb')
            outfile.write(buf)
            outfile.close()
            print("PDF file received")
