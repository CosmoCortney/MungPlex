import asyncio
import websockets
import random
import struct

class Room:
    def __init__(self, password):
        self.password = password
        self.id = self.generate_room_id()
        self.connections = []  

    def generate_room_id(self):
        """Generate a unique room ID in the format 'Game#XXXX'."""
        return f"Game#{random.randint(1000, 9999)}"

    def add_connection(self, websocket):
        """Add a WebSocket connection to the room."""
        if websocket not in self.connections:
            self.connections.append(websocket)

    def remove_connection(self, websocket):
        """Remove a WebSocket connection from the room."""
        if websocket in self.connections:
            self.connections.remove(websocket)

    def __repr__(self):
        return f"Room(ID={self.id}, Password={self.password}, Connections={len(self.connections)})"

rooms = {}

async def handle_message(websocket, message):
    """Process incoming messages and handle room creation or joining based on the first byte."""
    if not message:
        return

    # Ensure the message is a bytes object
    if isinstance(message, str):
        message = message.encode('utf-8')

    if len(message) < 1:
        await websocket.send(b"Error: Message too short")
        return

    if message.startswith(bytes([0x0C])):  # Check if the first byte is 0x0C (12 in hexadecimal) for room creation
        if len(message) < 2:
            await websocket.send(b"Error: Message too short for room creation")
            return
        
        room_password = message[1:].decode('utf-8')
        room = Room(room_password)
        rooms[room.id] = room
        print(f"Room created: {room}")
        await websocket.send(f"Room created with ID: {room.id} and Password: {room_password}".encode('utf-8'))
        room.add_connection(websocket)
    elif message.startswith(bytes([0x0B])):  # Check if the first byte is 0x0B (11 in hexadecimal) for joining a room
        if len(message) < 5:
            await websocket.send(b"Error: Message too short to join a room")
            return
        
        # Extract the next 4 bytes for the game ID
        game_id = struct.unpack('>I', message[1:5])[0]  # Big-endian 4-byte unsigned int
        # Extract the rest of the message as the password
        password = message[5:].decode('utf-8')

        room_id = f"Game#{game_id}"
        if room_id in rooms:
            room = rooms[room_id]
            if room.password == password:
                room.add_connection(websocket)
                print(f"Joined room: {room}")
                await websocket.send(f"Joined room with ID: {room.id}".encode('utf-8'))
            else:
                await websocket.send(b"Error: Incorrect password")
        else:
            await websocket.send(b"Error: Room does not exist")
    else:
        await websocket.send(b"Error: Invalid message format")

async def echo(websocket, path):
    try:
        await websocket.send("Welcome to the WebSocket server!".encode('utf-8'))

        async for message in websocket:
            print(f"Received message: {message}")
            await handle_message(websocket, message)
    except websockets.ConnectionClosed as e:
        print(f"Connection closed: {e}")

start_server = websockets.serve(echo, "localhost", 8765)

asyncio.get_event_loop().run_until_complete(start_server)
print("WebSocket server started on ws://localhost:8765")
asyncio.get_event_loop().run_forever()
