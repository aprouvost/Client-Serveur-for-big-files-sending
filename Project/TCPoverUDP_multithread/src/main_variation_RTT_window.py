import socket
import utils
import threading
import queue
from clientNconnexionControl import controlConnexion

print("Creating control socket")

ports_used = []
queue_ports = queue.Queue()
ports_list = [2424]

# récupère les ports utilisés

sock_control = socket.socket(socket.AF_INET,
                             socket.SOCK_DGRAM)  # UDP

sock_control.bind((utils.IP_ADDR, 2424))

while True:

    for RTT in [0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08]:
        print(f"RTT -> {RTT}")

        data, addr = utils.recvFromClient(sock_control)
        # debut du handshake il se fini dans un thread
        print(f"Main received : {data}")
        if data == utils.SYN:
            print("thread creation")
            thread = threading.Thread(target=controlConnexion,
                                      args=(sock_control, addr, ports_list, queue_ports, RTT, 100))
            thread.start()
            item = queue_ports.get()
            ports_list.append(item)
            print(f"DEBUG : port list : {ports_list}")
            thread.join()
