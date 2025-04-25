import asyncio
import websockets

async def websocket_client():
    uri = "ws://127.0.0.1:8080/gateway/proxy/10001"
    async with websockets.connect(uri) as websocket:
        message = "helloworld"
        print(f"Sending: {message}")
        await websocket.send(message)
        response = await websocket.recv()
        print(f"Received: {response}")

if __name__ == "__main__":
    asyncio.run(websocket_client())