class MaipOperationResult :
    def __init__(self, in_data = None, in_result_code = 0) -> None:
        self.result_code = in_result_code
        self.result_data = in_data
    
    def get_result_code(self):
        return self.result_code
    def get_result_data(self):
        return self.result_data