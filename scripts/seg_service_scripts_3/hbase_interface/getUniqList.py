
# this script is not being used currently, as I simply scan through a complete table
import happybase as hb
import zmq
import base64
import time
import sys
import os

#### CONFIG
hbase_url = '10.1.94.57'
hbase_table = 'image_cache'
outfpath = '/home/rgirdhar/memexdata/Dataset/processed/0004_IST/lists/Uniq_lists/from_image_cache.txt'
###########

last_print_time = -1
def tic_toc_print(str):
  global last_print_time
  if time.time() - last_print_time > 1:
    sys.stdout.write(str)
    last_print_time = time.time()

# set up hbase
conn = hb.Connection(hbase_url)
tab = conn.table(hbase_table)

fout = open(outfpath, 'w')

for key,data in tab.scan(columns=['image']):
  fout.write('%s\n' % data['image:bin'])

fout.close()

