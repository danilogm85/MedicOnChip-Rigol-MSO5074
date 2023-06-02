//TestHandler - Hold methods and parameters to perform tests (FCC,FCS...)

/*      BACKLOG

CONFIGURAÇÕES DO OSC QUE PODEM SER NECESSÁRIAS:
-ACQUIRE:
--MEM DEPTH
--HIGH RESOLUTION
--NUMERO DE MÉDIAS

-TRIGGER:
RUÍDO ESTÁ DISPARANDO O TRIGGER. COLOCAR TRIGGER EM UMA FONTE NÃO UTILIZADA (ex. D0) E TRIGGAR PELO FORCE???
-Nesse caso, temos que colocar um pull down no D0 para garantir que ele não trigue nada
-PRÉ TRIGGER?

-APLICAÇÃO DE SINAL:
NÃO DA PRA DAR O MANUAL TRIGGER REMOTAMENTE???
*/

#include "pch.h"
#include "TestHandler.h"
#include "MedicOnChip Rigol MSO5074Dlg.h"
//#include "MedicOnChip Rigol MSO5074.h"

//#include biblioteca do hilton
using namespace std;
vector <string> customSplit(string str, char separator);

//Constructors and destructors. Do we need them?
TestHandler::TestHandler(){};
TestHandler::~TestHandler(){};
MeasurementChannel::MeasurementChannel(int _id){
    id = _id;
}
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
    parameters.vds_source_params.wave_type = "RAMP";
    parameters.vg_source_params.Id = 2;
    parameters.vg_source_params.wave_type = "DC";
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
    //parameters.vds_meas_params.volts_div = stof(splitted_values[1])/4; //Vpeak*2/8 = Vpeak/4. We divide by ten because there are 8 divisions
    parameters.current_meas_params.volts_div = stof(splitted_values[1]) / 4; //Vpeak*2/8 = Vpeak/4. We divide by ten because there are 8 divisions

    //Read freq and cycles number and calculates the time scale to set
    //Warning: the t_scale result may not be acepted by the osciloscope, because there are specific values for it
    parameters.vds_source_params.cycles = stoul(ini.get("FCC").get("CYCLES"));
    parameters.vds_source_params.freq = stof(ini.get("FCC").get("FREQ"));
    parameters.t_scale = float(parameters.vds_source_params.cycles) / (float(parameters.vds_source_params.freq) * 10);   //Divides by ten because there are ten divisions in osciloscope time scale
    
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
    read_values = ini.get("FCC").get("MAX_VDS_EXPECT");
    //parameters.current_meas_params.volts_div = stof(read_values)*(parameters.g_tia)/4; //8 divisions
    parameters.vds_meas_params.volts_div = stof(read_values)/4; //8 divisions

   // parameters.vg_source_params.v_pp = parameters.vg_vector[0];
    //parameters.vg_source_params.v_offset = parameters.vg_vector[0];

    //Finding the greater absolute value of vg
    float max_mod = 0;
    for (int it = 0 ; it < parameters.vg_vector.size() ; it++) {
        if (parameters.vg_vector[it] < 0) {
            if (parameters.vg_vector[it] < max_mod) {
                max_mod = -parameters.vg_vector[it];
            }
        }
        else {
            if (parameters.vg_vector[it] > max_mod) {
                max_mod = parameters.vg_vector[it];
            }
        }
    }
    
    //VG measurement parameters
    parameters.vg_meas_params.Id = 3;
    if (max_mod != 0) {
        parameters.vg_meas_params.volts_div = max_mod/2;
    }
    else {
        parameters.vg_meas_params.volts_div = 0.01;
    }

    parameters.bursts = stoul(ini.get("FCC").get("BURSTS"));

    return parameters;  
}

