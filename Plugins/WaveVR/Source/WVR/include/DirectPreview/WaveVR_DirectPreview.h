#pragma once
typedef enum {
	DP_InitError_None = 0,
	DP_InitError_WSAStartUp_Failed = 1,
	DP_InitError_Already_Inited = 2,
	DP_InitError_Device_Not_Found = 3,
	DP_InitError_Can_Not_Connect_Server = 4,
	DP_InitError_IPAddress_Null = 5,
} DP_InitError;

typedef enum {
	DP_ConnectType_USB,
	DP_ConnectType_WiFi,
	DP_ConnectType_Other
} DP_ConnectType;

/**
 * @brief Init the DirectPreview DLL.
 *
 * @param type which indicate the connection type for this initialization.
 * @param IP which indicate the IP address if connection type is WiFi
 * @param enablePreview which indicate if we will render images to Direct Preview DLL.
 * @param enablePrintInfo which indicate if we need more information printed in console log.
 * @param saveImage which indicate if we need to save render images to disk regulary. **NOTE:** Save images to disk will decrease performance, set this parameter to true only in debug.
 * @return enum DP_InitError to know what error occurs
 */
extern "C" __declspec(dllexport) DP_InitError WVR_Init_S(DP_ConnectType type, const char* IP, bool enablePreview, bool enablePrintInfo, bool saveImage);

/**
 * @brief Quit the DirectPreview DLL.
 *
 * Close and release all the resource and connection allocated form Direct Preview DLL.
 */
extern "C" __declspec(dllexport) void WVR_Quit_S();

/**
 * @brief Returns poses to usage for rendering scene.
 *
 * This function gets the poses from Direct Preview DLL for the usage of rendering scene at the specific moment based on the pose origin model.
 *
 * The poses and device type are aggregated together as struct WVR_DevicePosePair_t.
 * The struct pointers of all tracked devices compose a specific size array to keep track of the locomotions of the devices.
 * The specific array size can be substituted with three levels of device count.
 * **WVR_DEVICE_COUNT_LEVEL_0** is only assumed to get HMD device.
 * **WVR_DEVICE_COUNT_LEVEL_1** is assumed that the HMD and two controllers are tracked.
 *
 * @param originModel enum @ref WVR_PoseOriginModel, specify the tracking universe of origin pose model.
 * @param pairArray struct pointer WVR_DevicePosePair_t (refer to @ref WVR_DevicePosePair), aggregation array to keep pose information and tracked device type.
 * @param pairArrayCount uint32_t, specify the size of WVR_DevicePosePair_t array. The three levels of device count are optional.
*/
extern "C" __declspec(dllexport) void WVR_GetSyncPose_S(WVR_PoseOriginModel originModel, WVR_DevicePosePair_t* retPose, uint32_t PoseCount);

/**
 * @brief Function to get button state of specific input id
 *
 * @param type which indicates what device type. (refer to @ref WVR_DeviceType)
 * @param id, one of @ref WVR_InputId, indicate which button
 * @return bool, true for pressed and false is unpressed
*/
extern "C" __declspec(dllexport) bool WVR_GetInputButtonState_S(WVR_DeviceType type, WVR_InputId id);

/**
 * @brief Function to get touch state of specific input id
 *
 * @param type which indicates what device type. (refer to @ref WVR_DeviceType)
 * @param id, one of @ref WVR_InputId, indicate which touch
 * @return bool, true for touched and false is untouched
*/
extern "C" __declspec(dllexport) bool WVR_GetInputTouchState_S(WVR_DeviceType type, WVR_InputId id);

/**
 * @brief Function to get analog data of specific input id
 *
 * @param type which indicates what device type. (refer to @ref WVR_DeviceType)
 * @param id, one of @ref WVR_InputId, indicate which touch
 * @return analog data for @ref WVR_InputId. (refer to @ref WVR_Axis)
*/
extern "C" __declspec(dllexport) WVR_Axis_t WVR_GetInputAnalogAxis_S(WVR_DeviceType type, WVR_InputId id);


/**
 * @brief Function to get the count of specified button type
 *
 * @param type which indicates what device type. (refer to @ref WVR_DeviceType)
 * @param inputType which indicates input type for the input id.
 * @return int, the count of specified button type.
*/
extern "C" __declspec(dllexport) int WVR_GetInputTypeCount_S(WVR_DeviceType type, WVR_InputType inputType);

