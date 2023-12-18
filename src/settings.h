#ifndef _KEE_SETTINGS
#define _KEE_SETTINGS

enum SettingsType {
	SETTINGS_DATA = 0x01,
	SETTINGS_RUN = 0x02,
	SETTINGS_LOCKTIME = 0x10,
};


class Settings {

	public:
		Settings();
		int init();
		std::string get(SettingsType);
		int set(SettingsType, std::string);

	private:
		std::string m_data;
		std::string m_run;
		std::string m_locktime;
};

#endif
