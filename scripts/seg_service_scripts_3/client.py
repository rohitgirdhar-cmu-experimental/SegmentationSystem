import zmq
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:5561")
socket.send(b"/memexdata/Dataset/processed/0001_Backpage/Images/corpus/ImagesTexas/Texas_2012_10_10_1349841918000_4_0.jpg" + "\0")
#socket.send(b"/memexdata/Dataset/processed/0001_Backpage/Images/corpus/ImagesCalifornia/California_2012_12_12_1355315257000_2_1.jpg" + "\0")
#socket.send(b"/srv2/rgirdhar/Work/Code/0005_ObjSegment/nips14_loc_seg_testonly/Caffe_Segmentation/segscripts/data/corpus/2.jpg" + "\0")
message = socket.recv()
print message
