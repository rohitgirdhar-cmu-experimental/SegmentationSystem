/**
 * Code to compute CNN (Xiaolong's segmentation network) features for a given image using CAFFE
 * (c) Rohit Girdhar
 */

#include <memory>
#include <chrono>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> // for to_lower
#include "caffe/caffe.hpp"
#include <zmq.h>

using namespace std;
using namespace std::chrono;
using namespace caffe;
using namespace cv;
namespace po = boost::program_options;
namespace fs = boost::filesystem;
#define DIM 50

void genSegImg(float xmin, float ymin, float xmax, float ymax, const Mat& S, Mat& res, int, int);

int
main(int argc, char *argv[]) {
  ::google::InitGoogleLogging(argv[0]);
#ifdef CPU_ONLY
  Caffe::set_mode(Caffe::CPU);
  LOG(INFO) << "Extracting Features in CPU mode";
#else
  Caffe::set_mode(Caffe::GPU);
#endif

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "Show this help")
    ("loc-network-path,n", po::value<string>()->required(),
     "Path to the localization prototxt file")
    ("loc-model-path,m", po::value<string>()->required(),
     "Path to localization caffemodel")
    ("seg-network-path,p", po::value<string>()->required(),
     "Path to the segmentation prototxt file")
    ("seg-model-path,q", po::value<string>()->required(),
     "Path to segmentation caffemodel")
    ("port-num", po::value<string>()->default_value("5556"),
     "Port to run this service on")
    ("temp-dir", po::value<string>()->default_value("temp-dir"),
     "Path to the directory where the images list and the results will be written out. "
     "Make sure each service uses a separate directory or else might mix up results")
    ;

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  if (vm.count("help")) {
    LOG(INFO) << desc;
    return -1;
  }
  try {
    po::notify(vm);
  } catch(po::error& e) {
    LOG(ERROR) << e.what();
    return -1;
  }

  const fs::path LOC_NETWORK_PATH = fs::path(vm["loc-network-path"].as<string>());
  const fs::path LOC_MODEL_PATH = 
    fs::path(vm["loc-model-path"].as<string>());
  const fs::path SEG_NETWORK_PATH = fs::path(vm["seg-network-path"].as<string>());
  const fs::path SEG_MODEL_PATH = 
    fs::path(vm["seg-model-path"].as<string>());
  const string temp_dir_path = vm["temp-dir"].as<string>();

  Caffe::set_phase(Caffe::TEST);
  Net<float> loc_caffe_test_net(LOC_NETWORK_PATH.string());
  loc_caffe_test_net.CopyTrainedLayersFrom(LOC_MODEL_PATH.string());
  Net<float> seg_caffe_test_net(SEG_NETWORK_PATH.string());
  seg_caffe_test_net.CopyTrainedLayersFrom(SEG_MODEL_PATH.string());

  void *context = zmq_ctx_new();
  void *responder = zmq_socket(context, ZMQ_REP);
  int rc = zmq_bind(responder, ("tcp://*:" + vm["port-num"].as<string>()).c_str());
  assert(rc == 0);

  LOG(INFO) << "Server Ready";

  while (true) {
    char buffer[1000], outbuf[1000];
    int len = zmq_recv(responder, buffer, 1000, 0);
    buffer[len] = '\0';
    
    system(("unlink " + temp_dir_path + "/img.jpg").c_str());
    string cmd = string("ln -s ") + buffer + " " + temp_dir_path + "/img.jpg";
    LOG(INFO) << "Running cmd : " << cmd;
    system(cmd.c_str());
    vector<Blob<float>*> dummy_blob_input_vec;
    loc_caffe_test_net.Forward(dummy_blob_input_vec);
    boost::shared_ptr<Blob<float> > bboxs = loc_caffe_test_net.blob_by_name("fc8_loc");
    vector<float> save_box;
    ofstream of((temp_dir_path + "/locResult.txt").c_str());
    of << temp_dir_path + "/img.jpg ";
    for (int i = 0; i < 4; i++) {
      of << bboxs->data_at(0, i, 0, 0) << " ";
      save_box.push_back(bboxs->data_at(0, i, 0, 0));
    }
    of << endl;
    of.close();

    seg_caffe_test_net.Forward(dummy_blob_input_vec);
    boost::shared_ptr<Blob<float> > output = seg_caffe_test_net.blob_by_name("fc8_seg");
    Mat seg(DIM, DIM, CV_32FC1);
    for (int i = 0; i < DIM; i++) {
      for (int j = 0; j < DIM; j++) {
        seg.at<float>(Point2d(j, i)) = (float) output->data_at(0, i * DIM + j, 0, 0);
      }
    }
    Mat seg2 = seg;
    genSegImg(save_box[0], save_box[1], save_box[2], save_box[3],
        seg, seg2, 256, 256);
    Mat seg3;
    seg2.convertTo(seg3, CV_8UC1);
    equalizeHist(seg3, seg3);
    imwrite((temp_dir_path + "/result.jpg").c_str(), seg3);

    //system("unlink temp-dir/img.jpg");
    zmq_send(responder, "done", 5, 0);
  }

  return 0;
}

void genSegImg(float xmin, float ymin, float xmax, float ymax, const Mat& S, Mat& res,
    int NW_IMG_HT, int NW_IMG_WID) {
  float OFFSET = (256 - 227) / 2.0;
  res = Mat(NW_IMG_HT, NW_IMG_WID, CV_32FC1);
  res.setTo(0);
  xmin = std::min(std::max(xmin + OFFSET, (float) 0), (float) NW_IMG_WID-1);
  ymin = std::min(std::max(ymin + OFFSET, (float) 0), (float) NW_IMG_HT-1);
  xmax = std::min(std::max(xmax + OFFSET, (float) 0), (float) NW_IMG_WID-1);
  ymax = std::min(std::max(ymax + OFFSET, (float) 0), (float) NW_IMG_HT-1);
  int x1 = xmin;
  int y1 = ymin;
  int x2 = xmax;
  int y2 = ymax;
  int height = MAX(1, y2 - y1 + 1);
  int width = MAX(1, x2 - x1 + 1);
  Mat extractedImage = res(Rect(x1, y1, width, height));
  Mat S2;
  resize(S, S2, Size(width, height));
  S2.copyTo(extractedImage);
}

