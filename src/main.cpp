//
//  main.cpp
//  RobustTextDetection
//
//  Created by Saburo Okita on 05/06/14.
//  Copyright (c) 2014 Saburo Okita. All rights reserved.
//

#include <iostream>

#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace std;
using namespace cv;


int main(int argc, const char * argv[])
{
    if(argc < 2){
      printf("usage: %s input_image_name",argv[0]);
      return 0;
    }

    string image_file_path = string(argv[1]);
    namedWindow("detection",WINDOW_AUTOSIZE );
    Mat image = imread(image_file_path);
    Mat grey;
    Mat detect_mat;
    cvtColor( image, grey, CV_BGR2GRAY);
    imshow("origin",image);
    image.copyTo(detect_mat);
    /* Use Tesseract to try to decipher our image */
    tesseract::TessBaseAPI tesseract_api;
    tesseract_api.Init(NULL, "chi_sim"  );
    tesseract_api.SetImage((uchar*) grey.data, grey.cols, grey.rows, 1, grey.cols);


    tesseract::TessBaseAPI* api = &tesseract_api;
    //string out = string(tesseract_api.GetUTF8Text());
    //cout<<out<<endl;
    Boxa* boxes = api->GetComponentImages(tesseract::RIL_SYMBOL, true, NULL, NULL);
    printf("Found %d textline image components.\n", boxes->n);
    for (int i = 0; i < boxes->n; i++) {
      BOX* box = boxaGetBox(boxes, i, L_CLONE);
      api->SetRectangle(box->x, box->y, box->w, box->h);
      char* ocrResult = api->GetUTF8Text();
      int conf = api->MeanTextConf();
      //fprintf(stdout, "Box[%d]: x=%d, y=%d, w=%d, h=%d, confidence: %d, text: %s",
      //                i, box->x, box->y, box->w, box->h, conf, ocrResult);
      Point p1 = Point(box->x, box->y);
      Point p2 = Point(box->x + box->w, box->y + box->h);
      rectangle(detect_mat,p1,p2,Scalar(0,0,255));
    }
    imwrite( "detection_segmentation.jpg",detect_mat);
    imshow("detection",detect_mat);
    waitKey();
    return 0;
}
