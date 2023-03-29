//TestHandler - Hold methods and parameters to perform tests (FCC,FCS...)

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "mini/ini.h"

#define MIN "MIN"
#define MAX "MAX"

const struct waves{
	std::string RAMP = "RAMP";
	std::string SQUARE = "SQUARE";
	std::string DC = "DC";
} waves;

struct Trigger_parameters{
	std::string status;
	float level;
	std::string type;
	std::string slope;
	unsigned int source;
};

struct SourceChannel_parameters{
	float v_pp = 0;
	float v_offset = 0;
	std::string wave_type = "none";
	unsigned int Id = 0;
	unsigned int freq = 0;
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
	float t_scale;
	float g_tia;
	float v_tia;
	std::map<std::string,float> rg_limits;
	std::vector<float> vg_vector;
};

class TestHandler
{
public:
	TestHandler();
	~TestHandler();
	Trigger_parameters read_trigger_parameters();			//ID 1
	bool send_trigger_parameters(Trigger_parameters);		//ID 2
	std::string read_trigger_status();						//ID 3
	bool set_trigger_mode(std::string mode);				//ID 4
	bool set_t_scale(float t_scale);						//ID 5
	float get_t_scale();									//ID 6
	void clear_screen();									//ID 7
	FCC_parameters get_fcc_parameters();					//ID 8
	bool set_osc_to_fcc(FCC_parameters);					//ID 9
	bool start_fcc();										//ID 10
	void save_fcc_in_csv(float vg, std::string data);		//ID 11
	void calculate_fcc_results();							//ID 12
};

class MeasurementChannel : public TestHandler
{
public:
	MeasurementChannel();
	~MeasurementChannel();
	MeasurementChannel_parameters read_parameters_from_osc();			//ID 13
	bool is_active();													//ID 14
	std::string write_parameters_to_osc(MeasurementChannel_parameters parameters);		//ID 15
	bool on();															//ID 17
	bool off();															//ID 18
};

class SourceChannel : public TestHandler
{
public:
	SourceChannel();
	~SourceChannel();
	SourceChannel_parameters read_parameters_from_osc();	//ID 19
	bool is_active();										//ID 20
	bool write_parameters_to_osc(SourceChannel_parameters parameters);	//ID 21
	bool start();											//ID 22
	bool stop();											//ID 23
};