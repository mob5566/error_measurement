#ifndef __DIRECT_X_H__
#define __DIRECT_X_H__

///////////////////////////////////////////////////////////////////////////////////////////////////

#define DEVICE_TYPE_UNKNOWN     0
#define DEVICE_TYPE_MSDC        1
#define DEVICE_TYPE_CAMERA      2

#define DEVICE_POWER_OFF        0
#define DEVICE_POWER_ON         1

#define IMAGE_UNKNOWN       0
#define IMAGE_320x240       1   // 4:3 
#define IMAGE_640x480       2   // 4:3 
#define IMAGE_800x600       3   // 4:3 
#define IMAGE_1024x768      4   // 4:3
#define IMAGE_1280x720      5   // 16:9
#define IMAGE_1280x800      6   // 8:5
#define IMAGE_1280x960      7   // 4:3
#define IMAGE_1280x1024     8   // 5:4
#define IMAGE_1600x1200     9   // 4:3 
#define IMAGE_1920x1080     10   // 16:9
#define IMAGE_2048x1536     11   // 4:3

//---------------------------------------


// Application-defined message to notify app of filtergraph events
#define WM_GRAPHNOTIFY      (WM_APP + 1)

#define WM_CAPTURE_BITMAP   (WM_APP + 2)


#ifndef _PLAYSTATE_
#define _PLAYSTATE_
typedef enum _PLAYSTATE {
    psInit = 0,
    psStopped,
    psPaused,
    psRunning
} PLAYSTATE;
#endif

static const GUID PROPSETID_VIDCAP_EXTENSION_UNIT_DC120 = 
{ 0x28f03370, 0x6311, 0x4a2e, { 0xba, 0x2c, 0x68, 0x90, 0xeb, 0x33, 0x40, 0x16 } };

static const GUID PROPSETID_VIDCAP_EXTENSION_UNIT_DC125 = 
{ 0xF679EF5B, 0x54DF, 0x4BFC, { 0xB8, 0xCB, 0xCA, 0x7E, 0x89, 0xA5, 0x23, 0x3F } };

///////////////////////////////////////////////////////////////////////////////////////////////////
//  Global data structure for storing CLSIDs and friendly strings
#define AUDIO_CAPTURE_SOURCES       0
#define AUDIO_COMPRESSORS           1
#define AUDIO_REDERERS              2
#define DIRECTSHOW_FILTERS          3
#define VIDEO_CAPTURE_SOURCES       4
#define VIDEO_COMPRESSORS           5

#pragma pack(1)
typedef struct _CATEGORY_INFORMATION {
    const CLSID *pclsid;
    char szName[128];
} CATEGORY_INFORMATION, *PCATEGORY_INFORMATION;
#pragma pack()

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

HRESULT __stdcall InitializeDirectX(void);
HRESULT __stdcall UninitializeDirectX(void);

HRESULT __stdcall GetCategoryInfo(CATEGORY_INFORMATION *lpCategoryInfo, ULONG *pCategoryInfoNum);
HRESULT __stdcall EnumCategoryFilters(const CLSID *clsid, char *pFilterNameBuffer, ULONG BufferLength);
HRESULT __stdcall ShowCompressCodecConfiguration(HWND hParentWnd, char *pCodecName);
HRESULT __stdcall DetectCaptureDeviceEx(UCHAR *IsDeviceExist);

HRESULT __stdcall InitializePreviewVideo(HWND hMainWnd, HWND hNotifyWnd, RECT *pPreviewRect);
HRESULT __stdcall UninitializePreviewVideo(void);

HRESULT __stdcall StartPreviewVideo(void);
HRESULT __stdcall StopPreviewVideo(void);
HRESULT __stdcall ChangePreviewState(UINT IsIcon, UINT *PlayState);

HRESULT __stdcall GetDirectXObject(void);
HRESULT __stdcall ReleaseDirectXObject(void);

HRESULT __stdcall StartCaptureVideo(char *pAviFileName, char *pCodecName, char *pAudioName);
HRESULT __stdcall StartCaptureVideo_UVC(char *pAviFileName, char *pCodecName, char *pAudioName);
HRESULT __stdcall StopCaptureVideo(void);

HRESULT __stdcall PauseStream(void);

HRESULT __stdcall OnResizeVideoWindow(RECT *pPreviewRect);
HRESULT __stdcall OnHandleGraphEvent(void);

HRESULT __stdcall BlendBitmap(HWND hPreviewWnd, HBITMAP hBmp, int x, int y);

