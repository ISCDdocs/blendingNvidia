/*

*/

#include "nv-control-warpblend.h"
#include "nv-control-screen.h"
#include <stdlib.h>
#include <sys/utsname.h>
#include <math.h>

typedef struct __attribute__((packed)) {
  float x, y;
} vertex2f;

typedef struct __attribute__((packed)) {
  vertex2f pos;
  vertex2f tex;
  vertex2f tex2;
} vertexDataRec;


static void print_display_name(Display *dpy, int target_id, int attr, char *name){
  Bool ret;
  char *str;

  ret = XNVCTRLQueryTargetStringAttribute(dpy,
    NV_CTRL_TARGET_TYPE_DISPLAY,
    target_id, 0,
    attr,
    &str);
  if (!ret) {
    printf("%1s : N/A", name);
    return;
  }

  printf("%1s: %s", name, str);
  XFree(str);
}


int main(int ac, char **av){

  Display *xDpy = XOpenDisplay(NULL);
  int i, gpu, disp;
  int *pDisplayData;

  //Get connected display devices on the present X screen
  Bool ret;
  int len;
  int screenId=0;
  GC gc;
  XGCValues values;
  Pixmap blendPixmap;
  int nvDpyId;
  srand(time(NULL));

  //For each GPU, get corresponding DPYS
  for(gpu=0;gpu<2;gpu++){
    ret = XNVCTRLQueryTargetBinaryData(xDpy,
      NV_CTRL_TARGET_TYPE_GPU,
      gpu,
      0,
      NV_CTRL_BINARY_DATA_DISPLAYS_CONNECTED_TO_GPU,
      (unsigned char **) &pDisplayData,
      &len);

    if (!ret) {
      fprintf(stderr, "Failed to query connected displays\n");
      return 1;
    }

    char name_str[64];


    for (disp = 1; disp <= pDisplayData[0]; disp++) {
      nvDpyId = pDisplayData[disp];
      printf("GPU = %d, ID = %d",gpu,disp);
      //print_display_name(xDpy, nvDpyId, NV_CTRL_STRING_DISPLAY_DEVICE_NAME,name_str);
      printf("\n");

      int W = 1920;
      int H = 1200;
      blendPixmap = XCreatePixmap(xDpy, RootWindow(xDpy, screenId), W, H, DefaultDepth(xDpy, screenId));

      //Zone totale de 256 pixels de large blending
      float gamma        = 2.00f;//2.025
      float ratio, correctedRatio;
      int color;
      int start, end;

      //Left
      if( (gpu==1 && disp==1) || (gpu==0 && disp==3) ){
        start = 1524 - 128;
        end = 1524 + 128;
        //White left
        values.foreground = 0xffffffff;
        gc = XCreateGC(xDpy, blendPixmap, GCForeground, &values);
        XFillRectangle(xDpy, blendPixmap, gc, 0, 0, start, 1200);

        //Blending
        for(i = start ; i < end ; i++){
          ratio = (float)(i - start)/255.0f;
          correctedRatio = 1-pow(ratio,(float)(gamma));
          color = (int)(correctedRatio * 255);
          values.foreground = color*0x010101;
          XChangeGC(xDpy, gc, GCForeground, &values);
          XFillRectangle(xDpy, blendPixmap, gc, i, 0, i+1, H);
        }

        //Black right
        values.foreground = 0x00000000;
        gc = XCreateGC(xDpy, blendPixmap, GCForeground, &values);
        XFillRectangle(xDpy, blendPixmap, gc, end, 0, 1920, 1200);
      }
      //Right
      if( (gpu==1 && disp==2  ) || (gpu==0 && disp==2) ){
        //if(gpu == 1){
        start = 1920 - 1524 - 128;
        end = 1920 - 1524 + 128;
        //Black left
        values.foreground = 0x000000;
        gc = XCreateGC(xDpy, blendPixmap, GCForeground, &values);
        XFillRectangle(xDpy, blendPixmap, gc, 0, 0, start, 1200);

        //Blending
        for(i = start ; i < end ; i++){
          ratio = 1.0f - (float)(i - start)/255.0f;
          correctedRatio = pow(ratio,(float)(gamma));
          color = (int)(correctedRatio * 255);
          //printf("%f - %f - %lu\n", ratio, correctedRatio, color*0x010101);
          values.foreground = 0xffffffff - color*0x010101;
          XChangeGC(xDpy, gc, GCForeground, &values);
          XFillRectangle(xDpy, blendPixmap, gc, i, 0, i+1, H);
        }

        //White right
        values.foreground = 0xffffff;
        gc = XCreateGC(xDpy, blendPixmap, GCForeground, &values);
        XFillRectangle(xDpy, blendPixmap, gc, end, 0, 1920, 1200);
      }

      //Set the intensity of the blend pixmaps
      if( (gpu==1 && disp==2) || (gpu==0 && disp==3) || (gpu==1 && disp==1) || (gpu==0 && disp==2) ){
        XNVCTRLSetScanoutIntensity(xDpy,
          screenId,
          nvDpyId,
          blendPixmap,
          True);
      }
    }

    XFree(pDisplayData);
  }
  return 0;
}
