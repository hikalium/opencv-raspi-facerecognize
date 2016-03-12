#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <unistd.h>
#include "RaspiCamCV.h"

#define CASCADE_PATH "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml"
#define XRES			640
#define YRES			480
#define FONT_SCALE		0.5
#define FONT_LINE_WIDTH	1
#define WINDOW_NAME		"FaceRecogTest"
#define FRECT_MIN		200
#define FRECT_MAX		400

int main(int argc, char *argv[ ]){
	int i;
	RASPIVID_CONFIG * config = (RASPIVID_CONFIG*)malloc(sizeof(RASPIVID_CONFIG));
	CvFont font;
    RaspiCamCvCapture *capture; 
	int exit =0;
	
	config->width=XRES;
	config->height=YRES;
	config->bitrate=0;	// zero: leave as default
	config->framerate=0;
	config->monochrome=0;

    capture = (RaspiCamCvCapture *) raspiCamCvCreateCameraCapture2(0, config); 
	free(config);

	CvHaarClassifierCascade *cvHCC = (CvHaarClassifierCascade *)cvLoad(CASCADE_PATH, NULL, NULL, NULL);
	CvMemStorage *cvMem = cvCreateMemStorage(0);
	CvSeq *face;
	
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, FONT_SCALE, FONT_SCALE, 0, FONT_LINE_WIDTH, 8);

	cvNamedWindow(WINDOW_NAME, 1);
	do {
		IplImage* image = raspiCamCvQueryFrame(capture);
		//face = cvHaarDetectObjects(image, cvHCC, cvMem, 1.1, 3, 0, cvSize(0, 0), cvSize(0, 0));
		face = cvHaarDetectObjects(
			image, cvHCC, cvMem,
			1.2, 2, CV_HAAR_DO_CANNY_PRUNING, 
			cvSize(FRECT_MIN, FRECT_MIN), cvSize(FRECT_MAX, FRECT_MAX));
		//
		for(i = 0; i < face->total; i++){
			CvRect *faceRect = (CvRect *)cvGetSeqElem(face, i);
			cvRectangle(image,
				cvPoint(faceRect->x, faceRect->y),
				cvPoint(faceRect->x + faceRect->width, faceRect->y + faceRect->height),
				CV_RGB(255, 0, 0), 2, CV_AA, 0);
		}
		//
		char text[200];
		sprintf(
			text
			, "d=%d"
			,face->total
			//, "w=%.0f h=%.0f fps=%.0f bitrate=%.0f monochrome=%.0f"
			//, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_FRAME_WIDTH)
			//, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_FRAME_HEIGHT)
			//, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_FPS)
			//, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_BITRATE)
			//, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_MONOCHROME)
		);
		cvPutText (image, text, cvPoint(05, 40), &font, cvScalar(255, 255, 0, 0));
		
		sprintf(text, "Press ESC to exit");
		cvPutText (image, text, cvPoint(05, 80), &font, cvScalar(255, 255, 0, 0));
		
		cvShowImage(WINDOW_NAME, image);
		
		char key = cvWaitKey(10);
		
		switch(key)	
		{
			case 27:		// Esc to exit
				exit = 1;
				break;
			case 60:		// < (less than)
				raspiCamCvSetCaptureProperty(capture, RPI_CAP_PROP_FPS, 25);	// Currently NOOP
				break;
			case 62:		// > (greater than)
				raspiCamCvSetCaptureProperty(capture, RPI_CAP_PROP_FPS, 30);	// Currently NOOP
				break;
		}
		
	} while (!exit);

	cvDestroyWindow(WINDOW_NAME);
	cvReleaseMemStorage(&cvMem);
	cvReleaseHaarClassifierCascade(&cvHCC);
	raspiCamCvReleaseCapture(&capture);
	return 0;
}