FCS_parameters TestHandler::get_fcs_parameters() {
    //Reading config file
    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;
    file.read(ini);

    //Create and setup parameters struct
    FCS_parameters parameters;
    parameters.vds_source_params.Id = 1;
    parameters.vds_source_params.wave_type = "DC";
    parameters.vg_source_params.Id = 2;
    parameters.vg_source_params.wave_type = "RAMP";
    parameters.vds_meas_params.Id = 1;
    parameters.current_meas_params.Id = 2;
    parameters.vg_meas_params.Id = 3;

    //Aux variables
    string read_values;
    vector<string> splitted_values;

    //Read Vg values and build vector
    parameters.vds_source_params.v_pp = 0;
    parameters.vds_source_params.v_offset = stof(ini.get("FCS").get("VDS"));

    //Read Vg max and min and calculate vpp and voffset
    read_values = ini.get("FCS").get("VG_SPAN");
    splitted_values = customSplit(read_values, ';');
    parameters.vg_source_params.v_pp = stof(splitted_values[1]) - stof(splitted_values[0]);
    parameters.vg_source_params.v_offset = stof(splitted_values[1]) - parameters.vg_source_params.v_pp / 2;
    //Vg source will also be measured as a reference in CH1, so we need to setup the measurement channel vertical scale:
    //Warning: the volts/div result may not be acepted by the osciloscope, because there are specific values for it
    parameters.vg_meas_params.volts_div = stof(splitted_values[1]) / 4; //Vpeak*2/8 = Vpeak/4. We divide by ten because there are 8 divisions

    //Read freq and cycles number and calculates the time scale to set
    //Warning: the t_scale result may not be acepted by the osciloscope, because there are specific values for it
    parameters.vg_source_params.cycles = stoul(ini.get("FCS").get("CYCLES"));
    parameters.vg_source_params.freq = stof(ini.get("FCS").get("FREQ"));
    parameters.t_scale = float(parameters.vg_source_params.cycles) / (float(parameters.vg_source_params.freq) * 10);   //Divides by ten because there are ten divisions in osciloscope time scale
    //log_string = to_string(parameters.vds_source_params.freq);

    //Read transimpedance amp gain and voltage drop
    parameters.g_tia = stof(ini.get("FCS").get("G_TIA"));
    parameters.v_tia = stof(ini.get("FCS").get("V_TIA"));

    //Read max expected current to calculate the vertical scale of the channel
    //Warning: the volts/div result may not be acepted by the osciloscope, because there are specific values for it
    read_values = ini.get("FCS").get("MAX_VDS_EXPECT");
    //parameters.current_meas_params.volts_div = stof(read_values) * (parameters.g_tia) / 4; //8 divisions
    parameters.vds_meas_params.volts_div = stof(read_values); //8 divisions

    //VDS measurement parameters
    //parameters.vds_meas_params.volts_div = parameters.vds_source_params.v_offset / 2;
    parameters.current_meas_params.volts_div = parameters.vds_source_params.v_offset / 2;

    parameters.bursts = stoul(ini.get("FCS").get("BURSTS"));

    return parameters;
}