/**
 * @brief Function to get the count of specified button type
 *
 * @return bool, true for touched and false
*/
extern "C" __declspec(dllexport) bool WVR_GetInputDeviceState_S(WVR_DeviceType type, uint32_t inputMask, uint32_t* buttons, uint32_t* touches,
			WVR_AnalogState_t* analogArray, uint32_t analogArrayCount);

/**
 * @brief Developer can know if WVR_DeviceType is connected
 *
 * @param type which indicate @ref WVR_DeviceType.
 * @return false if is disconnected, true is connected.
 */
extern "C" __declspec(dllexport) bool WVR_IsDeviceConnected_S(WVR_DeviceType type);

/**
 * @brief Function to get a parameter string from Direct Preview DLL.
 *
 * @param type which indicate @ref WVR_DeviceType.
 * @param pchValue send string to let AP communicate to Direct Preview DLL. **NOTE:** We only support GetRenderModelName now.
 * @param retValue The buffer to store parameter get from Direct Preview DLL. unBufferSize should be the size of this buffer.
 * @param unBufferSize The size of the buffer pointed to by pchValue.
 * @return The number of bytes necessary to hold the string, including the trailing null. Returns 0 (and 0-length string) on failure.
*/
extern "C" __declspec(dllexport) uint32_t WVR_GetParameters_S(WVR_DeviceType type, const char* pchValue, char* retValue, uint32_t unBufferSize);

/**
 * @brief Function to get the Degree of Freedom (DoF) of device's pose.
 *
 * @param type which indicate @ref WVR_DeviceType.
 * @return WVR_NumDoF_3DoF means the device provides 3 DoF pose (only rotation), WVR_NumDoF_6DoF means the device provides 6 DoF pose (rotation and position). (refer to @ref WVR_NumDoF)
*/
extern "C" __declspec(dllexport) WVR_NumDoF WVR_GetDegreeOfFreedom_S(WVR_DeviceType type);

/**
 * @brief Get current battery capacity of specified device with percentage.
 * @param type Type of the device. (refer to @ref WVR_DeviceType)
 * @return The percentage of battery of specified device.
 */
extern "C" __declspec(dllexport) float WVR_GetDeviceBatteryPercentage_S(WVR_DeviceType type);

/**
 * @brief Function to get next event in event queue

 * @param event a struct of Event information
 * @return true means fill event struct with the next event in the queue, false means no event in the queue
 */
extern "C" __declspec(dllexport) bool WVR_PollEventQueue_S(WVR_Event_t* event);

/**
 * @brief Function to get the suggested size of render target.
 *
 * This API provides render target size to fit display.
 *
 * @param width suggested width for the offscreen render target
 * @param height suggested height for the offscreen render target
 */
extern "C" __declspec(dllexport) void WVR_GetRenderTargetSize_S(uint32_t* width, uint32_t* height);

/**
 * @brief Function to get the boundary of the clipping plane.
 *
 * Returns the coordinate of the margin of clipping plane for the specified eye.
 * The necessary information can be used to compose a custom matrix, though most contents should use @ref WVR_GetProjection
 * instead of this method, but sometimes a content needs to do something fancy with its projection and can use
 * these values to compute one.
 *
 * @param eye determines which eye the function should return for corresponding clipping plane.
 * @param left coordinate for the left margin of the clipping plane.
 * @param right coordinate for the right margin of the clipping plane.
 * @param top coordinate for the top margin of the clipping plane.
 * @param bottom coordinate for the bottom margin of the clipping plane.
 */
extern "C" __declspec(dllexport) void WVR_GetClippingPlaneBoundary_S(WVR_Eye eye, float* left, float* right, float* top, float* bottom);

/**
 * @brief Function to return the transform from eye space to the head space.
 *
 * Eye space is the per-eye flavor of view space that provides stereo disparity.
 * Instead of Model * View * Projection the model is Model * View * Eye * Projection.
 * Normally View and Eye will be multiplied together and treated as View in your application.
 *
 * This matrix incorporates the user's interpupillary distance (IPD).
 *
 * @param eye Determines which eye the function should return the eye matrix for.
 * @param dof Specify the DoF of current content. 6DoF transform considers the depth of eye to head but 3DoF not.
 * @return The transform between the view space and eye space.
*/
extern "C" __declspec(dllexport) WVR_Matrix4f_t WVR_GetTransformFromEyeToHead_S(WVR_Eye eye, WVR_NumDoF dof);

