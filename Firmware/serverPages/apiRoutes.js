/// HTML Routes
const Route_SetupFull = '/SetupFull';

/// API Routes GET
const ApiGetPort_GenericRGB = "/api/GetPort/genericRGB";
const ApiGetPort_RGBISync = "/api/GetPort/RGBISync";
const ApiGetPort_RGBWAsync = "/api/GetPort/RGBWAsync";
const ApiGetPort_RGBWSingle = "/api/GetPort/RGBWSingle";
const ApiGetPort_IValues = "/api/GetPort/IValues/";
const ApiGetFiles = "/api/GetFiles";
/// API Routes SET
const ApiSetPort_GenericRGB = "/api/SetPort/genericRGB";
const ApiSetPort_RGBISync = "/api/SetPort/RGBISync";
const ApiSetPort_RGBWAsync = "/api/SetPort/RGBWAsync";
const ApiSetPort_RGBWSingle = "/api/SetPort/RGBWSingle";
const ApiSetPort_IValues = "/api/SetPort/IValues";

/// API RouteTypes:
const ChannelType_RGBISync = "rgbiSync";
const ChannelType_RGBWAsync = "rgbwAsync";
const ChannelType_RGBWSingle = "rgbwStrip";
const ChannelType_IValues = "grayPort";
const ChannelType_Effect = "effectCh";


const ApiGetStatus_WiFiStatus = "/api/GetStatus/WiFiStatus";
const ApiSetDevice_ResetWiFi = "/api/SetDevice/ResetWiFiConnect";
const ApiSetDevice_RestartDevice = "/api/SetDevice/ResetDevice";
const ApiSetDevice_SetWiFi = "api/SetDevice/SetWiFiConnect";
const ApiGetStatus_DeviceConfig = "/api/GetStatus/DeviceConfig";
const ApiGetStatus_DeviceType = "/api/GetStatus/DeviceType";

const ApiLoadPage = "api/LoadPage"
const ApiSaveToPage = "/api/SaveToPage";
const ApiResetProgram = "/api/ResetSavedPages";

/// File Routes
const FileConfigFile = "IOConfiguration.json"
const FileDeviceFile = "Device.json"
const FileGetFile = "/config/"
const FileUploadFile = "/uploadconfig/"
const FileDeleteFile = "/deleteconfig/"

const FileSizeLimit = {size: 200 * 1024, sizeString: '200 kB'} ;
