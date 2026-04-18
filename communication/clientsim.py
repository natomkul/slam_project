import socket
import time

HOST = "192.168.1.16"   # zmień na IP serwera
PORT = 3000

hex_data = """
54 2C 68 08 AB 7E E0 00 E4 DC 00 E2 D9 00 E5 D5 00
E3 D3 00 E4 D0 00 E9 CD 00 E4 CA 00 E2 C7 00 E9
C5 00 E5 C2 00 E5 C0 00 E5 BE 82 3A 1A 50
"""

payload = bytes.fromhex(hex_data)

print("Packet size:", len(payload))
print("Bytes:", payload.hex(" "))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print("Connected")

    while True:
        s.sendall(payload)
        print("Sent frame")
        time.sleep(1)
