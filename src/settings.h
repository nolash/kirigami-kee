#ifndef _KEE_SETTINGS
#define _KEE_SETTINGS

/**
 *
 * \brief Numeric code used to set and get specific settings items.
 */
enum SettingsType {
	/// Data directory
	SETTINGS_DATA = 0x01,
	/// Runtime directory
	SETTINGS_RUN = 0x02,
	/// Milliseconds a key will stay unlocked since last application use.
	SETTINGS_LOCKTIME = 0x10,
};


class Settings {

	public:
		/// Creates instance with system environment defaults.
		Settings();
		/**
		 * \param Prepare system for reading and/or writing settings.
		 *
		 * Create necessary directories.
		 *
		 * \return ERR_OK if successful, or \c errno of any error the has occurred.
		 */
		int init();
		/**
		 * \brief Retrieve a single settings entry.
		 *
		 * \param typ Settings entry type to retrieve.
		 * \return Result string value. An empty string is returned if no value is set.
		 */
		std::string get(SettingsType typ);
		/**
		 * \brief Set a single settings entry.
		 *
		 * \param typ Settings entry type to set.
		 * \param v Value to set for entry.
		 * \return ERR_OK if successful, ERR_FAIL if invalid settings type.
		 */
		int set(SettingsType typ, std::string v);

	private:
		std::string m_data;
		std::string m_run;
		std::string m_locktime;
};

#endif
