import zmq
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:5556")
socket.send(b"/memexdata/Dataset/processed/0001_Backpage/Images/corpus/ImagesTexas/Texas_2012_10_10_1349841918000_4_2.jpg" + "\0")
message = socket.recv()
print message
