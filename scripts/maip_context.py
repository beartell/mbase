from maip_builder import MaipBuilder
from maip_resolver import MaipResolver
from maip_operation_object import MaipOperationResult

class MaipContext:
    
    def __init__(self, ownercl, inCtxId):
        self.CTXID=inCtxId
        self.owner_client = ownercl

    def get_CTXID(self):
        return self.CTXID
    
    def get_context_status(self):
        if self.owner_client.is_operation_available() == False:
            return MaipOperationResult()

        my_builder = MaipBuilder()
        my_builder.set_request_identification("INF", "inf_get_context_status")
        my_builder.set_kval("CSID", self.owner_client.CSID)
        my_builder.set_kval("CLID", self.owner_client.CLID)
        my_builder.set_kval("CTXID", self.CTXID)
        generatedData = my_builder.generate_payload()
        self.owner_client.maip_socket.sendall(generatedData.encode())
        resultBytes = self.owner_client.maip_socket.recv(16384)
        if resultBytes:
            resultString = resultBytes.decode()
            my_resolver = MaipResolver()
            my_resolver.resolve(resultString)
            return MaipOperationResult(None, my_resolver.get_status_code())
        
        return MaipOperationResult()
    
    def is_context_active(self):
        if self.get_context_status().get_result_code() == 2012:
            return True
        return False

    def get_owner_client(self):
        return self.owner_client

    def set_input(self, role, input_prompt):
        if self.owner_client.is_operation_available() == False:
            return MaipOperationResult()

        myRole = role
        myInput = input_prompt

        my_builder = MaipBuilder()
        my_builder.set_request_identification("EXEC", "exec_set_input")
        my_builder.set_kval("STOK", self.owner_client.session_token)
        my_builder.set_kval("CTXID", self.CTXID)
        my_builder.set_kval("ROLE", myRole)
        
        generatedData = my_builder.generate_payload(myInput)
        self.owner_client.maip_socket.sendall(generatedData.encode())
        resultBytes = self.owner_client.maip_socket.recv(16384)
        if resultBytes:
            resultString = resultBytes.decode()
            my_resolver = MaipResolver()
            my_resolver.resolve(resultString)
            if my_resolver.get_status_code() == 2000:
                return MaipOperationResult(my_resolver.get_kval("MSGID")[0], my_resolver.get_status_code())

            return MaipOperationResult(None, my_resolver.get_status_code())
        return MaipOperationResult()
    
    def execute_input(self, inMsgIds):
        if self.owner_client.is_operation_available() == False:
            return MaipOperationResult()
        
        my_builder = MaipBuilder()
        my_builder.set_request_identification("EXEC", "exec_execute_input")
        my_builder.set_kval("STOK", self.owner_client.session_token)
        my_builder.set_kval("CTXID", self.CTXID)
        
        for ids in inMsgIds:
            my_builder.set_kval("MSGID", ids)
        
        generatedData = my_builder.generate_payload()
        self.owner_client.maip_socket.sendall(generatedData.encode())
        resultBytes = self.owner_client.maip_socket.recv(16384)
        if resultBytes:
            resultString = resultBytes.decode()
            my_resolver = MaipResolver()
            my_resolver.resolve(resultString)
            print(resultString)
            return MaipOperationResult(None, my_resolver.get_status_code())
        return MaipOperationResult()

    def get_next(self):
        if self.owner_client.is_operation_available() == False:
            return MaipOperationResult()
        
        my_builder = MaipBuilder()
        my_builder.set_request_identification("EXEC", "exec_next")
        my_builder.set_kval("STOK", self.owner_client.session_token)
        my_builder.set_kval("CTXID", self.CTXID)
        generatedData = my_builder.generate_payload()
        self.owner_client.maip_socket.sendall(generatedData.encode())
        resultBytes = self.owner_client.maip_socket.recv(16384)
        if resultBytes:
            resultString = resultBytes.decode()
            my_resolver = MaipResolver()
            my_resolver.resolve(resultString)
            isSpecial = True
            if my_resolver.get_status_code() == 3006:
                # since the finished status code does not send a SPECIAL character, we must handle that
                isSpecial = my_resolver.get_kval("SPECIAL")[0]
            if isSpecial == 1:
                outputFormat = {"token" : my_resolver.get_data(), "special" : True}
                return MaipOperationResult(outputFormat, my_resolver.get_status_code())
            else:
                outputFormat = {"token" : my_resolver.get_data(), "special" : False}
                return MaipOperationResult(outputFormat, my_resolver.get_status_code())
        return MaipOperationResult()
        
    