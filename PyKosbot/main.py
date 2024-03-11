import sys
import types
import socket
import selectors
import numpy as np
import json 
from threading import Thread

sel = selectors.DefaultSelector()

def EstablishFakeServer(self):
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

   
        
if __name__ == "__main__":
    print("Welcome to PyKosbot!\nThe terminal interface for Robot Simulation Version 2.")

    fake_conn = True
    if fake_conn:
        server = Thread(target=EstablishFakeServer)
        server.start()
    
    HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
    PORT = 65432  # Port to listen on (non-privileged ports are > 1023)
    
    print("Connecting to Kosbot Server...")
    
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection = client.connect((HOST, PORT))
    print("Connected to Kosbot Server on port ", PORT, "...")
    
    client.sendall(b"Kosbot?")
    #data = client.recv(1024)

    #print(f"Received {data!r}")
    print("Connection successfull.")
    
    ans = input("What would you like to do?")
    if ans == "q":

        server.join()
        exit()
    #connect to tcp
    
    #not sure if this should be a server or client
    
    