/**
* @brief Requests the properties from Direct Preview DLL.
*
* The interface fetch necessary properties from Direct Preview DLL.
* The output argument WVR_RenderProps_t* carries the frame rate and the existing status of the external display.
* The corresponding member value can be access via dereferencing the interface parameter referring to @ref WVR_RenderProps_t.
* Return value is the validness of properties. If it is not true, it means the fetched value is not available.
* Please note that some platforms return false rather than fetching render properties.
*
* @param props pointer of struct WVR_RenderProps_t, to aggregate the properties fetched from render runtime.
* @retval true Success to get the render properties.
* @retval false Fail to get the render properties, the fetched properties value is invalid.
*/
extern "C" __declspec(dllexport) bool WVR_GetRenderProps_S(WVR_RenderProps_t* props);

/**
* @brief The prototype of print callback.
*
* The callback is provided by AP and reveal debug message from Direct Preview DLL.
*
* @param str the debug message provided by Direct Preview DLL.
*/
typedef void(__stdcall *PrintLog)(const char* str);

/**
* @brief Set print callback to Direct Preview DLL.
*
* @param callback the instance of PrintLog.
*/
extern "C" __declspec(dllexport) void WVR_SetPrintCallback(PrintLog callback);

/**
* @brief Set array of render image's native handle (or resource) to Direct Preview DLL.
*
* This API will provide an array to Direct Preview DLL, this array contains 2 render image's native handles (or resources).
* After Direct Preview DLL received those render image's native handles (or resources), it will start to encode/streaming and send to target device to render.
* @param rtHandleLR the array of render image's native handle (or resource). **NOTE:** Array size should be 2. rtHandleLR[0] is left eye, rtHandleLR[1] is right eye.
*/
extern "C" __declspec(dllexport) bool WVR_SetRenderImageHandles(void* rtHandleLR[]);

/**
* @brief Set render image's native handle (or resource) to Direct Preview DLL.
*
* This API will provide render image's native handle (or resource). This native handle (or resource) must have 2 eyes' view (left eye's view must be placed on left size of image, and right eye's view must be placed on right side of image)
* After Direct Preview DLL received those render image's native handle (or resource), it will split this image to left eye image and right eye image, and will start to encode/streaming/send to target device to render.
* @param rtHandle the render image's native handle (or resource).
*/
extern "C" __declspec(dllexport) bool WVR_SetRenderImageHandle(void* rtHandle);

/**
 * @brief Function to set controller pose mode to adjust the controller ray.
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType). This should be the controller type.
 * @param mode Indicates what controller pose mode (refer to @ref WVR_ControllerPoseMode).
 * @return True means assigned mode enabled successfully; False means assigned mode is unsupported or the same as default mode (WVR_ControllerPoseMode_Raw).
 */
extern "C" __declspec(dllexport) bool WVR_SetControllerPoseMode_S(WVR_DeviceType type, WVR_ControllerPoseMode mode);

/**
 * @brief Function to get current enabled controller pose mode.
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType). This should be the controller type.
 * @param mode Returns the current enabled controller pose mode.
 * @return True means mode obtained successfully; False means mode obtained unsuccessfully.
 */
extern "C" __declspec(dllexport) bool WVR_GetControllerPoseMode_S(WVR_DeviceType type, WVR_ControllerPoseMode *mode);

