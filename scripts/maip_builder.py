class MaipBuilder:
    def __init__(self):
        self.major=1
        self.minor=0
        self.identification_line = ""
        self.version_name = "MAIP"
        self.kvals = {}
        self.user_message = ""
        self.end = "END"
        self.length=""
        self.length_word = "LENGTH"

    def set_version(self,major,minor):
        self.major=major
        self.minor=minor

    def get_version(self):
        return f"{self.version_name}{self.major}.{self.minor}"        

    def set_request_identification(self, var, inf_acq_model):
        formatted_string = f"{self.version_name}{self.major}.{self.minor} {var} {inf_acq_model} "
        self.identification_line = formatted_string
        return self.identification_line

    def user_input(self,client_input):
        self.user_message = client_input
        return self.user_message

    def length_of_word_function(self,client_input):
        self.length=len(client_input)
        length_format = f"{self.length_word}:{self.length}"
        return length_format
    
    def set_kval(self,key,value):
        if self.kvals.get(key) == None:
            self.kvals[key] = []
            self.kvals[key].append(str(value))
        else:
            self.kvals[key].append(";" + str(value))

        return f"{key}:{value}"
        
    def end_word(self):
        return self.end

    def generate_payload(self, client_input = ""):
        generatePayload = self.identification_line + '\n'
        dataLength = len(client_input.encode())
        if dataLength:
            self.set_kval("LENGTH", dataLength)
            self.user_message = client_input

        for msg in self.kvals:
            generatePayload += msg + ":"
            for msgValue in self.kvals[msg]:
                generatePayload += msgValue
            generatePayload+='\n'    
        generatePayload += self.end + '\n'
        generatePayload += self.user_message
        return generatePayload