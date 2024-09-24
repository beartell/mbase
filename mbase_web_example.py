from flask import Flask
from flask import render_template
from flask import request
from flask_socketio import SocketIO
from maip_client import MaipClient
from maip_operation_object import MaipOperationResult
import uuid
import datetime
import sqlite3
import json

app = Flask(__name__)
socketio = SocketIO(app)

# CLIENT CONTEXT MAP
clientContextMap = {}

# ERRORS LIST:
operationSuccessful = {"message" : "Success", "code" : 0}
errUnauthorizedClient = {"message" : "Unauthoized access", "code" : 1}
errUnregisteredClient = {"message" : "Unregistered client, clear the cookies", "code" : 2}
errInvalidCreateParams = {"message": "Invalid creation params", "code" : 3}
errIntegerConstrainFail = {"message" : "Integer value constrain is not satisfied", "code" : 4}
errContextLengthFail = {"message" : "Context length too short", "code" : 5}
errDbConnectionFail = {"message" : "DB Connection fail", "code" : 6}
errNoAvailableContext = {"message" : "No available context", "code" : 7}
errMaipConnectionFail = {"message" : "Failed to establish connection with MAIP host", "code" : 8}
errMaipPortFormatError = {"message" : "Invalid port format", "code" : 9}
errModelQueryError = {"message" : "Models couldn't be queried, make sure to check your MAIP host is setup correctly", "code" : 10}
errMaipMissingParams = {"message" : "MAIP parameters are missing", "code" : 10}
errContextNotFound = {"message" : "Context not found", "code" : 11}
errContextInitializeFail = {"message" : "Failed to initialize context on MAIP host.", "code" : 12}
errUnknownRole = {"message" : "Unknown message role", "code" : 13}
errMissingMessage = {"message" : "Missing message", "code" : 14}

# STATUS LIST
serverStatusRunning = 0
serverStatusFailedConnection = 1
serverStatusCantInitialize = 2
serverStatusModelNotSupported = 3

# --- Let there be light ---
userTableCreationString = """
CREATE TABLE IF NOT EXISTS users(mbase_id TEXT PRIMARY KEY);
"""

contextTableCreationString = """
    CREATE TABLE IF NOT EXISTS contexts(
    mb_id TEXT,
    ctx_id TEXT,
    ctx_length INTEGER,
    ctx_remember TINYINT,
    maip_host TEXT,
    maip_port INTEGER,
    selected_model TEXT,
    system_prompt TEXT,
    FOREIGN KEY(mb_id) REFERENCES users(mbase_id)
    );
"""

chatHistoryTableCreationString = """
    CREATE TABLE IF NOT EXISTS chat_history(
        chat_id TEXT,
        user_id TEXT,
        msg_role TEXT,
        msg_content TEXT,
        msg_index INTEGER,
        FOREIGN KEY(chat_id) REFERENCES contexts(ctx_id)
    );
"""
# --- Let there be light ---
with sqlite3.connect("mbase_web_program.db") as dbConnection:
    dbConnection.execute(userTableCreationString)
    dbConnection.execute(contextTableCreationString)
    dbConnection.execute(chatHistoryTableCreationString)
    dbConnection.commit()

@app.route("/")
def main_page():
    if request.cookies.get("mb-id") is None:
        expire_date = datetime.datetime.now()
        expire_date = expire_date + datetime.timedelta(days=9999)
        resp = app.make_response(render_template("index.html"))
        generatedUUID = str(uuid.uuid4())
        resp.set_cookie("mb-id", generatedUUID, expires=expire_date)
        with sqlite3.connect("mbase_web_program.db") as dbConnection:
            dbConnection.execute("INSERT INTO users(mbase_id) VALUES(:mb_uuid)", {"mb_uuid" : generatedUUID})
            dbConnection.commit()
        return resp
    return render_template("index.html")

