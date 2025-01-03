var WebSocket = require('ws');
var url = require('url');

const clients = [];

class SocketServer {
    ws;

    constructor() {
        this.ws = null;
    }

    broadcast(socket, data) {
        console.log("Connected clients: ", clients.length);
        for (var i = 0; i < clients.length; i++) {
            if (clients[i] != socket) {
                clients[i].send(data);
            }
        }
    }

    broadcastAll(data) {
        for (var i = 0; i < clients.length; i++) {
            clients[i].send(data);
        }
    }

    connectSocket(httpServer) {
        this.ws = new WebSocket.Server({
            server: httpServer
        });

        this.handleEvents()
    }

    handleEvents() {
        const _this = this

        this.ws.on('connection', function(socket, req) {
            clients.push(socket);
        
            socket.on('message', function(message) { 
                _this.broadcast(socket, message);
            });
        
            socket.on('close', function() {
                var index = clients.indexOf(socket);
                clients.splice(index, 1);
                console.log('disconnected');
            });
        });
    }
}

const socketInstance = new SocketServer()

module.exports = socketInstance;