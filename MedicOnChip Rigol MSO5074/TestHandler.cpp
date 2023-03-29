//TestHandler - Hold methods and parameters to perform tests (FCC,FCS...)

#include "pch.h"
#include "TestHandler.h"
#include "MedicOnChip Rigol MSO5074Dlg.h"
//#include "MedicOnChip Rigol MSO5074.h"

//#include biblioteca do hilton
using namespace std;

vector <string> customSplit(string str, char separator);
void string_to_char_array(std::string str, char* buffer);

//Constructors and destructors. Do we need them?
TestHandler::TestHandler(){};
TestHandler::~TestHandler(){};
MeasurementChannel::MeasurementChannel(){};
MeasurementChannel::~MeasurementChannel(){};
SourceChannel::SourceChannel(){};
SourceChannel::~SourceChannel(){};

//--------------Begin of TestHandler Class functions---------------------------------

FCC_parameters TestHandler::get_fcc_parameters(){
    //Reading config file
    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;
    file.read(ini);

    //Create and setup parameters struct
    FCC_parameters parameters;
    parameters.vds_source_params.Id = 1;
    parameters.vds_source_params.wave_type = waves.RAMP;
    parameters.vg_source_params.Id = 2;
    parameters.vg_source_params.wave_type = waves.DC;
    parameters.vds_meas_params.Id = 1;
    parameters.current_meas_params.Id = 2;

    //Aux variables
    string read_values;
    vector<string> splitted_values;

    //Read Vg values and build vector
    read_values = ini.get("FCC").get("VG");
    splitted_values = customSplit(read_values, ';');
    for (auto it: splitted_values){
        parameters.vg_vector.push_back(stof(it));
    }

    //Read Vds max and min and calculate vpp and voffset
    read_values = ini.get("FCC").get("VDS_SPAN");
    splitted_values = customSplit(read_values, ';');
    parameters.vds_source_params.v_pp = stof(splitted_values[1]) - stof(splitted_values[0]);
    parameters.vds_source_params.v_offset = stof(splitted_values[1]) - parameters.vds_source_params.v_pp/2;
    //Vds source will also be measured as a reference in CH1, so we need to setup the measurement channel vertical scale:
    //Warning: the volts/div result may not be acepted by the osciloscope, because there are specific values for it
    parameters.vds_meas_params.volts_div = stof(splitted_values[1])/4; //Vpeak*2/8 = Vpeak/4. We divide by ten because there are 8 divisions

    //Read freq and cycles number and calculates the time scale to set
    //Warning: the t_scale result may not be acepted by the osciloscope, because there are specific values for it
    float cycles = stof(ini.get("FCC").get("CYCLES"));
    parameters.vds_source_params.freq = stof(ini.get("FCC").get("FREQ"));
    parameters.t_scale = cycles/(parameters.vds_source_params.freq*10);   //Divides by ten because there are ten divisions in osciloscope time scale

    //Read RG_LIMITS
    read_values = ini.get("FCC").get("RG_LIMITS");
    splitted_values = customSplit(read_values, ';');
    parameters.rg_limits[MIN] = stof(splitted_values[0]);
    parameters.rg_limits[MAX] = stof(splitted_values[1]);

    //Read transimpedance amp gain and voltage drop
    parameters.g_tia = stof(ini.get("FCC").get("G_TIA"));
    parameters.v_tia = stof(ini.get("FCC").get("V_TIA"));

    //Read max expected current to calculate the vertical scale of the channel
    //Warning: the volts/div result may not be acepted by the osciloscope, because there are specific values for it
    read_values = ini.get("FCC").get("MAX_CURR_EXPECT");
    parameters.current_meas_params.volts_div = stof(read_values)*(parameters.g_tia)/4; //8 divisions

    return parameters;
}

//Get trigger parameters. Usefull to check if the parameters were correctly set by send_trigger_parameters() function
//Output: Trigger_parameters: struct with the current trigger configurations in the osciloscope
Trigger_parameters TestHandler::read_trigger_parameters()
{
    Trigger_parameters parameters;
    return parameters;
};

