/// HTML Routes
const Route_SetupFull = '/SetupFull';

/// API Routes GET
const ApiGetPort_RGBISync = "/api/GetPort/RGBISync/";
const ApiGetPort_RGBWAsync = "/api/GetPort/RGBWAsync/";
const ApiGetPort_RGBWSingle = "/api/GetPort/RGBWSingle/";
const ApiGetPort_IValues = "/api/GetPort/IValues/";
const ApiGetFiles = "/api/GetFiles";
/// API Routes SET
const ApiSetPort_RGBISync = "/api/SetPort/RGBISync";
const ApiSetPort_RGBWAsync = "/api/SetPort/RGBWAsync";
const ApiSetPort_RGBWSingle = "/api/SetPort/RGBWSingle";
const ApiSetPort_IValues = "/api/SetPort/IValues";

/// API RouteTypes:
const ChannelType_RGBISync = "rgbiSync";
const ChannelType_RGBWAsync = "rgbwAsync";
const ChannelType_RGBWSingle = "rgbwStrip";
const ChannelType_IValues = "grayPort";

const ApiGetStatus_WiFiStatus = "/api/GetStatus/WiFiStatus";
const ApiSetDevice_ResetWiFi = "/api/SetDevice/ResetWiFiConnect";
const ApiSetDevice_SetWiFi = "api/SetDevice/SetWiFiConnect";
const ApiGetStatus_DeviceConfig = "/api/GetStatus/DeviceConfig";
const ApiGetStatus_DeviceType = "/api/GetStatus/DeviceType";

const ApiSaveToPage = "/api/SaveToPage";
const ApiResetProgram = "/api/ResetProgram";

/// File Routes
const FileConfigFile = "IOConfiguration.json"
const FileDeviceFile = "Device.json"
const FileGetFile = "/config/"
const FileUploadFile = "/uploadconfig/"

const FileSizeLimit = {size: 200 * 1024, sizeString: '200 kB'} ;
