IP_ADDR = "0.0.0.0"  # To listen on all interfaces
BUFFER = 1500 - 6  # Can up to 65 000 length of UP Packet ( -6 for sequence )


ACK = "ACK"
SYN = "SYN"
SYN_ACK = "SYN-ACK"
FIN = "FIN"


def init():
    global last_ack
    last_ack = 0


def recvFromClient(sock):
    """
    :param sock:
    :return: tuple with (data received, address and port Tuple of recvfrom methode)
    """

    data, addr = sock.recvfrom(BUFFER)
    data = data.decode("utf-8")
    if data.endswith('\x00') or data.endswith('\n'):
        data = data[:-1]
    return data, addr


def sendToClient(sock, addr, message):
    """
    :param addr: addresse to send
    :param sock: use socket to send
    :param message: string to send
    :return: return statement of sendto methode
    """
    return sock.sendto(message.encode("utf-8"), addr)