HRESULT __stdcall GetNativeVideoSize(ULONG *pWidth, ULONG *pHeight, ULONG *pResolution);
HRESULT __stdcall SetBorderColor(COLORREF Color);

HRESULT __stdcall ShowCameraPropertyDialog(HWND hParentWnd);

HRESULT __stdcall CaptureStillImageToFile(unsigned char *pgBuffer);
HRESULT __stdcall OnCaptureBitmap(void);

HRESULT __stdcall InitializePreviewVideo_SetStreamFormat(int mIndex, HWND hNotifyWnd);

void __stdcall VideoWindow_Show(void);
void __stdcall VideoWindow_Hide(void);

void __stdcall GetFrameRate(char *fFrameRate);

HRESULT __stdcall QueryFocusRange(long *pValue,long *pMin,long *pMax);
HRESULT __stdcall QueryBrightnessRange(long *pValue,long *pMin,long *pMax);

HRESULT __stdcall SetBrightnessValue(int SetValue);
HRESULT __stdcall SetFocusValue(int SetValue);

HRESULT __stdcall QueryWhiteBalanceRange(long *pValue,long *pMin,long *pMax,long *pDef,long *pFlag); 
HRESULT __stdcall QueryExposureRange(long *pValue,long *pMin,long *pMax,long *pDef,long *pFlag);     

HRESULT __stdcall SetWhiteBalanceValue(int SetValue,int Flag); 
HRESULT __stdcall SetExposureValue(int SetValue,int Flag);     

HRESULT __stdcall GetCurrentResolutionInfo(int *pCurrentIndex);
HRESULT __stdcall GetResolutionInfo(ULONG *pCount,char *pResolution, int nIndex);
HRESULT __stdcall SetStreamFormat(int mIndex);

HRESULT __stdcall WriteToASIC(USHORT Addr, BYTE Value);
HRESULT __stdcall ReadFromASIC(USHORT Addr, BYTE *pValue);


int __stdcall WriteToSensor(BYTE SlaveID, BYTE SensorAddr, USHORT Data, BYTE DataNum);
int __stdcall ReadFromSensor(BYTE SlaveID, BYTE SensorAddr, USHORT *pData, BYTE DataNum);

//****************************************************

void __stdcall GDIDCSize(int Width,int Height);
void __stdcall GDIOverlay_MouseDown(int x,int y);
void __stdcall GDIOverlay_MouseMove(int dStyle,int dColor,int dWidth,int x,int y);
void __stdcall GDIOverlay_MouseUp(int x,int y);
void __stdcall ClearEraserShape(int x,int y);
void __stdcall ClearDC(void);
void __stdcall BackStore(HDC PreviewhDC);  
void __stdcall GDILaser(int dStyle,int dColor,int dWidth,int x,int y);

void __stdcall Recover_Annotation(void);
void __stdcall Recover_Annotation_Resize(int type);

void __stdcall DCtoImage(HDC PreviewhDC); 
void __stdcall ImagetoDC(HDC PreviewhDC);

int __stdcall MSDC_GetDriveName(UCHAR *pDrive);
int __stdcall MSDC_GetInquireData(UCHAR *pDrive, VOID *pBuffer);
int __stdcall MSDC_SwitchToCamera(UCHAR *pDrive);
int __stdcall MSDC_PowerOn(UCHAR *pDrive);
int __stdcall MSDC_PowerOff(UCHAR *pDrive);
int __stdcall MSDC_QueryPower(UCHAR *pDrive, DWORD *pPowerState);

int __stdcall CAM_GetConfigObject(void);
int __stdcall CAM_ReleaseConfigObject(void);
int __stdcall CAM_SwitchToMSDC(void);
int __stdcall CAM_SetPower(ULONG PowerType);
int __stdcall CAM_QueryPower(ULONG *PowerType);

int __stdcall EnumDeviceType(UCHAR *DeviceType);

int __stdcall InitializeMicVolume(HWND hCallback, char *pAudioName);
int __stdcall UninitializeMicVolume(void);
int __stdcall SetMicVolume(ULONG VolumeValue);
int __stdcall GetMicVolume(ULONG *VolumeValue);


int __stdcall OldModel_QueryPower(ULONG *PowerType);
int __stdcall Query_DeviceModel(ULONG *DeviceModel);
int __stdcall OldModel_SetPower(ULONG PowerType);

HRESULT __stdcall RepaintVideo(int pResolution);
HRESULT __stdcall RepaintFullScreenVideo(void);

#ifdef __cplusplus
}
#endif

#endif
