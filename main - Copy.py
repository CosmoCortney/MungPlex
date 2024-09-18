import asyncio
import websockets
import random

class Room:
    def __init__(self, password):
        self.password = password
        self.room_id = self.generate_room_id1()
        self.id = self.generate_room_id(self.room_id)
        self.connections = []  
        self.host = None  # Keep track of the host

    def generate_room_id1(self):
        return random.randint(1000, 9999)

    def generate_room_id(self, intw):
        """Generate a unique room ID in the format 'Game#XXXX'."""
        return f"Game#{intw}"

    def add_connection(self, websocket):
        """Add a WebSocket connection to the room."""
        if websocket not in self.connections:
            self.connections.append(websocket)
            if len(self.connections) == 1:  # The first connection is the host
                self.host = websocket
        print(self.host)

    def remove_connection(self, websocket):
        """Remove a WebSocket connection from the room."""
        if websocket in self.connections:
            self.connections.remove(websocket)

        # If the host disconnects or if there are no players left, disband the room
        if websocket == self.host:
            print(f"Host disconnected, disbanding room {self.id}")
            del rooms[self.id]  # Disband the room by removing it from the dictionary
        elif not self.connections:
            print(f"Room {self.id} is empty, disbanding...")
            del rooms[self.id]  # Disband the room if no one is left

    def __repr__(self):
        return f"Room(ID={self.id}, Password={self.password}, Connections={len(self.connections)}, Host={self.host})"

rooms = {}

async def handle_message(websocket, message):
    """Process incoming messages and handle room creation or joining based on the first byte."""
    if not message:
        return

    # Ensure the message is a bytes object
    if isinstance(message, str):
        message = message.encode('utf-8')

    print(message[0])
    if len(message) < 1:
        await websocket.send(b"Error: Message too short")
        return

    # Room creation (0x0C)
    if message[0] == 0x0C:  # Check if the first byte is 0x0C (12 in hexadecimal) for room creation
        if len(message) < 2:
            await websocket.send(b"Error: Message too short for room creation")
            return
        
        room_password = message[1:].decode('utf-8')
        room = Room(room_password)
        rooms[room.id] = room
        print(f"Room created: {room}")
        await websocket.send(f"Room created with ID: {room.id} and Password: {room_password}".encode('utf-8'))
        room.add_connection(websocket)
        response = bytearray([0x0C])
        response.extend(str(room.room_id).encode('utf-8'))  # Include the game ID in the response
        await websocket.send(response)

    # Room joining (0x0B)
    elif message[0] == 0x0B:  # Check if the first byte is 0x0B (11 in hexadecimal) for joining a room
        if len(message) < 5:
            await websocket.send(b"Error: Message too short to join a room")
            return

        # Extract the next 4 bytes for the game ID
        game_id = message[1:5].decode("utf-8")
        
        # Extract the rest of the message as the password (decode to string)
        password = message[5:].decode('utf-8')

        room_id = f"Game#{game_id}"
        print(f"Trying to join room: {room_id}, with password: {password}")
        
        if room_id in rooms:
            room = rooms[room_id]
            if room.password == password:
                room.add_connection(websocket)
                print(f"Joined room: {room}")
                response = bytearray([0x0B])
                await websocket.send(response)
            else:
                await websocket.send(b"Error: Incorrect password")
        else:
            await websocket.send(b"Error: Room does not exist")
    elif message[0] == 0x09:
        for room in list(rooms.values()):
            if websocket in room.connections:
                room.remove_connection(websocket)
                await websocket.send(bytearray([0x09]))
    elif message[0] == 0x08:
        for room in list(rooms.values()):
            if websocket in room.connections:
                room.remove_connection(websocket)
                await websocket.send(bytearray([0x0A]))
    else:
        await websocket.send(b"Error: Invalid message format")
    
async def echo(websocket, path):
    try:
        await websocket.send("Welcome to the WebSocket server!".encode('utf-8'))

        while True:
            try:
                # Wait for the next message, with a timeout of 6 seconds
                message = await asyncio.wait_for(websocket.recv(), timeout=6.0)
                await handle_message(websocket, message)
            except asyncio.TimeoutError:
                print("Timeout, disconnecting client due to inactivity.")
                await websocket.close()  # Close the connection on timeout
                break
    except websockets.ConnectionClosed as e:
        # Remove the connection from all rooms upon disconnection
        for room in list(rooms.values()):
            if websocket in room.connections:
                room.remove_connection(websocket)
        print(f"Connection closed: {e}")

start_server = websockets.serve(echo, "localhost", 8765)
ro1om = Room("AAA")
rooms[ro1om.id] = ro1om
print(ro1om)
asyncio.get_event_loop().run_until_complete(start_server)
print("WebSocket server started on ws://localhost:8765")
asyncio.get_event_loop().run_forever()