//Configure trigger
//Input: Trigger_parameters: struct with the trigger configurations to set
//Output: boolean success/fail flag (true=success)
bool TestHandler::send_trigger_parameters(Trigger_parameters)
{
    return false;
};

//Read the current trigger status. Usefull to check if set_trigger_mode() function worked.
//Output: status (STOP, SINGLE, etc...)
string TestHandler::read_trigger_status()
{
    return "STOP";
};

//Set the trigger mode
//Input: string mode: (STOP, SINGLE, etc...)
//Output: boolean success/fail flag (true=success)
bool TestHandler::set_trigger_mode(std::string mode)
{
    return false;
};

//Get horizontal (time) scale. Usefull to check if set_t_scale() function worked.
//Output: time scale (seconds/div)
float TestHandler::get_t_scale()
{
    return 0.0;
};

//Set horizontal (time) scale
//Input: time scale (seconds/div)
//Output: boolean success/fail flag (true=success)
bool TestHandler::set_t_scale(float t_scale)
{
    return false;
};

//Clear curves in the osciloscope screen
void TestHandler::clear_screen(){};

//Config trigger, time scale, source and measurement channels and let osciloscope ready to run FCC
//Output: boolean success/fail flag (true=success)
bool TestHandler::set_osc_to_fcc(FCC_parameters)
{
    return false;
};

//Set SINGLE mode and turn sources on
//Output: boolean success/fail flag (true=success)
bool TestHandler::start_fcc()
{
    return false;
};

//Save FCC curves into CSV file
//Inputs: to be defined
//Outputs: to be defined
void TestHandler::save_fcc_in_csv(float vg, string data){};

//Generate caracteristic curves and calculate Rg
//Inputs: to be defined
//Outputs: to be defined
void TestHandler::calculate_fcc_results(){};

//--------------End of TestHandler Class functions---------------------------------
//--------------Begin of MeasurementChannel Class functions------------------------


//Read the current measurement channel config. Usefull to check if write_parameters_to_osc() worked
//Output: MeasurementChannel_parameters struct with the current config
MeasurementChannel_parameters MeasurementChannel::read_parameters_from_osc()
{
    MeasurementChannel_parameters parameters;
    return parameters;
};
//
//Check if channel is on. Usefull to check if  on() and off() worked
//Output: boolean value, true = channel is on
bool MeasurementChannel::is_active()
{
    return false;
};

//Send the channel config to osciloscope
//Input: MeasurementChannel_parameters struct with the desired parameters
//Output: boolean success/fail flag (true=success)
std::string MeasurementChannel::write_parameters_to_osc(MeasurementChannel_parameters parameters){

    char command[256] = { 0 };
    std::string command_str = "";
    std::string log_string = "";
    std::string scale = ":SCAL ";
    std::string offset = ":OFFS ";
    std::string coupling = ":COUP ";
    std::string attenuation = ":PROB ";
    std::string invert = ":INV ";
    std::string bw_limit = ":BWLimit ";
    std::string channel = ":CHAN";

    channel += std::to_string(parameters.Id);
    command_str = channel;

    //Set Coupling
    command_str += coupling + parameters.coupling;
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    log_string += command_str;
    command_str = channel;

    //Set invert
    command_str += invert;
    if (parameters.invert) command_str += "ON";
    else command_str += "OFF";
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    log_string += command_str;
    command_str = channel;

    //Set BW limit
    command_str += bw_limit;
    if (parameters.bw_limit) command_str += "20M";
    else command_str += "OFF";
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);  
    log_string += command_str;
    command_str = channel;
  
    //Set Attenuation
    command_str += attenuation + parameters.attenuation;
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    log_string += command_str;
    command_str = channel;

    //Set fine scale adjust
    command_str = ":CHANnel1:VERNier ON\n";
    SendCommand(command);
    log_string += command_str;
    command_str = channel;

    //Set volts/div
    command_str += scale + std::to_string(parameters.volts_div);
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    log_string += command_str;
    command_str = channel;

    //Set offset
    command_str += offset + std::to_string(parameters.offset);
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    log_string += command_str;
    
    return log_string;
};