extern "C" __declspec(dllexport) void WVR_TriggerVibrationScale_S(WVR_DeviceType type, WVR_InputId id, uint32_t durationMicroSec, uint32_t frequency, float amplitude);
extern "C" __declspec(dllexport) void WVR_TriggerVibration_S(WVR_DeviceType type, WVR_InputId id, uint32_t durationMicroSec, uint32_t frequency, WVR_Intensity intensity);
extern "C" __declspec(dllexport) unsigned long WVR_GetSupportedFeatures_S();
extern "C" __declspec(dllexport) WVR_Result WVR_StartHandGesture_S(unsigned long demands);
extern "C" __declspec(dllexport) void WVR_StopHandGesture_S();
extern "C" __declspec(dllexport) WVR_Result WVR_GetHandGestureInfo_S(WVR_HandGestureInfo_t *info);
extern "C" __declspec(dllexport) WVR_Result WVR_GetHandGestureData_S(WVR_HandGestureData_t *data);
extern "C" __declspec(dllexport) WVR_Result WVR_StartHandTracking_S(WVR_HandTrackerType type);
extern "C" __declspec(dllexport) void WVR_StopHandTracking_S(WVR_HandTrackerType type);
extern "C" __declspec(dllexport) WVR_Result WVR_GetHandJointCount_S(WVR_HandTrackerType type, uint32_t *jointCount);
extern "C" __declspec(dllexport) WVR_Result WVR_GetHandTrackerInfo_S(WVR_HandTrackerType type, WVR_HandTrackerInfo_t *info);
//extern "C" __declspec(dllexport) WVR_Result WVR_GetHandTrackingData_S(WVR_HandTrackerType trackerType, WVR_HandModelType modelType, WVR_PoseOriginModel originModel, WVR_HandTrackingData_t *handTrackerData, WVR_HandPoseData_t *pose);
extern "C" __declspec(dllexport) bool WVR_ControllerSupportElectronicHand_S();
extern "C" __declspec(dllexport) void WVR_EnhanceHandStable_S(bool wear);
extern "C" __declspec(dllexport) bool WVR_IsEnhanceHandStable_S();
extern "C" __declspec(dllexport) WVR_InteractionMode WVR_GetInteractionMode_S();

/**
 * @brief Function to get the offset data of current enabled controller pose mode.
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType). This should be the controller type.
 * @param mode Indicates what controller pose mode (refer to @ref WVR_ControllerPoseMode).
 * @param translation Returns the translation offset of assigned mode.
 * @param quaternion Returns the quaternion offset of assigned mode.
 * @return True means offset obtained successfully; False means offset obtained unsuccessfully (Device type is not controller or not connected.).
 */
extern "C" __declspec(dllexport) bool WVR_GetControllerPoseModeOffset_S(WVR_DeviceType type, WVR_ControllerPoseMode mode, WVR_Vector3f_t* translation, WVR_Quatf_t* quaternion);
/**
 * @brief Use this function to get the controller model for the currently connected device. If the controller model does not exist,
 *        it will return the default model in the SDK.
 *
 * Note: It is not recommended to call this function in the render thread or any thread that will block frame update.
 *
 * @param ctrlerType Device type: WVR_DeviceType_Controller_Right or WVR_DeviceType_Controller_Left.(refer to @ref WVR_DeviceType)
 * @param ctrlerModel (Output) A double pointer for receiving the controller model structure.
 *                    The SDK will allocate the data structure for the controller model and write the address into ctrlerModel.
 * @retval WVR_SUCCESS The controller model was successfully retrieved from the currently connected device.
 * @retval WVR_Error_CtrlerModel_InvalidModel The SDK can't get the controller model information from the connected controller device or asset.
 * @retval WVR_Error_CtrlerModel_DeviceDisconnected The SDK can't get the controller model information since the controller you want to get is not connected.
 * @retval WVR_Error_CtrlerModel_Unknown The SDK can't get the controller model information since the system has not yet been initialized.
 * @retval WVR_Error_InvalidArgument If ctrlerModel is nullptr or pointer in *ctrlerModel isn't nullptr.
 * @version API Level 5
*/
extern "C" __declspec(dllexport) WVR_Result WVR_GetCurrentControllerModel_S(WVR_DeviceType ctrlerType, WVR_CtrlerModel_t **ctrlerModel, bool isOneBone = true);

/**
 * @brief Use this function to deallocate the memory used for the controller model.
 *
 * Delete the controller received from WVR_GetCurrentControllerModel.
 *
 * @param ctrlerModel An allocated object about controller model. The object will be destroyed and the structure cleared(ctrlerModel will be set nullptr).
 * @version API Level 5
*/
extern "C" __declspec(dllexport) void WVR_ReleaseControllerModel_S(WVR_CtrlerModel_t **ctrlerModel);
