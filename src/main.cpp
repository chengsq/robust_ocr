//
//  main.cpp
//  author: shiqing CHENG
// email: chengsq123@gmail.com
//

#include <iostream>

#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace std;
using namespace cv;

float bbOverlap(const BOX& box1,const BOX& box2)
{
    if (box1.x > box2.x+box2.w) { return 0.0; }
    if (box1.y > box2.y+box2.h) { return 0.0; }
    if (box1.x+box1.w < box2.x) { return 0.0; }
    if (box1.y+box1.h < box2.y) { return 0.0; }
    float colInt =  min(box1.x+box1.w,box2.x+box2.w) - max(box1.x, box2.x);
    float rowInt =  min(box1.y+box1.h,box2.y+box2.h) - max(box1.y,box2.y);
    float overlapArea = colInt * rowInt; //相交处面积
    float area1 = box1.w*box1.h;
    float area2 = box2.w*box2.h;
    return overlapArea / min(area1, area2); //相交处的比例
}

int  ProcessConfig(const char* language,
  tesseract::TessBaseAPI* api){
  api->Init(NULL, language);
  printf("Load language model: %s\n", api->GetInitLanguagesAsString());

  //api->SetPageSegMode(tesseract::PageSegMode::PSM_AUTO);

  // set some variables
  int iv;
  bool bv;
  // set int variable
  api->SetVariable("edges_max_children_per_outline", "30");
  api->GetIntVariable("edges_max_children_per_outline", &iv);
  printf("set edges_max_children_per_outline: %d\n", iv);
#if 1
  api->SetVariable("enable_new_segsearch", "T");
  api->GetBoolVariable("enable_new_segsearch", &bv);
  printf("enable_new_segsearch: %d\n", bv);

  api->SetVariable("language_model_ngram_on", "T");
  api->GetBoolVariable("language_model_ngram_on", &bv);
  printf("language_model_ngram_on: %d\n", bv);

  // set bool variable
  api->SetVariable("textord_force_make_prop_words", "F");
  api->GetBoolVariable("textord_force_make_prop_words", &bv);
  printf("set textord_force_make_prop_words: %d\n", bv);

  api->SetVariable("chop_enable", "T");
  api->GetBoolVariable("chop_enable", &bv);
  printf("chop_enable: %d\n", bv);

  api->SetVariable("use_new_state_cost", "T");
  api->GetBoolVariable("use_new_state_cost", &bv);
  printf("use_new_state_cost: %d\n", bv);
  //tessedit_char_whitelist A0123456789
  //api->SetVariable("tessedit_char_whitelist","01234567890");
  api->SetVariable("segment_segcost_rating", "F");
  api->GetBoolVariable("segment_segcost_rating", &bv);
  printf("segment_segcost_rating: %d\n", bv);

#endif
return 0;
}

int ProcessRegcognition(Mat image,const char* language,char* result){
  tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();;
  ProcessConfig(language,api);
  Mat grey_img,detect_mat;
  cvtColor(image, grey_img, CV_BGR2GRAY);
  imshow("origin", image);
  image.copyTo(detect_mat);
  api->SetImage((uchar*)grey_img.data, grey_img.cols, grey_img.rows, 1, grey_img.cols);
  api->Recognize(0);

  char* ocr_result = api->GetUTF8Text();
  printf("%s\n",ocr_result);
  strcpy(result,ocr_result);
  #if 0
  Boxa* boxes = api->GetComponentImages(tesseract::RIL_SYMBOL, true, NULL, NULL);
  // printf("find lines: %d", api->FindLines());
  // bounding box
  printf("Found %d textline image components.\n", boxes->n);
  for (int i = 0; i < boxes->n; i++) {
    BOX* box = boxaGetBox(boxes, i, L_CLONE);
    api->SetRectangle(box->x, box->y, box->w, box->h);
    int conf = api->MeanTextConf();
    char *ocrResult = api->GetUTF8Text();
    printf("Box[%d]: x=%d, y=%d, w=%d, h=%d, confidence: %d, text: %s", i, box->x, box->y, box->w, box->h, conf, ocrResult);
    Point p1 = Point(box->x, box->y);
    Point p2 = Point(box->x + box->w, box->y + box->h);
    rectangle(detect_mat, p1, p2, Scalar(0, 0, 255));
    #if 0
    Mat roi_img;
    char sub_image_name[128];
    sprintf(sub_image_name, "./sub/%d.jpg", i);
    roi_img = image(Range(box->y, box->y+box->h),Range(box->x, box->x+box->w));
    imwrite(sub_image_name, roi_img);
    //printf ("extract sub img: %s", sub_image_name);
    #endif
  }
  #endif
  delete api;
  imwrite("detection_segmentation.jpg", detect_mat);
  imshow("detection", detect_mat);
  return 1;
}

int main(int argc, const char* argv[]) {
  if (argc < 3) {
    printf("usage: %s input_image_name  langguage", argv[0]);
    return 0;
  }

  string image_file_path = string(argv[1]);
  const char *language = argv[2];
  namedWindow("detection", WINDOW_AUTOSIZE);
  Mat image = imread(image_file_path);
  char* string_result = new char[1024];
  ProcessRegcognition(image,language,string_result);
  printf("%s\n",string_result);
  waitKey();
  return 0;
}
