import socket
import json
import time
HOST = "127.0.0.1"
PORT = 8000

max_buffer_size = 2048

string_packet_size_byte_amount = 4 #Before a packet is received, the size of it is sent in 2 bytes

packet_type_byte_amount = 2 #The type is descirbed in a 4 byte integer 

packet_types = {
  0 : "Invalid",
  1 : "String",
  2 : "IntegerArray",
  3 : "Test",
}
def ProcessInvalidPacket(content_bytes=bytes, packet_size=int):
    
    return "Invalid Packet - "

def ProcessStringPacket(content_bytes=bytes, packet_size=int):
    string_size_bytes = content_bytes[:string_packet_size_byte_amount -1 ]
    string_size_network = int.from_bytes(string_size_bytes, byteorder="little")
    string_size_network = socket.ntohs(string_size_network)
    
    byte_string = content_bytes[string_packet_size_byte_amount:]
    string = byte_string.decode('UTF-8')
    return string

def ProcessIntegerArrayPacket(content_bytes=bytes, packet_size=int):
    int_byte_amount = 4
    int_amount = int(len(content_bytes) / int_byte_amount)

    ints = []
    for i in range(int(int_amount)):
        ints.append(int.from_bytes(content_bytes[i * int_byte_amount : (i * int_byte_amount) + int_byte_amount]))
    print("ints", ints)
    
    return ints

def ProcessTestPacket(content_bytes=bytes, packet_size=int):
    
    return

packet_type_funcs = {
    0 : ProcessInvalidPacket,
    1 : ProcessStringPacket,
    2 : ProcessIntegerArrayPacket,
    3 : ProcessTestPacket,
}

def ProcessPacket(size, contents):
    type_bytes = contents[0:packet_type_byte_amount]
    type_bytes = int.from_bytes(type_bytes, byteorder="little")
    type_index = socket.ntohs(type_bytes)

    contents_bytes = contents[packet_type_byte_amount:]
    
    packet_contents = packet_type_funcs[type_index](contents_bytes, size)

    return packet_types[type_index], size, packet_contents

def RecievePacket(sock=socket):
    packet_size = sock.recv(string_packet_size_byte_amount)
    packet_size = int.from_bytes(packet_size, byteorder="little")
    packet_size = socket.ntohs(packet_size)

    packet_content = sock.recv(packet_size)

    type, size, content  = ProcessPacket(size=packet_size, contents=packet_content)
           
    print("PACKET SIZE : ", packet_size)
    print("WHOLE PACKET : ", packet_content)
    print("TYPE : ", type)
    print("CONTENTS :", content)
    return type, size, content

def MakeStringPacketBuffer(message=str):
    byte_type = int.to_bytes(1, length=packet_type_byte_amount, byteorder="big")#1 - String, 4 - 4 byte integer, Big - endianess
    
    byte_string = message.encode('UTF-8')
    byte_string_size = int.to_bytes(len(byte_string), length=string_packet_size_byte_amount, byteorder="big")

    byte_packet = byte_type + byte_string_size
    byte_packet = byte_packet + byte_string
    byte_packet_size = int.to_bytes((len(byte_packet)), string_packet_size_byte_amount, byteorder="big")
    
    print("packet size : ", len(byte_packet))
    print("size : ", byte_packet_size)
    print("string size : ", int.from_bytes(byte_packet_size))
    print("packet : ", byte_packet)
    
    return byte_packet_size, byte_packet
    
def SendPacket(sock=socket):
    size, packet = MakeStringPacketBuffer("Hei")

    
    sent = sock.send(size)
    print("Sent ", sent, " bytes...")
    time.sleep(0.2)
    sent = sock.send(packet)
    print("Sent ", sent, " bytes...")
    return

if __name__ == "__main__":

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((HOST, PORT))
    

    while(True):
        type, size, content = RecievePacket(sock)
        #msg = input("Your message to server : ")
        #msg = str(msg)
        #if msg == "q":
        #    break
        #
        #size, packet = MakeStringPacketBuffer(msg)
        #sock.send(size)
        #sock.send(packet)
    
    sock.close()
    exit()