FCP_parameters TestHandler::get_fcp_parameters() {
    //Reading config file
    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;
    file.read(ini);

    //Create and setup parameters struct
    FCP_parameters parameters;
    //parameters.vds_source_params.Id = 1;
    //parameters.vds_source_params.wave_type = "DC";
    parameters.vg_source_params.Id = 2;
    parameters.vg_source_params.wave_type = "SQU";
    parameters.vds_meas_params.Id = 1;
    //parameters.current_meas_params.Id = 2;
    parameters.vg_meas_params.Id = 3;
    parameters.vds_source_params.Id = 1;
    parameters.vds_source_params.wave_type = "DC";
    parameters.current_meas_params.Id = 2;
    //Aux variables
    string read_values;
    //vector<string> splitted_values;

    //Read Vg values and build vector
    //parameters.vds_source_params.v_pp = 0;
    //parameters.vds_source_params.v_offset = stof(ini.get("FCS").get("VDS"));

    //Read Vg max and min and calculate vpp and voffset
    //read_values = ini.get("FCS").get("VG_SPAN");
    //splitted_values = customSplit(read_values, ';');
    //parameters.vg_source_params.v_pp = stof(splitted_values[1]) - stof(splitted_values[0]);
    //parameters.vg_source_params.v_offset = stof(splitted_values[1]) - parameters.vg_source_params.v_pp / 2;
    //Vg source will also be measured as a reference in CH1, so we need to setup the measurement channel vertical scale:
    //Warning: the volts/div result may not be acepted by the osciloscope, because there are specific values for it
    //parameters.vg_meas_params.volts_div = stof(splitted_values[1]) / 4; //Vpeak*2/8 = Vpeak/4. We divide by ten because there are 8 divisions

    //Read freq and cycles number and calculates the time scale to set
    //Warning: the t_scale result may not be acepted by the osciloscope, because there are specific values for it
    parameters.vg_source_params.cycles = stoul(ini.get("FCP").get("CYCLES"));
    parameters.vg_source_params.freq = stof(ini.get("FCP").get("FREQ"));
    parameters.t_scale = float(parameters.vg_source_params.cycles) / (float(parameters.vg_source_params.freq) * 10);   //Divides by ten because there are ten divisions in osciloscope time scale
    parameters.AquireAverages= stoul(ini.get("FCP").get("ACQUIRE_AVG"));
    log_string = to_string(parameters.t_scale);

    //Read transimpedance amp gain and voltage drop
    //parameters.g_tia = stof(ini.get("FCS").get("G_TIA"));
    //parameters.v_tia = stof(ini.get("FCS").get("V_TIA"));

    //Read max expected current to calculate the vertical scale of the channel
    //Warning: the volts/div result may not be acepted by the osciloscope, because there are specific values for it
    //read_values = ini.get("FCS").get("MAX_CURR_EXPECT");

    //VDS measurement parameters
    parameters.vds_meas_params.volts_div = stof(ini.get("FCP").get("MAX_VDS_EXPECT")) / 4;

    parameters.bursts = stoul(ini.get("FCP").get("BURSTS"));

    //Read Vg values and build vector
    parameters.vds_source_params.v_pp = 0;
    parameters.vds_source_params.v_offset = stof(ini.get("FCP").get("CURR_SOURCE_BIAS"));
    parameters.current_meas_params.volts_div = parameters.vds_source_params.v_offset/2; //8 divisions

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
//Configure triggerset_trigger_mode
//Input: Trigger_parameters: struct with the trigger configurations to set
//Output: boolean success/fail flag (true=success)
bool TestHandler::send_trigger_parameters(Trigger_parameters parameters)
{
    /*struct Trigger_parameters{
	float level;
	std::string mode;
	std::string slope;
	unsigned int source;
};*/

    char command[256] = { 0 };
    std::string command_str = "";
    //std::string log_string = "";
    std::string mode = ":MODE";
    std::string slope = ":SLOP ";
    std::string source = ":SOUR ";
    std::string level = ":LEV ";
    std::string trigger = ":TRIG";

    command_str = trigger;

    //Set Mode
    command_str += mode + " " + parameters.mode;
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    //log_string += command_str;
    command_str = trigger + ":" + parameters.mode;

    //Set slope
    command_str += slope + parameters.slope;
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    //log_string += command_str;
    command_str = trigger + ":" + parameters.mode;

    //Set SOURCE
    command_str += source + parameters.source;
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    //log_string += command_str;
    command_str = trigger + ":" + parameters.mode;

    //Set level
    command_str += level + std::to_string(parameters.level);
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    //log_string += command_str;

    return false;
}

//Read the current trigger status. Usefull to check if set_trigger_mode() function worked.
//Output: status (STOP, SINGLE, etc...)
string TestHandler::read_trigger_status()
{
    string read = ":TRIGger:STATus?\n";
    char SCPI_command[256];
    string_to_char_array(read, SCPI_command);
    read = readOsciloscope(SCPI_command);
   // log_string = read;
    return read;
}

//Set the trigger mode
//Input: string mode: (STOP, SINGLE, etc...)
//Output: boolean success/fail flag (true=success)
bool TestHandler::set_trigger_mode(std::string mode)
{
    return false;
};
void TestHandler::triggerMode_FCP() {
    char SCPI_command[256];
    string_to_char_array(sys_commands.RUN, SCPI_command);
    SendCommand(SCPI_command);    
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
    char command[256] = { 0 };
    std::string command_str = ":TIM:MAIN:SCAL ";
    command_str += std::to_string(t_scale) + "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);

    return true;
};

//Clear curves in the osciloscope screen
void TestHandler::clear_screen(){
    char channel_1[256] = ":CHANnel1:DISPlay OFF";
    char channel_2[256] = ":CHANnel2:DISPlay OFF";
    char channel_3[256] = ":CHANnel3:DISPlay OFF";
    char channel_4[256] = ":CHANnel4:DISPlay OFF";
    SendCommand(channel_1);
    SendCommand(channel_2);
    SendCommand(channel_3);
    SendCommand(channel_4);

};
void TestHandler::aquire_Numb_averages(FCP_parameters parameters) {
    std::string Aquire_N_AVERAGES = ":ACQuire:AVER "+to_string(parameters.AquireAverages) + "\n";
    char SCPI_Numb_averages[256];
    string_to_char_array(Aquire_N_AVERAGES,SCPI_Numb_averages);
    
    SendCommand(SCPI_Numb_averages);
};
void TestHandler::type_Aquire(std::string type) {
    std::string Aquire_TYPE = ":ACQuire:TYPE " + type + "\n";
    char SCPI_TYPE[256];
    string_to_char_array(Aquire_TYPE,SCPI_TYPE);
    SendCommand(SCPI_TYPE, true, ":ACQuire:TYPE?", type);
};
//Config trigger, time scale, source and measurement channels and let osciloscope ready to run FCC
//Output: boolean success/fail flag (true=success)
bool TestHandler::set_osc_to_fcc(FCC_parameters parameters)
{
    //Configura escala de tempo
    set_t_scale(parameters.t_scale);

    //Configura canais de medição
    MeasurementChannel vds_meas(parameters.vds_meas_params.Id);
    MeasurementChannel current_meas(parameters.current_meas_params.Id);
    vds_meas.write_parameters_to_osc(parameters.vds_meas_params);
    current_meas.write_parameters_to_osc(parameters.current_meas_params);

    //Configura trigger
    Trigger_parameters trigger_parameters;
    trigger_parameters.source = "CHAN1";
    send_trigger_parameters(trigger_parameters);

    //Configura fontes
    SourceChannel vds_source, vg_source;
    vds_source.write_parameters_to_osc(parameters.vds_source_params);
    vg_source.write_parameters_to_osc(parameters.vg_source_params);

    char buff[10] = { 0 };

    //Stop
    string_to_char_array(sys_commands.STOP, &buff[0]);
    SendCommand(buff);

    //Limpa tela
    string_to_char_array(sys_commands.CLEAR, &buff[0]);
    SendCommand(buff);

    //Desliga canais de fontes
    vds_source.stop(1);
    vg_source.stop(2);

    //Liga canais de medição
    vds_meas.on();
    current_meas.on();

    return false;
}
/*
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
*/
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

    char command[256] = { 0 };
    std::string command_str = "";
    //std::string log_string = "";
    std::string scale = ":SCAL ";
    std::string offset = ":OFFS ";
    std::string coupling = ":COUP ";
    std::string attenuation = ":PROB ";
    std::string invert = ":INV ";
    std::string bw_limit = ":BWLimit ";
    std::string channel = ":CHAN";

    channel += std::to_string(id);
    command_str = channel;

    //Set Coupling
    command_str += coupling + parameters.coupling;
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    //log_string += command_str;
    command_str = channel;

    //Set invert
    command_str += invert;
    if (parameters.invert) command_str += "ON";
    else command_str += "OFF";
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    //log_string += command_str;
    command_str = channel;

    //Set BW limit
    command_str += bw_limit;
    if (parameters.bw_limit) command_str += "20M";
    else command_str += "OFF";
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);  
    //log_string += command_str;
    command_str = channel;
  
    //Set Attenuation
    command_str += attenuation + parameters.attenuation;
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    //log_string += command_str;
    command_str = channel;

    //Set fine scale adjust
    command_str = ":CHANnel1:VERNier ON\n";
    SendCommand(command);
    //log_string += command_str;
    command_str = channel;

    //Set volts/div
    command_str += scale + std::to_string(parameters.volts_div);
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    //log_string += command_str;
    command_str = channel;

    //Set offset
    command_str += offset + std::to_string(parameters.offset);
    command_str += "\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);
    //log_string += command_str;
    
    return true;
}