@app.route("/chat/create", methods=["POST"])
def chat_creation_request():
    mbase_id = request.cookies.get("mb-id")
    if mbase_id is None:
        return {"status" : errUnauthorizedClient}

    maip_host = request.args.get("maip_host")
    maip_port = request.args.get("maip_port")
    selected_model = request.args.get("selected_model")
    context_length = request.args.get("ctx_length")
    context_remember = request.args.get("ctx_remember")
    system_prompt = request.args.get("system_prompt")
  
    if maip_host is None or maip_port is None or selected_model is None or context_length is None or context_remember is None or system_prompt is None:
        return {"status": errInvalidCreateParams}

    try:
        maip_port = int(maip_port)
        context_length = int(context_length)
        pass
    except ValueError:
        return {"status": errIntegerConstrainFail}
    
    if context_length < 32:
        return {"status" : errContextLengthFail}

    int_context_remember = 0

    if context_remember == "true":
        int_context_remember = 1
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("SELECT * FROM users WHERE mbase_id=?", (mbase_id,))
        if resultCursor.fetchone() is None:
            return {"status" : errUnregisteredClient}
        insertionString = """
        INSERT INTO contexts(
        mb_id,
        ctx_id,
        ctx_length,
        ctx_remember,
        maip_host,
        maip_port,
        selected_model,
        system_prompt
        )
        VALUES(?, ?, ?, ?, ?, ?, ?, ?)
        """
        
        newChatId = str(uuid.uuid4())
        dbConnection.execute(insertionString, (mbase_id, newChatId, context_length, int_context_remember, maip_host, maip_port, selected_model, system_prompt))
        dbConnection.commit()
        return {"status" : operationSuccessful, "chat_id" : newChatId}
    return {"status" : errDbConnectionFail}

@app.route("/chat/append")
def chat_append():
    mbase_id = request.cookies.get("mb-id")

    if mbase_id is None:
        return {"status" : errUnauthorizedClient}
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("SELECT * FROM users WHERE mbase_id=?", (mbase_id,))
        if resultCursor.fetchone() is None:
            return {"status" : errUnregisteredClient}
    
    chat_id = request.args.get("chat_id")
    message_role = request.args.get("msg_role")
    msg_content = request.args.get("message")

    if message_role != "Assistant" and message_role != "User":
        return {"status" : errUnknownRole}

    if len(msg_content) == 0:
        return {"status" : errMissingMessage}
    
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("INSERT INTO chat_history(chat_id, user_id, msg_role, msg_content, msg_index) VALUES(?, ?, ?, ?, (SELECT IFNULL(MAX(msg_index), 0) + 1 FROM chat_history WHERE chat_id=?))", (chat_id, mbase_id, message_role, msg_content, chat_id))
    return {"status" : operationSuccessful}

@app.route("/chat/get_chat_history/<string:chat_id>")
def get_chat_history(chat_id):
    mbase_id = request.cookies.get("mb-id")

    if mbase_id is None:
        return {"status" : errUnauthorizedClient}
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("SELECT * FROM users WHERE mbase_id=?", (mbase_id,))
        if resultCursor.fetchone() is None:
            return {"status" : errUnregisteredClient}
        resultCursor = dbConnection.execute("SELECT msg_role, msg_content, msg_index FROM chat_history WHERE user_id=? AND chat_id=? ORDER BY msg_index ASC", (mbase_id, chat_id))
        fetchResult = resultCursor.fetchall()
        messageList = []
        for i in fetchResult:
            msgRole = i[0]
            msgContent = i[1]
            msgIndex = i[2]
            historyInfo = {
                "msg_role" : msgRole,
                "msg_content" : msgContent,
                "msg_index" : msgIndex
            }
            messageList.append(historyInfo)
        return {"status" : operationSuccessful, "message_history" : messageList}
    return {"status" : errDbConnectionFail}