//Turn the channel on
//Output: boolean success/fail flag (true=success)
bool MeasurementChannel::on()
{
    return false;
};

//Turn the channel off
//Output: boolean success/fail flag (true=success)
bool MeasurementChannel::off()
{
    return false;
};

//--------------End of  MeasurementChannel Class functions------------------------
//--------------Begin of SourceChannel Class functions------------------------

//Read the current source channel config. Usefull to check if write_parameters_to_osc() worked
//Output: SourceChannel_parameters struct with the current config
SourceChannel_parameters SourceChannel::read_parameters_from_osc()
{
    SourceChannel_parameters parameters;
    return parameters;
};

//Check if channel is on. Usefull to check if  start() and stop() worked
//Output: boolean value, true = channel is on
bool SourceChannel::is_active()
{
    return false;
};

//Send the channel config to osciloscope
//Input: SourceChannel_parameters struct with the desired parameters
//Output: boolean success/fail flag (true=success)
bool SourceChannel::write_parameters_to_osc(SourceChannel_parameters parameters)
{
    return false;
};

//Start source
//Output: boolean success/fail flag (true=success)
bool SourceChannel::start()
{
    return false;
};

//Stop source
//Output: boolean success/fail flag (true=success)
bool SourceChannel::stop()
{
    return false;
};

//--------------End of  SourceChannel Class functions------------------------
//--------------Begin of auxiliar functions----------------------------------------

// Custom split() function (https://favtutor.com/blogs/split-string-cpp)
vector <string> customSplit(string str, char separator) {
    int startIndex = 0, endIndex = 0;
    vector <string> strings;
    for (int i = 0; i <= str.size(); i++) {
        
        // If we reached the end of the word or the end of the input.
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            strings.push_back(temp);
            startIndex = endIndex + 1;
        }
    }
    return strings;
}

//Test function
int main(){
    TestHandler tester;
    FCC_parameters results;
    results = tester.get_fcc_parameters();

    cout << results.vds_source_params.v_pp << endl;
    cout << results.vds_source_params.freq << endl;
    cout << results.vds_source_params.v_offset << endl;
    cout << results.vds_source_params.Id << endl;
    cout << results.vds_source_params.wave_type << endl;

    cout << results.vg_source_params.v_pp << endl;
    cout << results.vg_source_params.freq << endl;
    cout << results.vg_source_params.v_offset << endl;
    cout << results.vg_source_params.Id << endl;
    cout << results.vg_source_params.wave_type << endl;

    return 0;
}

void string_to_char_array(std::string str, char* buffer) {
    for (int i = 0; i < str.std::string::length(); i++) {
        buffer[i] = str[i];
    }
}

/*
void SendCommand(char _command[256])
{
    ViSession defaultRM, vi;
    char buf[256] = { 0 };
    CString s, command;
    //char* stringTemp;
    char stringTemp[256];

    ViChar buffer[VI_FIND_BUFLEN];
    ViRsrc matches = buffer;
    ViUInt32 nmatches;
    ViFindList list;

    viOpenDefaultRM(&defaultRM);
    //Acquire the USB resource of VISA
    viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
    viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);

    //Send the command received
    //m_combox.GetLBText(m_combox.GetCurSel(), strTemp);
    //m_combox.GetWindowText(strTemp);
    command = _command;
    command += "\n";
    //stringTemp = (char*)(LPCTSTR)strTemp;
    for (int i = 0; i < command.GetLength(); i++)
        stringTemp[i] = (char)command.GetAt(i);

    theApp.m_pMainWnd->UpdateData(TRUE);
    theApp.m_pMainWnd->m_receive = stringTemp;
    theApp.m_pMainWnd->UpdateData(FALSE);

    viPrintf(vi, stringTemp);

    viClose(defaultRM);
}
*/
//--------------End of auxiliar functions----------------------------------------