//Turn the channel on
//Output: boolean success/fail flag (true=success)
bool MeasurementChannel::on()
{
    char command[20] = { 0 };
    std::string command_str = ":CHAN";
    command_str += std::to_string(id) + ":DISP 1\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);

    return false;
};

//Turn the channel off
//Output: boolean success/fail flag (true=success)
bool MeasurementChannel::off()
{

    char command[20] = { 0 };
    std::string command_str = ":CHAN";
    command_str += std::to_string(id) + ":DISP 0\n";
    string_to_char_array(command_str, &command[0]);
    SendCommand(command);

    return false;
}

unsigned int MeasurementChannel::get_id() {
    return id;
}

float MeasurementChannel::get_sample_period(ViSession m_vi) {
    ViByte buf[2048];
    ViUInt32 cnt = 2048;
    ViUInt32  readcnt;
    char* temp;
    float Ts;
    //Determina a resolução horizontal
    viPrintf(m_vi, ":WAVeform:XINCrement?\n");
    viRead(m_vi, buf, cnt, &readcnt);
    temp = new char[readcnt];
    for (int i = 0; i < readcnt; i++)
        temp[i] = buf[i];
    Ts = atof(temp);		//Período de amostragem
    delete[] temp;

    return Ts;
}   

int MeasurementChannel::read_channel_wave(ViSession m_vi, float* result_buff) {
    ViByte buf[2048];

    ViUInt32 cnt = 2048;
    ViUInt32  readcnt;

    char* temp;
    //float Ts;
    float deltaV;
    int N;
    int tam;
    float* sinal;
    //float result[2048];

    //Seleciona o canal
    temp = new char[256];
    sprintf_s(temp, 256, ":WAVeform:SOURce CHANnel%d\n", id);
    viPrintf(m_vi, temp);
    delete[] temp;

    /*
    //Determina a resolução horizontal
    viPrintf(m_vi, ":WAVeform:XINCrement?\n");
    viRead(m_vi, buf, cnt, &readcnt);
    temp = new char[readcnt];
    for (int i = 0; i < readcnt; i++)
        temp[i] = buf[i];
    Ts = atof(temp);		//Período de amostragem
    delete[] temp;*/

    //Determina a resolução vertical
    viPrintf(m_vi, ":WAVeform:YINCrement?\n");
    viRead(m_vi, buf, cnt, &readcnt);
    temp = new char[readcnt];
    for (int i = 0; i < readcnt; i++)
        temp[i] = buf[i];
    deltaV = atof(temp);	//Resolução vertical
    delete[] temp;

    //Lê os dados
    viPrintf(m_vi, ":WAVeform:DATA?\n");
    viRead(m_vi, buf, cnt, &readcnt);
    temp = new char[2];					// Obtém o parâmetro N do cabeçalho
    sprintf_s(temp, 2, "%c", buf[1]);
    N = atoi(temp);
    delete[] temp;

    temp = new char[N + 1];				// Obtém o tamanho do buffer de dados
    for (int i = 0; i < N; i++)
        temp[i] = buf[2 + i];
    temp[N] = '\n';
    tam = atoi(temp);
    delete[] temp;

    /*
    CString lixo;						// Imprime o tamanho
    lixo.Format(_T("%d"), tam);
    GetDlgItem(IDC_EDIT_RCVD_MSG)->SetWindowTextW(lixo);
    */

    //std::ofstream myfile;
    //myfile.open("example1.csv");

   // result_buff = new float[tam];				// Aloca o buffer e preenche
    for (int i = 0; i < tam; i++) {
        result_buff[i] = (buf[2 + N + i] - 127) * deltaV;
        //result[i] = sinal[i];
        //myfile  << sinal[i] << "\n";
    }

    //myfile.close();

    //delete[] sinal;

    return tam;
}

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
bool SourceChannel::is_active(SourceChannel_parameters parameters)
{
    string read = ":SOURce" + to_string(parameters.Id) + ":OUTPut?" + "\n";
    char SCPI_command[256];
    string_to_char_array(read, SCPI_command);
    read = readOsciloscope(SCPI_command);
    if (read == "1") return true;
    else if (read == "0") return false;
    
};