@app.route("/chat/get_chats")
def get_chats():
    statusModel = {"message" : "Unauthorized access", "code" : 0}
    mbase_id = request.cookies.get("mb-id")
    if mbase_id is None:
        return {"status" : statusModel}
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("SELECT * FROM users WHERE mbase_id=?", (mbase_id,))
        if resultCursor.fetchone() is None:
            statusModel["message"] = "Client is not registered"
            return {"status" : statusModel}
        resultCursor = dbConnection.execute("SELECT ctx_id, ctx_length, ctx_remember, maip_host, maip_port, selected_model, system_prompt FROM contexts WHERE mb_id=?", (mbase_id,))
        fetchResult = resultCursor.fetchall()
        if fetchResult is None:
            # MEANS NO CONTEXT IS AVAILABLE
            return {"status" : errContextNotFound}
        chatList = []
        for i in fetchResult:
            contextId = i[0]
            contextLength = i[1]
            contextRemember = i[2]
            maipHost = i[3]
            maipPort = i[4]
            selectedModel = i[5]
            systemPrompt = i[6]
            chat_info = {
                "context_id" : contextId,
                "context_length" : contextLength,
                "context_remember" : contextRemember,
                "maip_host" : maipHost,
                "maip_port" : maipPort,
                "selected_model" : selectedModel,
                "system_prompt" : systemPrompt
            }
            chatList.append(chat_info)
        return {"status" : operationSuccessful, "chat_list" : chatList}
    return {"status" : errDbConnectionFail}

@app.route("/chat/get_chat_info/<string:chat_id>")
def get_chat_info(chat_id : str):
    mbase_id = request.cookies.get("mb-id")
    if mbase_id is None:
        return {"status" : errUnauthorizedClient}
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("SELECT * FROM users WHERE mbase_id=?", (mbase_id,))
        if resultCursor.fetchone() is None:
            return {"status" : errUnregisteredClient}
        resultCursor = dbConnection.execute("SELECT ctx_id, ctx_length, ctx_remember, maip_host, maip_port, selected_model, system_prompt FROM contexts WHERE mb_id=? AND ctx_id=?", (mbase_id, chat_id))
        fetchResult = resultCursor.fetchone()
        if fetchResult is None:
            # MEANS NO CONTEXT IS AVAILABLE
            # prepare error status
            return {"status" : errContextNotFound}
        contextId = fetchResult[0]
        contextLength = fetchResult[1]
        contextRemember = fetchResult[2]
        maipHost = fetchResult[3]
        maipPort = fetchResult[4]
        selectedModel = fetchResult[5]
        systemPrompt = fetchResult[6]
        return {"status" : operationSuccessful, "chat_info" : {
            "context_id" : contextId,
            "context_length" : contextLength,
            "context_remember" : contextRemember,
            "maip_host" : maipHost,
            "maip_port" : maipPort,
            "selected_model" : selectedModel,
            "system_prompt" : systemPrompt
        }}
    return {"status" : errDbConnectionFail}

@app.route("/maip/get_models")
def get_models():
    mbase_id = request.cookies.get("mb-id")
    if mbase_id is None:
        return {"status": errUnauthorizedClient}
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("SELECT * FROM users WHERE mbase_id=?", (mbase_id,))
        if resultCursor.fetchone() is None:
            return {"status" : errUnregisteredClient}
    maip_host = request.args.get("maip_host")
    maip_port = request.args.get("maip_port")
    if maip_host is None or maip_port is None:
        return {"status": errMaipMissingParams}
    try:
        maip_port = int(maip_port)
        pass
    except ValueError:
        return {"status": errMaipPortFormatError}
    maipClient = MaipClient(maip_host, maip_port)
    if maipClient.is_connection_active() == False:
        return {"status" : errMaipConnectionFail}
    operationResult : MaipOperationResult = maipClient.create_client()
    if operationResult.get_result_code() == 2000:
        modelQueryResult : MaipOperationResult = maipClient.get_program_models()
        if operationResult.get_result_code() == 2000:
            return {"status" : operationSuccessful, "models" : modelQueryResult.get_result_data()}
        else:
            return {"status" : errModelQueryError, "maip_status_code" : operationResult.get_result_code()}
    return {"status": errModelQueryError, "maip_status_code" : operationResult.get_result_code()}

