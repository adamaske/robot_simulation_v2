import json

import Kosbot_Info

class Kosbot():
    def __init__(self) -> None:
        
        self.version = "1.0"
        self.running = False    
        self.initialized = self.setup_kosbot()
     
        self.connection_ids = []
        return
    
    def setup_kosbot(self):
        self.resp_map = { "request_connection" : self.connect_client,
                        "disconnect" : self.disconnect_client,
                        "close_kosbot" : self.stop_kosbot,
                        "print_kosbot_info" : self.print_info
                         }
        
        self.running = True
        return True
    
    def parse_msg(self, data):
        print(f"Kosbot : parse_msg : {data!r}")
        
        js = json.loads(data.decode('UTF-8', 'strict'))
        command = js["command"]
        
        js_resp = self.resp_map[command["request"]](command)
        js = Kosbot_Info.wrap_request(js_resp)
        
        outgoing = json.dumps(js)
        outgoing = str.encode(outgoing) 
        return outgoing
    
    def connect_client(self, cmd):
        con_id = len(self.connection_ids)
        self.connection_ids.append(con_id)
        cmd = {
            "request" : "connection_accepted",
            "connection_id" : con_id
        }
        return cmd
    
    def disconnect_client(self, cmd):
        id_to_dq = cmd["connection_id"]
        cmd = {
            "request" : "accepted_disconnection",
            "disconnected_id" : id_to_dq
        }
        self.connection_ids.remove(id_to_dq)
        return
    def print_info(self, cmd):
        print("========= KOSBOT ============")
        print(f"Requested by {cmd["connection_id"]}")
        print("Active connections : ", self.connection_ids)
        
        return
    def stop_kosbot(self, cmd):
        self.running = False
        print("Stopping Kosbot!")
        resp = {
            "request" : "closed"
        }
        return resp
    def is_running(self):
        return self.running