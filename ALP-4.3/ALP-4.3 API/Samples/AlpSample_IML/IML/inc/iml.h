#ifndef __IMLLIBRARY
#define __IMLLIBRARY

#ifdef IML_EXPORTS
    #define IML_API extern "C" __declspec(dllexport)
#else
    #define IML_API extern "C" __declspec(dllimport)
#endif

// Source File Definements
#define IML_ZOOM_MIN        0.01f
#define IML_ZOOM_MAX        100.f 

// Position Flags
#define IML_ALIGN_DEFAULT   0
#define IML_ALIGN_LEFT      1<<1
#define IML_ALIGN_HCENTER   1<<2
#define IML_ALIGN_RIGHT     1<<3
#define IML_ALIGN_TOP       1<<4
#define IML_ALIGN_VCENTER   1<<5
#define IML_ALIGN_BOTTOM    1<<6

// Zoom
#define IML_SCALE           1<<7 
#define IML_ZOOM            1<<8

// Interpolation Mode
#define IML_INTERPOLATION_DEFAULT              0
#define IML_INTERPOLATION_NEAREST_NEIGHBOR     1<<9
#define IML_INTERPOLATION_BILINEAR             1<<10
#define IML_INTERPOLATION_BICUBIC              1<<11


//////////////////////////////////////////////////////////////////////////////
// error codes
#define IML_SUCCESS                          0
#define IML_ERR                             -1
#define IML_ERR_BAD_PARAMS                  IML_ERR - 1
#define IML_ERR_BAD_MEMORY_PTR              IML_ERR - 2
#define IML_ERR_OUT_OF_MEMORY               IML_ERR - 3
#define IML_ERR_LOAD_IMAGE                  IML_ERR - 4

#define IML_ERR_WIN                         IML_ERR - 1000
#define IML_ERR_WIN_INIT                    IML_ERR WIN -1


// IMLSetImageSize
// Param:
//		AlpWidth			- number of pixel columns on DMD
//		AlpHeight			- number of pixel rows on DMD
// Return:
//		IML_SUCCESS or IML_ERR_BAD_PARAMS
// Examples:
//		IMLSetImageSize(1024,  768);	// for XGA
//		IMLSetImageSize(1920, 1080);	// for 1080p
//		IMLSetImageSize(1920, 1200);	// for WUXGA

IML_API long IMLSetImageSize(
		unsigned short AlpWidth,
		unsigned short AlpHeight);

// IMLGetImageSize
// Return:
//		number of bytes that IMLLoadImage requires in pImgBuffer
IML_API unsigned long IMLGetImageSize();

// IMLGetImageWidth
// Return:
//		number of pixel columns on DMD
IML_API unsigned short IMLGetImageWidth();

// IMLGetImageHeight
// Return:
//		number of pixel rows on DMD
IML_API unsigned short IMLGetImageHeight();

// IMLLoadImage
// Param: 
//         strFile           - filename of image to load (UNICODE character string)
//         lFlags            - alignment
//                             IML_ALIGN_DEFAULT:   top left corner (first row ,first column)
//                             IML_ALIGN_LEFT:      left
//                             IML_ALIGN_HCENTER:   horizontal center
//                             IML_ALIGN_RIGHT:     right
//                             IML_ALIGN_TOP:       top
//                             IML_ALIGN_VCENTER:   vertical center
//                             IML_ALIGN_BOTTOM:    bottom
//
//                             IML_SCALE:           scale image (fits in whole buffer)
//                                                  !!! changes aspect ratio !!!
//                             IML_ZOOM:            zoom image
//                           - interpolation mode
//                             IML_INTERPOLATION_NEAREST_NEIGHBOR
//                             IML_INTERPOLATION_BILINEAR
//                             IML_INTERPOLATION_BICUBIC
//         fZoom             - zoom factor, valid value [IML_ZOOM_MIN,IML_ZOOM_MAX]
//         pImgBuffer        - ptr to buffer of size AlpWidth * AlpHeight
// Return: 
//         IML_SUCCESS:  OK
//         else       :  an error occurs
// Desc:   
//         - loads an image in previously allocated memory (pImgBuffer)
//         - pBuffer must point to a memory area of size 1024x768 bytes (ALP_SIZE)
//         - strImgFileName must be 0-terminated UNICODE character string

IML_API long IMLLoadImageW(
        LPCWSTR     strFile,
        long        lFlags,
        float       fZoom,
        void       *pImgBuffer);

// single byte char version 
IML_API long IMLLoadImageA(
        LPCSTR      strFile,
        long        lFlags,
        float       fZoom,
        void       *pImgBuffer);

#ifdef UNICODE
    #define IMLLoadImage  IMLLoadImageW
#else
    #define IMLLoadImage  IMLLoadImageA
#endif


#endif