//Send the channel config to osciloscope
//Input: SourceChannel_parameters struct with the desired parameters
//Output: boolean success/fail flag (true=success)
bool SourceChannel::write_parameters_to_osc(SourceChannel_parameters parameters)
{
    waveForm_write_to_osc(parameters);
    Frequency_write_to_osc(parameters);
    VPP_write_to_osc(parameters);
    Voffset_write_to_osc(parameters);
    Generator_type_to_osc(parameters);
    //Burst_Type_write_to_osc(parameters);
    //Burst_Cycles_write_to_osc(parameters);
    
    return false;
};

void SourceChannel::Frequency_write_to_osc(SourceChannel_parameters parameters)
{
    std::string freq = ":SOURce" + to_string(parameters.Id) + ":FREQuency " + to_string(parameters.freq) + "\n";
    char SCPI_freq[256];
    string_to_char_array(freq, SCPI_freq);
    SendCommand(SCPI_freq);

  
};

void SourceChannel::waveForm_write_to_osc(SourceChannel_parameters parameters)
{
    std::string wave = ":SOURce" + to_string(parameters.Id) + ":FUNCtion " + parameters.wave_type + "\n";
    char SCPI_wave_form[256];
    string_to_char_array(wave,SCPI_wave_form);
    SendCommand(SCPI_wave_form);

    

};

