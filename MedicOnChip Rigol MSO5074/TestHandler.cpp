//TestHandler - Hold methods and parameters to perform tests (FCC,FCS...)

#include "pch.h"
#include "TestHandler.h"
#include "MedicOnChip Rigol MSO5074Dlg.h"

//#include biblioteca do hilton
using namespace std;

vector <string> customSplit(string str, char separator);

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
    parameters.vds_meas_params.volts_div = stof(splitted_values[1])/10; //Ten divisions

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
    parameters.current_meas_params.volts_div = stof(read_values)*(parameters.g_tia)/10; //ten divisions

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
bool MeasurementChannel::write_parameters_to_osc(MeasurementChannel_parameters parameters){

    vector <string> commands;
    commands.push_back("opa");
    return false;
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

//--------------End of auxiliar functions----------------------------------------

