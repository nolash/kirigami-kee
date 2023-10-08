#include <gcrypt.h>
#include <QDebug>

#include "gpg.h"

#define GPG_MIN_VERSION "1.10.2"

const char *gpgVersion = nullptr;

int GpgStore::check(std::string p) {
	const char *v;
	if (gpgVersion == nullptr) {
		v = gcry_check_version(GPG_MIN_VERSION);
		if (v == nullptr) {
			return 1;
		}
	}
	gpgVersion = v;
	qDebug() << "Using gpg version" << QString::fromLocal8Bit(gpgVersion);
	return 0;
}
