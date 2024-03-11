import sys
import types
import socket
import selectors
import numpy as np
import json 
from threading import Thread

sel = selectors.DefaultSelector()

def EstablishFakeServer():
    HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
    PORT = 65432  # Port to listen on (non-privileged ports are > 1023)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print(f"FakeServer : Listening on port {PORT}...")
        conn, addr = s.accept()
        with conn:
            print(f"FakeServer : Incoming connection accepted at {addr}")
            while True:
                data = conn.recv(1024)
                print(f"Data : {data}")  
                #check for kosbot comfirmation message?
                if not data:
                    break
                conn.sendall(data)
            print("FakeServer : Connection dropped...")


def Test1():
    print("Test 1")
def Test2():
    print("Test 2")
def Test3():
    print("Test 3")


if __name__ == "__main__":
    print("Welcome to PyKosbot!\nThe terminal interface for Robot Simulation Version 2.")
    
    
    run = True
    while run:
        print("What would you like to do?")
        print(f"1. Test 1")
        print(f"2. Test 2")
        print(f"3. Test 3")
        
        ans = int(input("Answer (int): ")) 
        if ans < 1 or ans > 3:
            run = False
        if ans == 1:
            Test1()
        if ans == 2:
            Test2()
        if ans == 3:
            Test3()
            
    print(f"Closing...")
    exit()
    
    fake_conn = False
    if fake_conn:
        server = Thread(target=EstablishFakeServer)
        server.start()
    
    HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
    PORT = 27015  # Port to listen on (non-privileged ports are > 1023)
    
    print("Connecting to Kosbot Server...")
    
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection = client.connect((HOST, PORT))
    print("Connection successfull.")
    print("Connected to Kosbot Server on port ", PORT, "...")
    
    client.close()
    client.sendall(b"Kosbot?")
    data = client.recv(1024)

    print(f"Received {data!r}")
    
    idx = 0
    opts = [(1, "Hello ", Tetst)]
    map(lambda x, y, z: print_opt(y, x), opts)
    
    ans = input("What would you like to do?")
    ans = int(ans)
    
    if ans < len(opts):
        print("Invalid...")
    
    
    if ans == "q":

        server.join()
        exit()
    #connect to tcp
    
    #not sure if this should be a server or client
    
    