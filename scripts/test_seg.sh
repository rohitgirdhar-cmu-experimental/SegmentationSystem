#!/usr/bin/env sh
CAFFEROOT=/srv2/rgirdhar/Work/Code/0005_ObjSegment/nips14_loc_seg_testonly/Caffe_Segmentation/
SEGSCRDIR=$CAFFEROOT/segscripts/
GLOG_logtostderr=1  $CAFFEROOT/build/tools/test_net_seg_hdf5stor.bin  \
    seg_test.prototxt  \
    $SEGSCRDIR/models/seg.caffemodel \
    /home/rgirdhar/memexdata/Dataset/processed/0001_Backpage/Images/lists/Images.txt \
    /home/rgirdhar/memexdata/Dataset/processed/0001_Backpage/Features/Segment/segResults

