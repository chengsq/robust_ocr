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

int main(int argc, const char* argv[]) {
  if (argc < 2) {
    printf("usage: %s input_image_name", argv[0]);
    return 0;
  }

  string image_file_path = string(argv[1]);
  namedWindow("detection", WINDOW_AUTOSIZE);
  Mat image = imread(image_file_path);
  Mat grey;
  Mat detect_mat;
  cvtColor(image, grey, CV_BGR2GRAY);
  imshow("origin", image);
  image.copyTo(detect_mat);
  /* Use Tesseract to try to decipher our image */
  tesseract::TessBaseAPI tesseract_api;
  tesseract::TessBaseAPI* api = &tesseract_api;

  api->Init(NULL, "eng");
  printf("Load language model: %s\n", api->GetInitLanguagesAsString());


  //api->SetPageSegMode(tesseract::PageSegMode::PSM_AUTO);

  // set some variables
  int iv;
  bool bv;
  // set int variable
  api->SetVariable("edges_max_children_per_outline", "40");
  api->GetIntVariable("edges_max_children_per_outline", &iv);
  printf("set edges_max_children_per_outline: %d\n", iv);
#if 1
  api->SetVariable("enable_new_segsearch", "F");
  api->GetBoolVariable("enable_new_segsearch", &bv);
  printf("enable_new_segsearch: %d\n", bv);

  api->SetVariable("language_model_ngram_on", "F");
  api->GetBoolVariable("language_model_ngram_on", &bv);
  printf("language_model_ngram_on: %d\n", bv);

  // set bool variable
  api->SetVariable("textord_force_make_prop_words", "F");
  api->GetBoolVariable("textord_force_make_prop_words", &bv);
  printf("set textord_force_make_prop_words: %d\n", bv);

  api->SetVariable("chop_enable", "T");
  api->GetBoolVariable("chop_enable", &bv);
  printf("chop_enable: %d\n", bv);

  api->SetVariable("use_new_state_cost", "F");
  api->GetBoolVariable("use_new_state_cost", &bv);
  printf("use_new_state_cost: %d\n", bv);
  //tessedit_char_whitelist A0123456789
  //api->SetVariable("tessedit_char_whitelist","01234567890");
  api->SetVariable("segment_segcost_rating", "F");
  api->GetBoolVariable("segment_segcost_rating", &bv);
  printf("segment_segcost_rating: %d\n", bv);

#endif
  //tesseract_api.Init(NULL, "chi_sim");
  tesseract_api.SetImage((uchar*)grey.data, grey.cols, grey.rows, 1, grey.cols);
  api->Recognize(0);
  for (int i = 0; i < 1; ++i)
  {
    char * ocrResult = api->GetUTF8Text();
    printf("%s\n",ocrResult);
  }
  #if 0
  Boxa* boxes = api->GetComponentImages(tesseract::RIL_SYMBOL, true, NULL, NULL);
  // printf("find lines: %d", api->FindLines());


  // bounding box
  printf("Found %d textline image components.\n", boxes->n);
  for (int i = 0; i < boxes->n; i++) {
    BOX* box = boxaGetBox(boxes, i, L_CLONE);
    api->SetRectangle(box->x, box->y, box->w, box->h);
    // int conf = api->MeanTextConf();
    // char *ocrResult = api->GetUTF8Text();
    // fprintf(stdout, "Box[%d]: x=%d, y=%d, w=%d, h=%d, confidence: %d, text:
    // %s",
    //                i, box->x, box->y, box->w, box->h, conf, ocrResult);
    Point p1 = Point(box->x, box->y);
    Point p2 = Point(box->x + box->w, box->y + box->h);

    /*if (i != boxes->n-1)
    {
      BOX* b11 = boxaGetBox(boxes, i+1, L_CLONE);
      // api->SetRectangle(b11->x, b11->y, b11->w, b11->h);
      // Point p11 = Point(b11->x, b11->y);
      // Point p22 = Point(b11->x + b11->w, b11->y + b11->h);

      if (bbOverlap(*box, *b11) > 0.1)
      {
        box->x = min(box->x, b11->x);
        box->y = min(box->y, b11->y);
        box->w = max(box->x, b11->x) - box->x;
        box->h = max(box->y, b11->y) - box->y;

        p1 = Point(box->x, box->y);
        p2 = Point(box->x + box->w, box->y + box->h);
        i++;
      }
    }*/

    rectangle(detect_mat, p1, p2, Scalar(0, 0, 255));

    Mat roi_img;
    char sub_image_name[128];
    sprintf(sub_image_name, "./sub/%d.jpg", i);
    roi_img = image(Range(box->y, box->y+box->h),Range(box->x, box->x+box->w));
    imwrite(sub_image_name, roi_img);
    //printf ("extract sub img: %s", sub_image_name);
  }
  #endif
  imwrite("detection_segmentation.jpg", detect_mat);
  imshow("detection", detect_mat);
  waitKey();
  return 0;
}
