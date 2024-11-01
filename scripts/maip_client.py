import socket
from maip_builder import MaipBuilder
from maip_resolver import MaipResolver
from maip_context import MaipContext
from maip_operation_object import MaipOperationResult
import time

class MaipClient:
    def __init__(self, hostname : str, hostport : int, username : str, access_token : str):
        self.host_name = hostname
        self.host_port = hostport
        self.user_name = username
        self.access_token = access_token
        self.session_token = ""
        self.maip_socket = None

        if self.host_name != "":
            self.maip_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                self.maip_socket.connect((self.host_name, self.host_port))
            except OSError as msg:
                self.maip_socket.close()
                self.maip_socket = None
                
    def is_connection_active(self) -> bool:
        if self.maip_socket == None:
            return False
        return True

    def is_operation_available(self) -> bool:
        if self.is_connection_active() == False:
            return False

        if self.session_token == "":
            return False
        return True

    def get_session_token(self) -> str:
        return self.session_token
    
    def get_username(self) -> str:
        return self.user_name

    def get_host_name(self) -> str:
        return self.host_name

    def get_host_port(self) -> int:
        return self.host_port

    def get_raw_socket(self) -> socket:
        return self.maip_socket

    def create_client(self) -> MaipOperationResult:
        if self.is_connection_active() == False:
            return MaipOperationResult()

        if self.session_token == "":
            my_builder = MaipBuilder()
            my_builder.set_request_identification("INF", "inf_access_request")
            my_builder.set_kval("USERNAME", self.user_name)
            my_builder.set_kval("ACCTOKEN", self.access_token)
            generatedData = my_builder.generate_payload()
            self.maip_socket.sendall(generatedData.encode())
            
            resultBytes = self.maip_socket.recv(16384)
            if resultBytes:
                resultString = resultBytes.decode()
                my_resolver = MaipResolver()
                my_resolver.resolve(resultString)
                operationResult = MaipOperationResult(None, my_resolver.get_status_code())
                if my_resolver.get_status_code() == 2000:
                    self.session_token = my_resolver.get_kval("STOK")[0]
                return operationResult
            return MaipOperationResult()
        
    def get_program_models(self) -> MaipOperationResult:
        if self.is_operation_available() == False:
            return MaipOperationResult()

        my_builder = MaipBuilder()
        my_builder.set_request_identification("INF", "inf_get_program_models")
        my_builder.set_kval("STOK", self.session_token)
        generatedData = my_builder.generate_payload()
        self.maip_socket.sendall(generatedData.encode())
        
        resultBytes = self.maip_socket.recv(16384)
        if resultBytes:
            resultString = resultBytes.decode()
            my_resolver = MaipResolver()
            my_resolver.resolve(resultString)
            
            if my_resolver.get_status_code() == 2000:
                return MaipOperationResult(my_resolver.get_kval("MODEL"), my_resolver.get_status_code())                    
            return MaipOperationResult(None, my_resolver.get_status_code())
        return MaipOperationResult()
    
    def load_model(self, in_model_name, in_token) -> MaipOperationResult:
        if self.is_operation_available() == False:
            return MaipOperationResult()

        my_builder = MaipBuilder()
        my_builder.set_request_identification("INF", "inf_load_model")
        my_builder.set_kval("STOK", self.session_token)
        my_builder.set_kval("MODEL", in_model_name)
        my_builder.set_kval("CTXSIZE", in_token)
        generatedData = my_builder.generate_payload()
        self.maip_socket.sendall(generatedData.encode())
        resultBytes = self.maip_socket.recv(16384)
        if resultBytes:
            resultString = resultBytes.decode()
            my_resolver = MaipResolver()
            my_resolver.resolve(resultString)
            print(resultString)
    
    def unload_model(self, in_model_name) -> MaipOperationResult:
        if self.is_operation_available() == False:
            return MaipOperationResult()

        my_builder = MaipBuilder()
        my_builder.set_request_identification("INF", "inf_unload_model")
        my_builder.set_kval("STOK", self.session_token)
        my_builder.set_kval("MODEL", in_model_name)
        generatedData = my_builder.generate_payload()
        self.maip_socket.sendall(generatedData.encode())
        resultBytes = self.maip_socket.recv(16384)
        if resultBytes:
            resultString = resultBytes.decode()
            my_resolver = MaipResolver()
            my_resolver.resolve(resultString)
            print(resultString)

    def create_context(self, in_model_name, in_token) -> MaipOperationResult:
        if self.is_operation_available() == False:
            return MaipOperationResult()

        my_builder = MaipBuilder()
        my_builder.set_request_identification("INF", "inf_create_context")
        my_builder.set_kval("STOK", self.session_token)
        my_builder.set_kval("MODEL", in_model_name)
        my_builder.set_kval("CTXSIZE", in_token)
        generatedData = my_builder.generate_payload()
        self.maip_socket.sendall(generatedData.encode())
        resultBytes = self.maip_socket.recv(16384)
        if resultBytes:
            resultString = resultBytes.decode()
            my_resolver = MaipResolver()
            my_resolver.resolve(resultString)
            if my_resolver.get_status_code() == 2000:
                return MaipOperationResult(MaipContext(self, my_resolver.get_kval("CTXID")[0]), my_resolver.get_status_code())
            return MaipOperationResult(None, my_resolver.get_status_code())
        return MaipOperationResult()

newMaipClient = MaipClient("127.0.0.1", 4553, "admin", "yumniye99")
newMaipClient.create_client()

outData = newMaipClient.get_program_models()
for i in outData.get_result_data():
    newMaipClient.load_model(i, 12000)

# print(outData.get_result_data())
# print(outData.get_result_code())

outData = newMaipClient.create_context("Llama 3.2 1B Instruct", 2000)
print(outData.get_result_code())
if outData.get_result_code() == 2000:
    myContext : MaipContext = outData.get_result_data()
    inputIdList = []
    inputSetResult = myContext.set_input("System", "You are a helpful assistant and your name is Joe.")
    inputIdList.append(inputSetResult.get_result_data())
    outPrompt = input("Enter your prompt: ")
    inputSetResult = myContext.set_input("User", outPrompt)
    inputIdList.append(inputSetResult.get_result_data())
    myContext.execute_input(inputIdList)
    
    aiResponse = myContext.get_next()
    aiResponseText = ""
    while aiResponse.get_result_code() == 3006:
        aiResponseText += aiResponse.get_result_data()["token"]
        aiResponse = myContext.get_next()
    print(aiResponseText)
