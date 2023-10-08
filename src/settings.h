enum SettingsType {
	DATA = 0x01,
};


class Settings {

	public:
		Settings();
		std::string get(SettingsType);

	private:
		std::string m_data;
};
