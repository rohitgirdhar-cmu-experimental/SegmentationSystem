/**
 * Code to compute CNN (ImageNet) features for a given image using CAFFE
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

  fs::path LOC_NETWORK_PATH = fs::path(vm["loc-network-path"].as<string>());
  fs::path LOC_MODEL_PATH = 
    fs::path(vm["loc-model-path"].as<string>());
  fs::path SEG_NETWORK_PATH = fs::path(vm["seg-network-path"].as<string>());
  fs::path SEG_MODEL_PATH = 
    fs::path(vm["seg-model-path"].as<string>());

  Caffe::set_phase(Caffe::TEST);
  Net<float> loc_caffe_test_net(LOC_NETWORK_PATH.string());
  loc_caffe_test_net.CopyTrainedLayersFrom(LOC_MODEL_PATH.string());
  Net<float> seg_caffe_test_net(SEG_NETWORK_PATH.string());
  seg_caffe_test_net.CopyTrainedLayersFrom(SEG_MODEL_PATH.string());


  system("cp /home/rgirdhar/memexdata/Dataset/processed/0001_Backpage/Images/corpus/ImagesTexas/Texas_2012_10_10_1349841918000_4_2.jpg temp-dir/img.jpg");
  vector<Blob<float>*> dummy_blob_input_vec;
  loc_caffe_test_net.Forward(dummy_blob_input_vec);
  boost::shared_ptr<Blob<float> > bboxs = loc_caffe_test_net.blob_by_name("fc8_loc");
  ofstream of("temp-dir/locResult.txt");
  of << "temp-dir/img.jpg ";
  for (int i = 0; i < 4; i++) {
    of << bboxs->data_at(0, i, 0, 0) << " ";
  }
  of << endl;
  of.close();

  {
  seg_caffe_test_net.Forward(dummy_blob_input_vec);
  boost::shared_ptr<Blob<float> > output = seg_caffe_test_net.blob_by_name("fc8_seg");
  Mat seg(DIM, DIM, CV_32FC1);
  for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
      seg.at<float>(i, j) = (float) output->data_at(0, i * DIM + j, 0, 0);
    }
  }
  Mat seg2;
  seg.convertTo(seg2, CV_8UC1);
  equalizeHist(seg2, seg2);
  imwrite("temp.jpg", seg2);
  }
 
  system("cp /home/rgirdhar/memexdata/Dataset/processed/0001_Backpage/Images/corpus/ImagesTexas/Texas_2012_10_10_1349841918000_4_0.jpg temp-dir/img.jpg");
  loc_caffe_test_net.Forward(dummy_blob_input_vec);
  bboxs = loc_caffe_test_net.blob_by_name("fc8_loc");
  ofstream of2("temp-dir/locResult.txt");
  of2 << "temp-dir/img.jpg ";
  for (int i = 0; i < 4; i++) {
    of2 << bboxs->data_at(0, i, 0, 0) << " ";
  }
  of2 << endl;
  of2.close();
  
  {
  seg_caffe_test_net.Forward(dummy_blob_input_vec);
  boost::shared_ptr<Blob<float> > output = seg_caffe_test_net.blob_by_name("fc8_seg");
  Mat seg(DIM, DIM, CV_32FC1);
  for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
      seg.at<float>(i, j) = (float) output->data_at(0, i * DIM + j, 0, 0);
    }
  }
  Mat seg2;
  seg.convertTo(seg2, CV_8UC1);
  equalizeHist(seg2, seg2);
  imwrite("temp.jpg", seg2);
  }

  return 0;
}

void genSegImg(float xmin, float ymin, float xmax, float ymax, const Mat& S, Mat& res,
    int NW_IMG_HT, int NW_IMG_WID) {
  res = Mat(NW_IMG_HT, NW_IMG_WID, CV_32FC1);
  res.setTo(0);
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