@app.route("/maip/server_status")
def server_status():
    mbase_id = request.cookies.get("mb-id")
    if mbase_id is None:
        return {"status": errUnauthorizedClient}
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("SELECT * FROM users WHERE mbase_id=?", (mbase_id,))
        if resultCursor.fetchone() is None:
            return {"status" : errUnregisteredClient}
    maip_host = request.args.get("maip_host")
    maip_port = request.args.get("maip_port")
    selected_model = request.args.get("selected_model")
    if maip_host is None or maip_port is None:
        return {"status": errMaipMissingParams}
    try:
        maip_port = int(maip_port)
        pass
    except ValueError:
        return {"status": errMaipPortFormatError}
    
    maipClient = MaipClient(maip_host, maip_port)
    if maipClient.is_connection_active() == False:
        # cant connect to server
        return {"status" : operationSuccessful, "server_status" : serverStatusFailedConnection}
    operationResult : MaipOperationResult = maipClient.create_client()
    if operationResult.get_result_code() != 2000:
        # cant initialize client
        return {"status" : operationSuccessful, "server_status" : serverStatusCantInitialize}
    operationResult = maipClient.get_program_models()
    if operationResult.get_result_code() != 2000:
        # cant initialize client
        return {"status" : operationSuccessful, "server_status" : serverStatusCantInitialize}
    for i in operationResult.get_result_data():
        if selected_model == i:
            # things are successful
            return {"status" : operationSuccessful, "server_status" : serverStatusRunning}
    
    # model is not supported
    return {"status" : operationSuccessful, "server_status" : serverStatusModelNotSupported, "models" : operationResult.get_result_data()}

@app.route("/maip/initialize_context")
def initialize_context():
    mbase_id = request.cookies.get("mb-id")

    if mbase_id is None:
        return {"status" : errUnauthorizedClient}
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("SELECT * FROM users WHERE mbase_id=?", (mbase_id,))
        if resultCursor.fetchone() is None:
            return {"status" : errUnregisteredClient}
     
    maip_host = request.args.get("maip_host")
    maip_port = request.args.get("maip_port")
    selected_model = request.args.get("selected_model")
    context_length = request.args.get("ctx_length")
    context_remember = request.args.get("ctx_remember")
    request.get_json(force=True)
    if maip_host is None or maip_port is None or selected_model is None or context_length is None or context_remember is None:
        return {"status": errInvalidCreateParams}

    try:
        maip_port = int(maip_port)
        context_length = int(context_length)
        pass
    except ValueError:
        return {"status": errIntegerConstrainFail}
    
    if context_length < 32:
        return {"status" : errContextLengthFail}

    if clientContextMap.get(mbase_id) is None:
        pass
    else:
        # delete the context
        pass
    
    maipClient : MaipClient = MaipClient(maip_host, maip_port)
    if maipClient.is_connection_active() is False:
        return {"status" : errMaipConnectionFail} 
    
    myResult : MaipOperationResult = maipClient.create_context(selected_model, context_length)
    if myResult.get_result_code() != 2016 or myResult.get_result_code() != 2000:
        return {"status" : errContextInitializeFail}
    
    clientContextMap.setdefault(mbase_id, {"context_handle" : myResult.get_result_data(), "msg_ids" : []})
    return {"status" : operationSuccessful}

@app.route("/maip/create_message_id")
def create_message_id():
    mbase_id = request.cookies.get("mb-id")
    if mbase_id is None:
        return {"status" : errUnauthorizedClient}
    with sqlite3.connect("mbase_web_program.db") as dbConnection:
        resultCursor = dbConnection.execute("SELECT * FROM users WHERE mbase_id=?", (mbase_id,))
        if resultCursor.fetchone() is None:
            return {"status" : errUnregisteredClient}


@socketio.on('init_context')
def initialize_context(data):
    # maip_host
    # maip_port
    # context length
    # selected model
    
    initContextDict : dict = json.loads(data)
    
    clientContextMap.setdefault(initContextDict.get("mbase-id"), initContextDict)

    # if it is present, delete the active context

@socketio.on('execute_input')
def execute_input(data):
    pass

@socketio.on('get_next')
def get_next(data):
    pass

socketio.run(app=app)
#     myClient = MaipClient()
#     return {"maiphost": uuid.uuid4(), "port" : request.args.get("maip_port")}