void SourceChannel::VPP_write_to_osc(SourceChannel_parameters parameters) {
    
    std::string vpp = ":SOURce" + to_string(parameters.Id) + ":VOLTage " + to_string(parameters.v_pp) + "\n";
    char SCPI_vpp[256];
    string_to_char_array(vpp,SCPI_vpp);
    SendCommand(SCPI_vpp);

    

};

void SourceChannel::Voffset_write_to_osc(SourceChannel_parameters parameters)
{
    std::string offset = ":SOURce" + to_string(parameters.Id) + ":VOLTage:OFFSet " + to_string(parameters.v_offset) + "\n";
    char SCPI_Voffset[256];
    string_to_char_array(offset,SCPI_Voffset);
    SendCommand(SCPI_Voffset);

    

};
void SourceChannel::Generator_type_to_osc(SourceChannel_parameters parameters)
{
    std::string generatorType = ":SOURce" +to_string(parameters.Id)+":TYPE "+parameters.generatorType + "\n";
    char SCPI_GeneratorType[256];
    string_to_char_array(generatorType,SCPI_GeneratorType);
    SendCommand(SCPI_GeneratorType);
};
void SourceChannel::Burst_Type_write_to_osc(SourceChannel_parameters parameters)
{
    std::string burstType = ":SOURce" + to_string(parameters.Id) + ":BURSt:TYPE "+ parameters.Burst_Type + "\n";
    char SCPI_BurstType[256];
    string_to_char_array(burstType, SCPI_BurstType);
    SendCommand(SCPI_BurstType);

};
void SourceChannel::Burst_Cycles_write_to_osc(SourceChannel_parameters parameters)
{
    std::string burst = ":SOURce" + to_string(parameters.Id) + ":BURSt:CYCLes " + to_string(parameters.cycles) + "\n";
    char SCPI_BurstCycles[256];
    string_to_char_array(burst, SCPI_BurstCycles);
    SendCommand(SCPI_BurstCycles);
};
//Start source
//Output: boolean success/fail flag (true=success)
bool SourceChannel::start(int Source_ID)
{
    string aux = ":SOURce" + to_string(Source_ID) + ":OUTPut 1\n";
    char SCPI_command[256];
    string_to_char_array(aux, SCPI_command);
    SendCommand(SCPI_command);
    return false;
};

//Stop source
//Output: boolean success/fail flag (true=success)
bool SourceChannel::stop(int Source_ID)
{
    string aux = ":SOURce" + to_string(Source_ID) + ":OUTPut 0\n";
    char SCPI_command[256];
    string_to_char_array(aux, SCPI_command);
    SendCommand(SCPI_command);
    return false;
};
/*
bool SourceChannel::burst(int Source_ID)
{

}*/

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

