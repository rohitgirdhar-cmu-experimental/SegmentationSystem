import happybase as hb
import zmq
import base64
import time
import sys
import os

#### CONFIG
hbase_url = '10.1.94.57'
hbase_table = 'image_cache'
temp_img_path = './temp-dir/img.jpg'
seg_service_port = 5561
output_hbase_table = 'rgirdhar-CMU-segmentation'
seg_fpath = '../temp-dir/result.jpg'
loc_fpath = '../temp-dir/locResult.txt'
bin_image_key_name = 'image:binary'
START_ID = '00103C5B9425A40D806D2D1D7D0D2AF8CEF4D303'
###########

last_print_time = -1
def tic_toc_print(str):
  global last_print_time
  if time.time() - last_print_time > 1:
    sys.stdout.write(str)
    last_print_time = time.time()

def refineBox(bbox):
  OFFSET = (256 -227) / 2
  return [str(float(el) + OFFSET) for el in bbox]

# set up hbase
conn = hb.Connection(hbase_url)
tab = conn.table(hbase_table)
out_tab = conn.table(output_hbase_table)
# set up zmq
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect('tcp://localhost:%d' % seg_service_port)

total_time_elap = 0
nitem = 0
for key,data in tab.scan(START_ID, batch_size=300):
  start_time = time.time()
  # get image
  #raw_img = base64.b64decode(data['image:orig'])
  if bin_image_key_name not in data:
    continue
  raw_img = data[bin_image_key_name]
  f = open(temp_img_path, 'wb')
  f.write(raw_img)
  f.close()
  if len(raw_img) == 0:
    continue
  # run segmentation
  socket.send(os.path.realpath(temp_img_path) + '\0')
  socket.recv()
  f = open(seg_fpath)
  seg_output = base64.b64encode(f.read())
  f.close()
  f = open(loc_fpath)
  loc_output = refineBox(f.read().strip().split(' ')[1:])
  f.close()
  #writing it out
  out_tab.put(key, {'seg:orig' : seg_output, 
      'loc:xmin' : loc_output[0],
      'loc:ymin' : loc_output[1],
      'loc:xmax' : loc_output[2],
      'loc:ymax' : loc_output[3]})

  end_time = time.time()
  time_elap = end_time - start_time
  total_time_elap += time_elap
  nitem += 1
  tic_toc_print('Done till %d (key=%s). Elapsed %f sec\n Avg time %f sec\n' % (nitem, key, time_elap, total_time_elap / nitem))

