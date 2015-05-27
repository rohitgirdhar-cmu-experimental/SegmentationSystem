export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../nips14_loc_seg_testonly/Caffe_Segmentation/build/lib/
GLOG_logtostderr=1 ../../services/computeSegmentation.bin -n protos/loc.prototxt -m /srv2/rgirdhar/Work/Code/0005_ObjSegment/nips14_loc_seg_testonly/Caffe_Segmentation/segscripts/models/loc.caffemodel  -p protos/seg.prototxt -q /srv2/rgirdhar/Work/Code/0005_ObjSegment/nips14_loc_seg_testonly/Caffe_Segmentation/segscripts/models/seg.caffemodel 

