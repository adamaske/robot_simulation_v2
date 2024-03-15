import sys
import types
import socket
import selectors
import numpy as np
import json 
from threading import Thread
import json
 

class Server(Thread):
    def __init__(self, args):
        Thread.__init__(self)
        self.host = args[0]
        self.port = args[1]
        
        self.msg_queue = []
        
        self.running = True
        self.connected = False
        
        self.logged_msgs = []
        
    def connect(self):
        self.connected = False
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind((self.host, self.port))
        sock.listen()
        print(f"Server : Listening on port {PORT}...")
        self.conn, addr = sock.accept()   
        self.conn.setblocking(False)
        self.conn.settimeout(1)        
        self.connected = True
        print(f"Server : Accepted connection at {addr}")

    def run(self):
        if not self.connected:
            self.connect()
        while self.running and self.connected:
            self.checkout_queue()
            self.listen()
            
    def add_msg(self, msg):
        self.msg_queue.append(msg)
        
    def checkout_queue(self):
        if len(self.msg_queue) == 0:
            return
        for msg in self.msg_queue:
            self.conn.sendall(msg)
        self.msg_queue = []
        
    def listen(self):
        try:
            msg = self.conn.recv(1024)
        except socket.timeout:
            return
        except socket.error:
            self.close_server()
            return
        else:
            if len(msg) == 0:
                #the client disconnected 
                return
            else:
                return
        
    def close_server(self):
        self.connected = False
        self.conn.close()
    
    def log(self, msg):
        self.log.append(msg)
        
    def checkout_log(self):
        for lg in self.logged_msgs:
            print(lg)
            
def opt1(server):
    cmd = {
        "kosbot" : "1.0",
        "command" : "pose",
    }
    json_obj = json.dumps(cmd)
    bytes_msg = str.encode(json_obj)
    server.add_msg(bytes_msg)
    
def opt2(server):
    cmd = {
        "kosbot" : "1.0",
        "command" : "update",
    }
    json_obj = json.dumps(cmd)
    bytes_msg = str.encode(json_obj)
    server.add_msg(bytes_msg) 

def disconnect_server(server):
    server.close_server()
    
def check_server(server):
    
    return

if __name__ == "__main__":
    print("Welcome to PyKosbot!\nThe terminal interface for Robot Simulation Version 2.")
    
    HOST = "127.0.0.1"
    PORT = 8000
   
    server_thread = Server(args=(HOST, PORT))
    server_thread.start()
    
    opts = ((opt1, "Send robot msg"), 
            (opt2, "opt2"), 
            (check_server, "Server Status"))
    
    run = True
    while run:
        print("What would you like to do?")
        for opt in range(len(opts)):
            print(f"{opt + 1}. {opts[opt][1]}")
            
        ans = int(input("Answer (int): ")) - 1
        if ans < 0 or ans > len(opts)-1:
            input("Unrecongized...")
            break

        opts[ans][0](server_thread)    
            
    print(f"Closing...")
    server_thread.close_server()
    server_thread.join()
    exit()

    