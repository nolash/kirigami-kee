#ifndef _KEE_SETTINGS
#define _KEE_SETTINGS

enum SettingsType {
	SETTINGS_DATA = 0x01,
};


class Settings {

	public:
		Settings();
		int init();
		std::string get(SettingsType);

	private:
		std::string m_data;
};

#endif
