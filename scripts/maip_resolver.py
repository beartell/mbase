class MaipResolver:
    def __init__(self):
        self.maip_name = ""
        self.identification_line = ""
        self.server_message = ""
        self.status_code = 0
        self.dict = {}
        self.key=""
        self.value = 0
        self.end_name = ""


    def convert_value(self,value):
                 try:
                     if '.' in value:
                         return float(value)
                     return int(value)
                 except ValueError:
                  return value


    def get_kval(self, in_key) -> list:
        return self.dict[in_key]

    def get_data(self) -> str:
        return self.server_message

    def get_status_code(self) -> int:
        return self.status_code

    def resolve(self,sv_message):
        msg_len = len(sv_message) 
        msg_iterator = 0       
        
        for msg_iterator in range(msg_len):
            if sv_message[msg_iterator] == ' ':
                break
            self.maip_name += sv_message[msg_iterator]
            
        msg_iterator += 1 
        tmpStatus = ""
        for msg_iterator in range(msg_iterator, msg_len):
            if sv_message[msg_iterator] == '\n':
                break
            tmpStatus += sv_message[msg_iterator]
        msg_iterator += 1
        self.status_code = int(tmpStatus)   
        
        activeKey = ""
        activeValue = ""
        keyParsed = False

        for msg_iterator in range(msg_iterator, msg_len):
            if keyParsed == False:
                if sv_message[msg_iterator] == ':':
                    self.dict[activeKey] = []
                    keyParsed = True
                    continue
                activeKey += sv_message[msg_iterator]
                if activeKey == "END\n":
                    break
            else:
                if sv_message[msg_iterator] == '\n':
                    if len(activeValue) != 0:
                        
                        activeValue = self.convert_value(activeValue) # burayı ekledim
                        self.dict[activeKey].append(activeValue)

                    keyParsed = False
                    activeKey = ""
                    activeValue = ""
                    continue

                if sv_message[msg_iterator] == ';':
                    
                    activeValue = self.convert_value(activeValue) # burayıda ekledim.
                    self.dict[activeKey].append(activeValue)
                    activeValue = ""
                    continue
                activeValue += sv_message[msg_iterator]

        try:
            if self.dict["LENGTH"]:
             msg_iterator += 1
             dataLength = int(self.dict["LENGTH"][0])
             try:
                 for msg_iterator in range(msg_iterator, msg_iterator + dataLength):
                    self.server_message += sv_message[msg_iterator]
             except IndexError:
                pass
        except KeyError:
            pass