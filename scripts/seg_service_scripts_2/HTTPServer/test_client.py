#!/usr/bin/env python
import sys
import urllib2
import urllib
import base64
import json

from threading import Thread

def make_request(url):
#data = "http://10.3.2.61/~ubuntu/projects/001_backpage/dataset/corpus/ImagesTexas/Texas_2012_10_10_1349853753000_5_1.jpg"
    data = "http://10.1.94.128:8000/~rgirdhar/memex/dataset/0001_Backpage/Images/ImagesTexas/Texas_2012_10_10_1349841918000_4_0.jpg"
#    data = "http://10.1.94.128:8000/~rgirdhar/memex/dataset/0001_Backpage/Images/ImagesCalifornia/California_2014_10_5_1412515911000_10_6.jpg"
#    data = "http://aws.tj-tool.com/media/images/2015/05/18/Minnesota_2014_10_14_1413313669000_10_4.jpg"
#    data = "http://10.1.94.128:8000/~rgirdhar/memex/dataset/0001_Backpage/Images/ImagesTexas/Texas_2012_10_10_1349915757000_3_0.jpg"
    #data = "http://10.3.2.61/~ubuntu/projects/001_backpage/dataset/corpus/ImagesNevada/Nevada_2012_10_2_1349231418000_8_5.jpg"
    headers = { 'Content-type' : 'text',  'Content-length' : str(len(data))}
    req = urllib2.Request(url, data, headers) #POST request
    try:
      response = urllib2.urlopen(req)
      result = response.read()
      result = json.loads(result)
      seg = base64.b64decode(result[0][1])
      f = open('seg.jpg', 'wb')
      f.write(seg)
      f.close()
    except urllib2.URLError, err:
      print err

def main():
    port = 8890
    try:
      make_request("http://10.1.94.128:%d" % port)
    except urllib2.HTTPError, err:
      print err

main()
