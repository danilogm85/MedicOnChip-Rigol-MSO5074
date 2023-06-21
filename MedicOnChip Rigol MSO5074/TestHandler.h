//TestHandler - Hold methods and parameters to perform tests (FCC,FCS...)

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "MedicOnChip Rigol MSO5074Dlg.h"
#include "mini/ini.h"

#define MIN "MIN"
#define MAX "MAX"

//#define NUM_MEDIAS 1
/*
const struct Waves{
	std::string RAMP = "RAMP";
	std::string SQUARE = "SQUARE";
	std::string DC = "DC";
} waves;
*/
const struct sys_commands {	
	std::string STOP = ":STOP";
	std::string RUN = ":RUN";
	std::string SINGLE = ":SINGle";
	std::string TFORCE = ":TFORce";
	std::string CLEAR = ":CLEar";
	std::string MDEPTH = ":ACQuire:MDEPth 1M";
	std::string HRES = ":ACQuire:TYPE HRES";
} sys_commands;

struct Trigger_parameters{
	float level = 0.01;
	std::string mode = "EDGE";
	std::string slope = "POS";
	std::string source = "CHAN1";
};

struct SourceChannel_parameters{
	float v_pp = 0;
	float v_offset = 0;
	std::string wave_type = "none";
	unsigned int Id = 0;
	float freq = 0;
	float LOWfreq = 0;
	float HIGHfreq = 0;
	unsigned int cycles=0;	//Number of cycles of the burst
	float LOW_CYCLES = 0;
	float HIGH_CYCLES = 0;
	std::string generatorType = "none";
	std::string Burst_Type = "none";
};

struct MeasurementChannel_parameters{
	float volts_div;
	float offset = 0;
	std::string coupling = "DC";
	std::string attenuation = "1";
	bool invert = false;
	bool bw_limit = false;
	unsigned int Id;
};

struct FCC_parameters{
	SourceChannel_parameters vds_source_params;
	SourceChannel_parameters vg_source_params;
	MeasurementChannel_parameters vds_meas_params;
	MeasurementChannel_parameters current_meas_params;
	MeasurementChannel_parameters vg_meas_params;
	float t_scale;
	float g_tia;
	float v_tia;
	std::map<std::string,float> rg_limits;
	std::vector<float> vg_vector;
	unsigned int bursts = 1;
	std::string AquireType = "HRES";
};

struct FCS_parameters {
	SourceChannel_parameters vds_source_params;
	SourceChannel_parameters vg_source_params;
	MeasurementChannel_parameters vds_meas_params;
	MeasurementChannel_parameters current_meas_params;
	MeasurementChannel_parameters vg_meas_params;
	float t_scale;
	float g_tia;
	float v_tia;
	//std::map<std::string, float> rg_limits;
	//std::vector<float> vg_vector;
	float flat_region_width;
	unsigned int bursts = 1;
	std::string AquireType = "HRES";
};

struct FCP_parameters {
	//SourceChannel_parameters vds_source_params;
	SourceChannel_parameters vg_source_params;
	SourceChannel_parameters vds_source_params;
	MeasurementChannel_parameters current_meas_params;
	MeasurementChannel_parameters vds_meas_params;
	//MeasurementChannel_parameters current_meas_params;
	MeasurementChannel_parameters vg_meas_params;
	float t_scale;
	float Low_t_scale;
	float High_t_scale;
	unsigned int bursts = 1;
	int AquireAverages = 2;
	std::string AquireType = "AVER";
	//float g_tia;
	//float v_tia;
	//std::map<std::string, float> rg_limits;
	//std::vector<float> vg_vector;
	//float flat_region_width;
};

class TestHandler
{
public:
	TestHandler();
	~TestHandler();
	Trigger_parameters read_trigger_parameters();			//ID 1
	bool send_trigger_parameters(Trigger_parameters parameters);		//ID 2 - QUASE OK, FALTA A VERIFICAÇÃO SE CONFIGUROU CERTO
	std::string read_trigger_status();	
	void triggerMode_FCP();//ID 3
	bool set_trigger_mode(std::string mode);				//ID 4
	bool set_t_scale(float t_scale);						//ID 5 - QUASE OK, FALTA A VERIFICAÇÃO SE CONFIGUROU CERTO
	float get_t_scale();									//ID 6
	void clear_screen();	
	void aquire_Numb_averages(FCP_parameters parameters);//ID 7
	void type_Aquire(std::string type);
	FCC_parameters get_fcc_parameters();					//ID 8
	FCS_parameters get_fcs_parameters();				
	FCP_parameters get_fcp_parameters();
	bool set_osc_to_fcc(FCC_parameters parameters);					//ID 9
	//bool start_fcc();										//ID 10
	//void save_fcc_in_csv(float vg, std::string data);		//ID 11
	//void calculate_fcc_results();							//ID 12
	std::string log_string = "";
	std::string LOW_log_string = "";
	std::string HIGH_log_string = "";
};

class MeasurementChannel : public TestHandler
{
private:
	int id;
public:
	MeasurementChannel(int _id);
	~MeasurementChannel();
	MeasurementChannel_parameters read_parameters_from_osc();			//ID 13
	bool is_active();													//ID 14
	bool write_parameters_to_osc(MeasurementChannel_parameters parameters);		//ID 15 - QUASE OK, FALTA A VERIFICAÇÃO SE CONFIGUROU CERTO
	bool on();															//ID 17
	bool off();															//ID 18
	unsigned int get_id();
	int read_channel_wave(ViSession m_vi, float* result_buff);
	float get_sample_period(ViSession m_vi);
};

class SourceChannel : public TestHandler
{
private:
	unsigned int id;
public:
	SourceChannel();
	~SourceChannel();
	SourceChannel_parameters read_parameters_from_osc();	//ID 19
	bool is_active(SourceChannel_parameters parameters);										//ID 20
	bool write_parameters_to_osc(SourceChannel_parameters parameters);	//ID 21
	void waveForm_write_to_osc(SourceChannel_parameters parameters);
	void VPP_write_to_osc(SourceChannel_parameters parameters);
	void Voffset_write_to_osc(SourceChannel_parameters parameters);
	void Frequency_write_to_osc(SourceChannel_parameters parameters);
	void Generator_type_to_osc(SourceChannel_parameters parameters);
	void Burst_Type_write_to_osc(SourceChannel_parameters parameters);
	void Burst_Cycles_write_to_osc(SourceChannel_parameters parameters);
	bool start(int Source_ID);											//ID 22
	bool stop(int Source_ID);											//ID 23
	//bool burst(int Source_ID);
};

void string_to_char_array(std::string str, char* buffer);